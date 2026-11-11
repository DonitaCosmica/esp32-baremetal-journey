idf_component_register(
  SRCS "main.c" "net.c" "link.c" "display.c" "nodes.c" "telemetry.c" "ui.c"
  INCLUDE_DIRS "."
  REQUIRES nvs_flash esp_wifi esp_netif esp_event mqtt esp_timer log driver json lwip proto
)