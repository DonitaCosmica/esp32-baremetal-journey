#pragma once
#include <stdint.h>

#define REG32(a) (*(volatile uint32_t *)(a))

#define SENSOR_PIN 4

#define GPIO_BASE 0x3FF44000u
#define GPIO_IN (GPIO_BASE + 0x3Cu)
#define IOMUX_GPIO4 0x3FF49048u
#define IOMUX_MCU_SEL_SHIFT 12
#define IOMUX_FUN_IE (1u << 9)
#define GPIO_FUNC_SEL 2u

static inline void sensor_init(void) {
  uint32_t v = REG32(IOMUX_GPIO4);
  v &= ~(7u << IOMUX_MCU_SEL_SHIFT);
  v |= (GPIO_FUNC_SEL << IOMUX_MCU_SEL_SHIFT) | IOMUX_FUN_IE;
  REG32(IOMUX_GPIO4) = v;
}

static inline uint8_t sensor_detected(void) {
  return ((REG32(GPIO_IN) >> SENSOR_PIN) & 1u) ? 0 : 1;
}