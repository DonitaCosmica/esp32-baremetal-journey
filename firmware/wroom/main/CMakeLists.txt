idf_component_register(
  SRCS "main.c" "tx.c"
  INCLUDE_DIRS "."
  REQUIRES nvs_flash esp_wifi esp_netif esp_event esp_timer log proto
)