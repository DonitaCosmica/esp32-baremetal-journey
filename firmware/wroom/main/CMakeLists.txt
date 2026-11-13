idf_component_register(
  SRCS "main.c" "tx.c" "sensors.c"
  INCLUDE_DIRS "."
  REQUIRES nvs_flash esp_wifi esp_netif esp_event esp_timer log proto fc51 ds18b20 tsw20 tds yfs201 ph4502c dht11 mq135 led
)