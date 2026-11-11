#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROTO_MAGIC 0xA5
#define PROTO_VERSION 1
#define PROTO_MAX_FRAME 250 // limite de ESP-NOW

typedef struct __attribute__((packed)) {
  uint8_t magic, version, node_id, seq, count; // 5 bytes
} proto_hdr_t;

typedef struct __attribute__((packed)) {
  uint8_t id;
  float value;
} proto_entry_t; // 5 bytes: hasta 49 metricas por trama

typedef struct {
  uint8_t buf[PROTO_MAX_FRAME];
  size_t len;
} proto_frame_t;

/* Emisor */
void proto_begin(proto_frame_t*, uint8_t, uint8_t);
bool proto_add(proto_frame_t*, uint8_t, float); // false si no cabe

/* Receptor */
bool proto_parse(const uint8_t*, size_t, proto_hdr_t*); // valida longitud y version
void proto_get(const uint8_t*, size_t, uint8_t*, float*);