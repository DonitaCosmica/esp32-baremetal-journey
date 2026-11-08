#include <stdint.h>

#define REG32(a) (*(volatile uint32_t *)(a))

#define LED_PIN 2
#define BTN_PIN 5

#define CPU_HZ 20000000u
#define DEBOUNCE_MS 30u

#define GPIO_BASE 0x60004000u
#define GPIO_OUT_W1TS (GPIO_BASE + 0x08u)
#define GPIO_OUT_W1TC (GPIO_BASE + 0x0Cu)
#define GPIO_ENABLE_W1TS (GPIO_BASE + 0x24u)
#define GPIO_IN (GPIO_BASE + 0x3Cu)
#define GPIO_FUNC_OUT_SEL(n) (GPIO_BASE + 0x554u + 4u*(n))

#define IOMUX_PIN(n) (0x60009004u + 4u*(n))
#define IOMUX_MCU_SEL_SHIFT 12
#define IOMUX_FUN_WPU (1u << 8)
#define IOMUX_FUN_IE (1u << 9)
#define GPIO_FUNC_SEL 1u

#define RTC_BASE 0x60008000u
#define RTC_WDTCONFIG0 (RTC_BASE + 0x98u)
#define RTC_WDTWPROTECT (RTC_BASE + 0xB0u)
#define RTC_SWD_CONF (RTC_BASE + 0xB4u)
#define RTC_SWD_WPROTECT (RTC_BASE + 0xB8u)
#define TIMG0_BASE 0x6001F000u
#define TIMG0_WDTCONFIG0 (TIMG0_BASE + 0x48u)
#define TIMG0_WDTWPROTECT (TIMG0_BASE + 0x64u)
#define WDT_KEY 0x50D83AA1u
#define SWD_KEY 0x8F1D312Au

extern uint32_t _bss_start, _bss_end;

static void wdt_disable(void) {
  REG32(TIMG0_WDTWPROTECT) = WDT_KEY;
  REG32(TIMG0_WDTCONFIG0) = 0;
  REG32(TIMG0_WDTWPROTECT) = 0;

  REG32(RTC_WDTWPROTECT) = WDT_KEY;
  REG32(RTC_WDTCONFIG0) = 0;
  REG32(RTC_WDTWPROTECT) = 0;

  REG32(RTC_SWD_WPROTECT) = SWD_KEY;
  REG32(RTC_SWD_CONF) |= (1u << 31);
  REG32(RTC_SWD_WPROTECT) = 0;
}

static inline uint32_t ccount(void) {
  uint32_t c;
  __asm__ volatile("rsr %0, ccount" : "=a"(c));
  return c;
}

static inline void iomux_set_gpio(uint32_t pin, uint32_t extra_bits) {
  uint32_t v = REG32(IOMUX_PIN(pin));
  v &= ~(7u << IOMUX_MCU_SEL_SHIFT);
  v |= (GPIO_FUNC_SEL << IOMUX_MCU_SEL_SHIFT) | extra_bits;
  REG32(IOMUX_PIN(pin)) = v;
}

static inline uint32_t btn_read(void) {
  return (REG32(GPIO_IN) >> BTN_PIN) & 1u; // 1 = suelto, 0 = presionado
}

static inline void led_write(uint32_t on) {
  if(on) REG32(GPIO_OUT_W1TS) = (1u << LED_PIN);
  else REG32(GPIO_OUT_W1TC) = (1u << LED_PIN);
}

void app_main(void) {
  for(uint32_t *p = &_bss_start; p < &_bss_end; ++p) *p = 0;
  wdt_disable();

  iomux_set_gpio(LED_PIN, 0);
  REG32(GPIO_FUNC_OUT_SEL(LED_PIN)) = 0x100;
  REG32(GPIO_ENABLE_W1TS) = (1u << LED_PIN);

  iomux_set_gpio(BTN_PIN, IOMUX_FUN_IE | IOMUX_FUN_WPU);

  uint32_t led_on = 0;
  led_write(led_on);

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
      }
    }
  }
}