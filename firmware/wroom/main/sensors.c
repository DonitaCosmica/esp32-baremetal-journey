#include <math.h>
#include <stdbool.h>
#include "esp_random.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "board.h"
#include "fc51.h"
#include "ds18b20.h"
#include "led.h"
#include "metrics.h"
#include "sensors.h"

#define DEBOUNCE_US 50000

static fc51_t prox;
static ds18b20_t water_temp;

static float read_proximity(void) {
  return fc51_detected(&prox) ? 1.f : 0.f;
}

static float read_temp_c(void) {
  return ds18b20_read(&water_temp);
}

static float read_heap(void) {
  return (float)esp_get_free_heap_size();
}

static float rnd(void) {
  return (float)(esp_random() % 2001) / 1000.f - 1.f;
}

static float walk(float *value, float lo, float hi, float step) {
  *value += rnd() * step;
  if(*value < lo) *value = lo;
  if(*value > hi) *value = hi;
  return *value;
}

static float sim_ph(void) {
  static float value = 7.2f;
  return walk(&value, 6.5f, 8.5f, 0.02f);
}

static float sim_turbidity(void) {
  static float value = 1.4f;
  return walk(&value, 0.2f, 4.0f, 0.05f);
}

static float sim_flow(void) {
  static float value = 12.f;
  return walk(&value, 5.f, 20.f, 0.3f);
}

static float sim_battery(void) {
  static float value = 3.85f;
  return walk(&value, 3.4f, 4.2f, 0.005f);
}

typedef struct {
  metric_id_t id;
  float (*read)(void);
  uint32_t sample_ms;
  float deadband;
  float value, last_sent;
  bool valid;
  int64_t t_sample;
} sensor_t;

static sensor_t SENSOR_TABLE[] = {
  { .id = M_OBJECT_DETECTED,  .read = read_proximity, .sample_ms = 10, .deadband = 0.5f },
  { .id = M_PH,               .read = sim_ph,         .sample_ms = 500 },
  { .id = M_TURBIDITY_NTU,    .read = sim_turbidity,  .sample_ms = 500 },
  { .id = M_WATER_TEMP_C,     .read = read_temp_c,    .sample_ms = 1000 },
  { .id = M_FLOW_LPS,         .read = sim_flow,       .sample_ms = 500 },
  { .id = M_FREE_HEAP_BYTES,  .read = read_heap,      .sample_ms = 2000 },
  { .id = M_BATTERY_V,        .read = sim_battery,    .sample_ms = 5000 },
};

#define N_SENSORS (sizeof SENSOR_TABLE / sizeof SENSOR_TABLE[0])

static bool sample(sensor_t *sensor, int64_t now) {
  float value = sensor->read();
  sensor->t_sample = now;

  if(isnan(value)) {
    sensor->valid = false;
    return false;
  }

  bool urgent = sensor->valid && sensor->deadband > 0 && fabsf(value - sensor->last_sent) >= sensor->deadband;
  sensor->value = value;
  sensor->valid = true;
  return urgent;
}


void sensors_init(void) {
  fc51_init(&prox, PROX_PIN);
  ds18b20_init(&water_temp, TEMP_PIN);

  int64_t now = esp_timer_get_time();
  
  for(size_t i = 0; i < N_SENSORS; ++i) {
    sample(&SENSOR_TABLE[i], now);
    SENSOR_TABLE[i].last_sent = SENSOR_TABLE[i].value;
  }
}

bool sensors_poll(int64_t now_us) {
  bool urgent = false;

  for(size_t i = 0; i < N_SENSORS; ++i) {
    if(now_us - SENSOR_TABLE[i].t_sample >= (int64_t)SENSOR_TABLE[i].sample_ms * 1000) {
      urgent |= sample(&SENSOR_TABLE[i], now_us);
    }
  }

  return urgent;
}

void sensors_fill(proto_frame_t *frame) {
  for(size_t i = 0; i < N_SENSORS; ++i) {
    if(!SENSOR_TABLE[i].valid) continue;
    if(!proto_add(frame, SENSOR_TABLE[i].id, SENSOR_TABLE[i].value)) break;
    SENSOR_TABLE[i].last_sent = SENSOR_TABLE[i].value;
  }
}