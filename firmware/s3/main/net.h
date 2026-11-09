#pragma once
#include <stdint.h>

void net_start(void);
void net_publish_led(uint32_t);
void net_publish_proximity(uint32_t);