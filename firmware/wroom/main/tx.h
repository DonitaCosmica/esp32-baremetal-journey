#pragma once
#include <stdint.h>
#include <stdbool.h>

void tx_start(void);
void tx_send(const uint8_t*, size_t);
bool tx_linked(void);