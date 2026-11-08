#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "net.h"
#include "secrets.h"

#define TOPIC_LED "telemetry/s3/led"
#define TOPIC_STATUS "telemetry/s3/status"

static const char *TAG = "net";
static esp_mqtt_client_handle_t mqtt;
static volatile bool mqtt_ok = false;
static volatile bool mqtt_started = false;
static volatile int last_led = -1; // último estado, para republicar al reconectar

static void publish_led(int on) {
  char buf[64];
  int n = snprintf(buf, sizeof buf, "{\"led\":%d,\"ts\":%lu}", on,
    (unsigned long)(esp_timer_get_time() / 1000));
  
  esp_mqtt_client_publish(mqtt, TOPIC_LED, buf, n, 1, 1);
}

static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
  if(base == WIFI_EVENT && id == WIFI_EVENT_STA_START) esp_wifi_connect();
  else if(base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
    wifi_event_sta_disconnected_t *d = (wifi_event_sta_disconnected_t *)data;
    ESP_LOGW(TAG, "Wi-Fi desconectado, motivo=%d", d->reason);
    esp_wifi_connect();
  } else if(base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *e = (ip_event_got_ip_t *)data;
    ESP_LOGI(TAG, "IP asignada: " IPSTR, IP2STR(&e->ip_info.ip));
    if(!mqtt_started) {
      mqtt_started = true;
      esp_mqtt_client_start(mqtt);
    }
  }
}

static void on_mqtt(void *arg, esp_event_base_t base, int32_t id, void *data) {
  switch((esp_mqtt_event_id_t)id) {
    case MQTT_EVENT_CONNECTED: {
      ESP_LOGI(TAG, "MQTT conectado");
      mqtt_ok = true;
      esp_mqtt_client_publish(mqtt, TOPIC_STATUS, "online", 0, 1, 1);
      if(last_led >= 0) publish_led(last_led);
      break;
    }

    case MQTT_EVENT_DISCONNECTED: {
      ESP_LOGW(TAG, "MQTT desconectado");
      mqtt_ok = false;
      break;
    }

    default: break;
  }
}

void net_publish_led(uint32_t on) {
  last_led = (int)on;
  if(mqtt_ok) publish_led((int)on);
}

void net_start(void) {
  esp_err_t r = nvs_flash_init();
  if(r == ESP_ERR_NVS_NO_FREE_PAGES || r == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    r = nvs_flash_init();
  }

  ESP_ERROR_CHECK(r);
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi, NULL));
  ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_wifi, NULL));

  wifi_config_t wc = { 0 };
  strlcpy((char *)wc.sta.ssid, WIFI_SSID, sizeof wc.sta.ssid);
  strlcpy((char *)wc.sta.password, WIFI_PASS, sizeof wc.sta.password);
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wc));

  esp_mqtt_client_config_t mc = {
    .broker.address.uri = MQTT_URI,
    .credentials.client_id = "s3-led",
    .session.last_will = {
      .topic = TOPIC_STATUS, .msg = "offline", .qos = 1, .retain = 1,
    },
  };

  mqtt = esp_mqtt_client_init(&mc);
  esp_mqtt_client_register_event(mqtt, ESP_EVENT_ANY_ID, on_mqtt, NULL);

  ESP_ERROR_CHECK(esp_wifi_start());
}