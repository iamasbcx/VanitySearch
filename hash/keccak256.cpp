/*
 * Keccak256 implementation for TRON address generation
 * Adapted from OpenCL reference implementation in calc_addrs.cl
 */

#include "keccak256.h"
#include <string.h>

// Keccak256 round constants
static const uint64_t keccakf_rndc[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

// Rotate left
static inline uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

// Keccak-f[1600] permutation
static void keccakf(uint64_t st[25]) {
    for (int round = 0; round < 24; round++) {
        uint64_t bc[5];
        
        // Theta
        for (int i = 0; i < 5; i++) {
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
        }
        
        for (int i = 0; i < 5; i++) {
            uint64_t t = bc[(i + 4) % 5] ^ rotl64(bc[(i + 1) % 5], 1);
            for (int j = 0; j < 25; j += 5) {
                st[j + i] ^= t;
            }
        }
        
        // Rho and Pi
        uint64_t t = st[1];
        for (int i = 0; i < 24; i++) {
            int j = ((i + 1) * (i + 2) / 2) % 25;
            bc[0] = st[j];
            int shift = ((i + 1) * (i + 2) / 2) % 64;
            if (shift == 0) shift = 1;
            st[j] = rotl64(t, shift);
            t = bc[0];
        }
        
        // Chi
        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; i++) {
                bc[i] = st[j + i];
            }
            for (int i = 0; i < 5; i++) {
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }
        
        // Iota
        st[0] ^= keccakf_rndc[round];
    }
}

void keccak256(const uint8_t* input, size_t inputLen, uint8_t* output) {
    uint64_t st[25];
    uint8_t temp[144];
    size_t rsiz = 136; // 200 - 2 * 256 / 8
    size_t rsizw = rsiz / 8;
    
    memset(st, 0, sizeof(st));
    
    // Absorb input
    for (size_t i = 0; i < inputLen; ) {
        memset(temp, 0, sizeof(temp));
        size_t len = inputLen - i;
        if (len > rsiz) len = rsiz;
        memcpy(temp, input + i, len);
        
        // Padding
        if (len < rsiz) {
            temp[len] = 0x01;
            temp[rsiz - 1] |= 0x80;
        }
        
        // XOR with state
        for (size_t j = 0; j < rsizw; j++) {
            uint64_t val = 0;
            for (int k = 0; k < 8; k++) {
                val |= ((uint64_t)temp[j * 8 + k]) << (8 * k);
            }
            st[j] ^= val;
        }
        
        keccakf(st);
        i += len;
        
        if (len < rsiz) break;
    }
    
    // If we didn't pad yet, pad now
    if (inputLen % rsiz == 0 && inputLen > 0) {
        memset(temp, 0, sizeof(temp));
        temp[0] = 0x01;
        temp[rsiz - 1] = 0x80;
        for (size_t j = 0; j < rsizw; j++) {
            uint64_t val = 0;
            for (int k = 0; k < 8; k++) {
                val |= ((uint64_t)temp[j * 8 + k]) << (8 * k);
            }
            st[j] ^= val;
        }
        keccakf(st);
    }
    
    // Squeeze output
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            output[i * 8 + j] = (st[i] >> (8 * j)) & 0xFF;
        }
    }
}
