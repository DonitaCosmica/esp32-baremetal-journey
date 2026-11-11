#include <math.h>
#include <time.h>
#include "cJSON.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "gw_config.h"
#include "metrics.h"
#include "net.h"
#include "nodes.h"
#include "proto.h"
#include "telemetry.h"

static double round_to(float value, int decimals) {
  double p = 1;
  while(decimals-- > 0) p *= 10;
  return round((double)value * p) / p;
}

static void add_entry(cJSON *g[3], uint8_t id, float value) {
  const metric_info_t *metric = metric_find(id);

  if(!metric) return;
  if(metric->kind == KIND_STATUS) cJSON_AddStringToObject(g[metric->group], metric->name, status_name((uint8_t)value));
  else cJSON_AddNumberToObject(g[metric->group], metric->name, round_to(value, metric->decimals));
}

static cJSON *new_root(cJSON *g[3]) {
  cJSON *root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "ts", (double)time(NULL));
  cJSON_AddStringToObject(root, "via", GW_DEVICE_ID);

  g[GRP_METRICS] = cJSON_AddObjectToObject(root, "metrics");
  g[GRP_HEALTH] = cJSON_AddObjectToObject(root, "sensors_health");
  g[GRP_DIAG] = cJSON_AddObjectToObject(root, "diagnostics");

  return root;
}

static void publish(const char *device, cJSON *root) {
  char *str = cJSON_PrintUnformatted(root);

  if(str) {
    net_publish(device, "telemetry", str, 1, true);
    cJSON_free(str);
  }
}

void telemetry_from_node(const uint8_t *frame, size_t len, int8_t rssi) {
  proto_hdr_t hdr;
  if(!proto_parse(frame, len, &hdr)) return;

  cJSON *g[3];
  cJSON *root = new_root(g);

  for(size_t i = 0; i < hdr.count; ++i) {
    uint8_t id;
    float value;
    proto_get(frame, i, &id, &value);
    add_entry(g, id, value);
  }

  add_entry(g, M_RSSI_DBM, rssi);

  char dev[24];
  node_device_id(dev, sizeof dev, hdr.node_id);
  publish(dev, root);
}

void telemetry_from_gateway(uint32_t led_on) {
  net_info_t net_info;
  net_get_info(&net_info);

  cJSON *g[3];
  cJSON *root = new_root(g);

  add_entry(g, M_LED, (float)led_on);
  add_entry(g, M_RSSI_DBM, net_info.rssi);
  add_entry(g, M_FREE_HEAP_BYTES, (float)esp_get_free_heap_size());
  add_entry(g, M_UPTIME_S, (float)(esp_timer_get_time() / 1000000));

  publish(GW_DEVICE_ID, root);
}