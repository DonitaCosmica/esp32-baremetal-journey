#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "display.h"
#include "gw_config.h"
#include "net.h"
#include "nodes.h"
#include "ui.h"

#define NODE_ROWS 3

static void render(void) {
  net_info_t net_info;
  net_get_info(&net_info);

  node_t nodes[MAX_NODES];
  size_t n = nodes_snapshot(nodes, MAX_NODES);
  int online = 0;
  
  for(size_t i = 0; i < n; ++i) online += nodes[i].online;
  int64_t now = esp_timer_get_time();

  display_clear();
  display_printf(0, "GW %s", GW_DEVICE_ID);
  display_printf(1, "WiFi %s", net_info.wifi_up ? net_info.ssid : "sin conexion");
  display_printf(2, "%s", net_info.wifi_up ? net_info.ip : "-");
  display_printf(3, "ch%d %ddBm MQTT %s", net_info.channel, net_info.rssi, net_info.mqtt_up ? "OK" : "--");
  display_printf(4, "--- Nodos %d/%d ---", online, (int)n);

  for(size_t i = 0; i < n && i < NODE_ROWS; ++i) {
    int age = (int)((now - nodes[i].last_seen_us) / 1000000);
    display_printf(5 + (int)i, "N%02d %s %ddBm %ds", nodes[i].node_id, nodes[i].online ? "OK" : "OFF", nodes[i].rssi, age);
  }

  display_flush();
}

static void ui_task(void *arg) {
  for(;;) {
    render();
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ui_start(void) {
  xTaskCreate(ui_task, "ui", 4096, NULL, 3, NULL);
}