#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "proto.h"

void sensors_init(void);
bool sensors_poll(uint64_t);
void sensors_fill(proto_frame_t*);