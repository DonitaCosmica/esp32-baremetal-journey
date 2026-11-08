#pragma once
#include <stdint.h>

#define REG32(a) (*(volatile uint32_t *)(a))

#define LED_PIN 2
#define BTN_PIN 5

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

static inline void hw_gpio_init(void) {
  iomux_set_gpio(LED_PIN, 0);
  REG32(GPIO_FUNC_OUT_SEL(LED_PIN)) = 0x100;
  REG32(GPIO_ENABLE_W1TS) = (1u << LED_PIN);
  iomux_set_gpio(BTN_PIN, IOMUX_FUN_IE | IOMUX_FUN_WPU);
}