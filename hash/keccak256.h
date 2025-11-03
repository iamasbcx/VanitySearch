/*
 * Keccak-256 hash implementation for TRON address generation
 */

#ifndef KECCAK256_H
#define KECCAK256_H

#include <stdint.h>
#include <stddef.h>

void keccak256(const uint8_t* input, size_t length, uint8_t* output);

#endif // KECCAK256_H
