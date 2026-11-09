#include "esp_log.h"
#include "esp_now.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "proto.h"
#include "link.h"

static const char *TAG = "link";
static QueueHandle_t q;
static link_cb_t user_cb;

static void on_recv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if(len != (int)sizeof(proto_msg_t)) return;

  const proto_msg_t *m = (const proto_msg_t *)data;
  if(m->magic != PROTO_MAGIC || m->type != MSG_PROXIMITY) return;

  uint8_t d = m->detected ? 1 : 0;
  xQueueSend(q, &d, 0);
}

static void link_task(void *arg) {
  int last = -1;
  uint8_t d;
  for(;;) {
    if(xQueueReceive(q, &d, portMAX_DELAY) == pdTRUE && d != last) {
      last = d;
      ESP_LOGI(TAG, "proximidad: %d", d);
      user_cb(d);
    }
  }
}

void link_start(link_cb_t cb) {
  user_cb = cb;
  q = xQueueCreate(8, sizeof(uint8_t));
  xTaskCreate(link_task, "link", 4096, NULL, 5, NULL);
  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_recv_cb(on_recv));
}