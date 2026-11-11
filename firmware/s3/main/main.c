#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "display.h"
#include "gw_config.h"
#include "hw.h"
#include "link.h"
#include "net.h"
#include "telemetry.h"
#include "ui.h"

#define CPU_HZ (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000u)
#define DEBOUNCE_MS 30u

void app_main(void) {
  display_init();
  net_start();
  link_start();
  ui_start();
  hw_gpio_init();

  uint32_t led_on = 0;
  led_write(led_on);

  const uint32_t debounce_ticks = DEBOUNCE_MS * (CPU_HZ / 1000u);
  uint32_t stable = btn_read();
  uint32_t last = stable;
  uint32_t t_change = ccount();
  uint64_t t_gw = 0;

  while(1) {
    uint32_t raw = btn_read();

    if(raw != last) {
      last = raw;
      t_change = ccount();
    }

    if(raw !=stable && (ccount() - t_change) >= debounce_ticks) {
      stable = raw;
      if(stable == 0) {
        led_on ^= 1u;
        led_write(led_on);
        telemetry_from_gateway(led_on);
        t_gw = esp_timer_get_time();
      }
    }

    int64_t now = esp_timer_get_time();
    if(now - t_gw >= (int64_t)GW_REPORT_MS * 1000) {
      telemetry_from_gateway(led_on);
      t_gw = now;
    }

    vTaskDelay(1);
  }
}