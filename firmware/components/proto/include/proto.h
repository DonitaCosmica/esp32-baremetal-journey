#pragma once
#include <stdint.h>

#define PROTO_MAGIC 0xA5

typedef enum { MSG_PROXIMITY = 1 } msg_type_t;

typedef struct __attribute__((packed)) {
  uint8_t magic;
  uint8_t type;
  uint8_t detected;
  uint8_t seq;
} proto_msg_t;