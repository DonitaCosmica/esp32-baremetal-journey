#include <string.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_now.h"
#include "nvs_flash.h"
#include "proto.h"
#include "config.h"
#include "tx.h"

static const char *TAG = "tx";
static const uint8_t S3_MAC[6] = S3_MAC_ADDR;
static volatile int fails = 0;
static uint8_t channel = ESPNOW_CHANNEL; 

static void on_sent(const uint8_t *mac, esp_now_send_status_t status) {
  if(status == ESP_NOW_SEND_SUCCESS) {
    if(fails > 0) ESP_LOGI(TAG, "enlazado en canal %d", channel);
    fails = 0;
  } else {
    fails++;
  }
}

static void hop_channel(void) {
  channel = channel % 13 + 1;
  ESP_ERROR_CHECK(esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE));
  ESP_LOGW(TAG, "probando canal %d", channel);
  fails = 1;
}

void tx_start(void) {
  esp_err_t r = nvs_flash_init();
  if(r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    r = nvs_flash_init();
  }

  ESP_ERROR_CHECK(r);
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));

  ESP_ERROR_CHECK(esp_now_init());
  ESP_ERROR_CHECK(esp_now_register_send_cb(on_sent));

  esp_now_peer_info_t peer = { 0 };
  memcpy(peer.peer_addr, S3_MAC, 6);
  peer.channel = 0;
  peer.ifidx = WIFI_IF_STA;
  peer.encrypt = false;
  ESP_ERROR_CHECK(esp_now_add_peer(&peer));
}

void tx_send_proximity(uint8_t detected) {
  if(fails >= 2) hop_channel();

  static uint8_t seq;
  proto_msg_t m= { PROTO_MAGIC, MSG_PROXIMITY, detected ? 1 : 0, seq++ };
  esp_err_t r = esp_now_send(S3_MAC, (const uint8_t *)&m, sizeof m);
  if(r != ESP_OK) ESP_LOGW(TAG, "esp_now_send: %s", esp_err_to_name(r));
}

bool tx_linked(void) { return fails == 0; }