/*
 * Keccak256 implementation for TRON address generation
 * Adapted from OpenCL reference implementation
 */

#ifndef KECCAK256_H
#define KECCAK256_H

#include <stdint.h>
#include <string.h>

void keccak256(const uint8_t* input, size_t inputLen, uint8_t* output);

#endif // KECCAK256_H
