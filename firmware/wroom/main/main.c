#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "hw.h"
#include "tx.h"

#define DEBOUNCE_US 50000
#define HEARTBEAT_US 2000000

static const char *TAG = "wroom";

void app_main(void) {
  sensor_init();
  tx_start();

  uint8_t stable = sensor_detected();
  uint8_t last = stable;
  int64_t t_change = esp_timer_get_time();
  int64_t t_tx = 0;

  ESP_LOGI(TAG, "estado inicial: %s", stable ? "objeto detectado" : "objeto no detectado");
  tx_send_proximity(stable);
  t_tx = esp_timer_get_time();

  while(1) {
    int64_t now = esp_timer_get_time();
    uint8_t raw = sensor_detected();

    if(raw != last) { last = raw; t_change = now; }

    if(raw != stable && (now - t_change) >= DEBOUNCE_US) {
      stable = raw;
      ESP_LOGI(TAG, "cambio: %s", stable ? "objeto detectado" : "objeto no detectado");
      tx_send_proximity(stable);
      t_tx = now;
    }

    if((now - t_tx) >= HEARTBEAT_US) {
      tx_send_proximity(stable);
      t_tx = now;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}