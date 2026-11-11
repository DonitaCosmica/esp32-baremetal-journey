#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "config.h"
#include "metrics.h"
#include "proto.h"
#include "sensors.h"
#include "tx.h"

static void send_frame(void) {
  static uint8_t seq;
  proto_frame_t frame;
  proto_begin(&frame, NODE_ID, seq++);
  sensors_fill(&frame);
  proto_add(&frame, M_UPTIME_S, (float)(esp_timer_get_time() / 1000000));
  tx_send(frame.buf, frame.len);
}

void app_main(void) {
  tx_start();
  sensors_init();

  int64_t t_tx = esp_timer_get_time();
  send_frame();

  while(1) {
    int64_t now =esp_timer_get_time();
    bool urgent = sensors_poll(now);

    if(urgent || (now - t_tx) >= (int64_t)REPORT_MS * 1000) {
      send_frame();
      t_tx = now;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}