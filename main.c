#include <stdint.h>

#define REG32(a) (*(volatile uint32_t *)(a))
#define LED_PIN 2

/* ---------- Direcciones por chip ---------- */
#if defined(TARGET_esp32s3)
  #define GPIO_BASE 0x60004000u
  #define GPIO_FUNC_OUT_SEL (GPIO_BASE + 0x554u + 4u*LED_PIN)
  #define IOMUX_PIN (0x60009004u + 4u*LED_PIN)
  #define GPIO_FUNC_SEL 1u
  #define RTC_BASE 0x60008000u
  #define RTC_WDTCONFIG0 (RTC_BASE + 0x98u)
  #define RTC_WDTWPROTECT (RTC_BASE + 0xB0u)
  #define RTC_SWD_CONF (RTC_BASE + 0xB4u)
  #define RTC_SWD_WPROTECT (RTC_BASE + 0xB8u)
  #define TIMG0_BASE 0x6001F000u
  #define CPU_HZ 20000000u /* al salir del ROM el CPU va con el XTAL de 20 MHz (verificar) */
#elif defined(TARGET_esp32)
  #define GPIO_BASE 0x3FF44000u
  #define GPIO_FUNC_OUT_SEL (GPIO_BASE + 0x530u + 4u*LED_PIN)
  #define IOMUX_PIN 0x3FF49040u            /* GPIO2 */
  #define GPIO_FUNC_SEL 2u
  #define RTC_BASE 0x3FF48000u
  #define RTC_WDTCONFIG0 (RTC_BASE + 0x8Cu)
  #define RTC_WDTWPROTECT (RTC_BASE + 0xA4u)
  #define TIMG0_BASE 0x3FF5F000u
  #define CPU_HZ 40000000u /* al salir del ROM el CPU va con el XTAL de 40 MHz (verificar) */
#endif

#define GPIO_OUT_W1TS (GPIO_BASE + 0x08u)
#define GPIO_OUT_W1TC (GPIO_BASE + 0x0Cu)
#define GPIO_ENABLE_W1TS (GPIO_BASE + 0x24u)

#define TIMG0_WDTCONFIG0 (TIMG0_BASE + 0x48u)
#define TIMG0_WDTWPROTECT (TIMG0_BASE + 0x64u)

#define WDT_KEY 0x50D83AA1u
#define SWD_KEY 0x8F1D312Au

#define IOMUX_MCU_SEL_SHIFT 12

extern uint32_t _bss_start, _bss_end;

static void wdt_disable(void) {
  /* Watchdog del timer group 0 */
  REG32(TIMG0_WDTWPROTECT) = WDT_KEY;
  REG32(TIMG0_WDTCONFIG0) = 0;
  REG32(TIMG0_WDTWPROTECT) = 0;

  /* Watchdog del RTC */
  REG32(RTC_WDTWPROTECT) = WDT_KEY;
  REG32(RTC_WDTCONFIG0) = 0;
  REG32(RTC_WDTWPROTECT) = 0;

#if defined(TARGET_esp32s3)
  /* Super watchdog: se desactiva con auto-feed (bit 31) */
  REG32(RTC_SWD_WPROTECT) = SWD_KEY;
  REG32(RTC_SWD_CONF) |= (1u << 31);
  REG32(RTC_SWD_WPROTECT) = 0;
#endif
}

static inline uint32_t ccount(void) {
  uint32_t c;
  __asm__ volatile("rsr %0, ccount" : "=a"(c)); /* contador de ciclos, como DWT/SysTick */
  return c;
}

static void delay_ms(uint32_t ms) {
  uint32_t t0 = ccount();
  uint32_t n = ms * (CPU_HZ / 1000u);
  while ((ccount() - t0) < n);
}

void app_main(void) {
  for (uint32_t *p = &_bss_start; p < &_bss_end; p++) *p = 0;
  wdt_disable();

  REG32(IOMUX_PIN) = (REG32(IOMUX_PIN) & ~(7u << IOMUX_MCU_SEL_SHIFT)) | (GPIO_FUNC_SEL << IOMUX_MCU_SEL_SHIFT);
  REG32(GPIO_FUNC_OUT_SEL) = 0x100; /* salida = registro GPIO_OUT */
  REG32(GPIO_ENABLE_W1TS) = (1u << LED_PIN); /* pin como salida */

  while (1) {
    REG32(GPIO_OUT_W1TS) = (1u << LED_PIN); /* ON  */
    delay_ms(1000);
    REG32(GPIO_OUT_W1TC) = (1u << LED_PIN); /* OFF */
    delay_ms(1000);
  }
}