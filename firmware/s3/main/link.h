#pragma once
#include <stdint.h>

typedef void (*link_cb_t)(uint8_t);

void link_start(link_cb_t);