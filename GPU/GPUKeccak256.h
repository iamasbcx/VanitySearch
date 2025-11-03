/*
 * GPU Keccak-256 hash implementation for TRON address generation
 */

#ifndef GPUKECCAK256_H
#define GPUKECCAK256_H

// ---------------------------------------------------------------------------------
// Keccak-256 for TRON
// ---------------------------------------------------------------------------------

__device__ __constant__ uint64_t keccakf_rndc[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL,
    0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL,
    0x000000000000008aULL, 0x0000000000000088ULL,
    0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL,
    0x8000000000008089ULL, 0x8000000000008003ULL,
    0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL,
    0x8000000080008081ULL, 0x8000000000008080ULL,
    0x0000000080000001ULL, 0x8000000080008008ULL
};

__device__ __constant__ unsigned int keccakf_rotc[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
};

__device__ __constant__ unsigned int keccakf_piln[24] = {
    10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1
};

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

__device__ void keccakf_gpu(uint64_t st[25]) {
    int i, j, r;
    uint64_t t, bc[5];

    for (r = 0; r < 24; r++) {
        // Theta
        for (i = 0; i < 5; i++)
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];

        for (i = 0; i < 5; i++) {
            t = bc[(i + 4) % 5] ^ ROTL64(bc[(i + 1) % 5], 1);
            for (j = 0; j < 25; j += 5)
                st[j + i] ^= t;
        }

        // Rho Pi
        t = st[1];
        for (i = 0; i < 24; i++) {
            j = keccakf_piln[i];
            bc[0] = st[j];
            st[j] = ROTL64(t, keccakf_rotc[i]);
            t = bc[0];
        }

        // Chi
        for (j = 0; j < 25; j += 5) {
            for (i = 0; i < 5; i++)
                bc[i] = st[j + i];
            for (i = 0; i < 5; i++)
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
        }

        // Iota
        st[0] ^= keccakf_rndc[r];
    }
}

__device__ void keccak256_gpu(const uint8_t* input, size_t length, uint8_t* output) {
    uint64_t st[25];
    size_t rsiz = 136; // 200 - 2 * 32 = 136 (rate for SHA3-256)
    size_t pt = 0;

    // Initialize state
    for (int i = 0; i < 25; i++)
        st[i] = 0;

    // Absorb input
    for (size_t i = 0; i < length; i++) {
        ((uint8_t*)st)[pt++] ^= input[i];
        if (pt >= rsiz) {
            keccakf_gpu(st);
            pt = 0;
        }
    }

    // Padding
    ((uint8_t*)st)[pt] ^= 0x01;
    ((uint8_t*)st)[rsiz - 1] ^= 0x80;
    keccakf_gpu(st);

    // Output (copy first 32 bytes)
    for (int i = 0; i < 32; i++) {
        output[i] = ((uint8_t*)st)[i];
    }
}

// Helper function to compute TRON address from public key
__device__ void computeTronAddress(uint8_t* pubkey, uint8_t* address) {
    uint8_t hash[32];
    
    // Compute Keccak-256 hash of public key (64 bytes)
    keccak256_gpu(pubkey, 64, hash);
    
    // TRON address is the last 20 bytes of the hash with 0x41 prefix
    address[0] = 0x41;
    for (int i = 0; i < 20; i++) {
        address[i + 1] = hash[i + 12];
    }
}

#endif // GPUKECCAK256_H
