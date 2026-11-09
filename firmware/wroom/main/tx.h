#pragma once
#include <stdint.h>
#include <stdbool.h>

void tx_start(void);
void tx_send_proximity(uint8_t);
bool tx_linked(void);