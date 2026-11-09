#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "hw.h"
#include "net.h"
#include "link.h"
#include "display.h"

#define CPU_HZ (CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 1000000u)
#define DEBOUNCE_MS 30u

static void on_proximity(uint8_t detected) {
  display_show_proximity(detected);
  net_publish_proximity(detected);
}

void app_main(void) {
  display_init();
  display_show_waiting();
  net_start();
  link_start(on_proximity);
  hw_gpio_init();

  uint32_t led_on = 0;
  led_write(led_on);
  net_publish_led(led_on);

  const uint32_t debounce_ticks = DEBOUNCE_MS * (CPU_HZ / 1000u);
  uint32_t stable = btn_read();
  uint32_t last = stable;
  uint32_t t_change = ccount();

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
        net_publish_led(led_on);
      }
    }

    vTaskDelay(1);
  }
}