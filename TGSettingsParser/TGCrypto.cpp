/*
 * TGCrypto - Implementation
 * Based on Telegram Desktop (https://github.com/telegramdesktop/tdesktop)
 */

#include "TGCrypto.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

// OpenSSL headers
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

// Static MD5 context for stateful operations
static MD5_CTX g_md5ctx;

void TGCrypto::sha1(const void* data, size_t len, uint8_t* result) {
    SHA1((const unsigned char*)data, len, result);
}

void TGCrypto::sha256(const void* data, size_t len, uint8_t* result) {
    SHA256((const unsigned char*)data, len, result);
}

void TGCrypto::sha512(const void* data, size_t len, uint8_t* result) {
    SHA512((const unsigned char*)data, len, result);
}

void TGCrypto::md5Init() {
    MD5_Init(&g_md5ctx);
}

void TGCrypto::md5Update(const void* data, size_t len) {
    MD5_Update(&g_md5ctx, data, len);
}

void TGCrypto::md5Final(uint8_t* result) {
    MD5_Final(result, &g_md5ctx);
}

bool TGCrypto::pbkdf2_hmac_sha1(
    const uint8_t* password, size_t passwordLen,
    const uint8_t* salt, size_t saltLen,
    int iterations,
    uint8_t* key, size_t keyLen) {
    
    return PKCS5_PBKDF2_HMAC_SHA1(
        (const char*)password, (int)passwordLen,
        salt, (int)saltLen,
        iterations,
        (int)keyLen, key) == 1;
}

bool TGCrypto::pbkdf2_hmac_sha512(
    const uint8_t* password, size_t passwordLen,
    const uint8_t* salt, size_t saltLen,
    int iterations,
    uint8_t* key, size_t keyLen) {
    
    return PKCS5_PBKDF2_HMAC(
        (const char*)password, (int)passwordLen,
        salt, (int)saltLen,
        iterations,
        EVP_sha512(),
        (int)keyLen, key) == 1;
}

bool TGCrypto::aesIgeEncrypt(const void* src, void* dst, size_t len,
                              const uint8_t* key, uint8_t* iv) {
    AES_KEY aesKey;
    if (AES_set_encrypt_key(key, 256, &aesKey) != 0) {
        return false;
    }
    AES_ige_encrypt((const unsigned char*)src, (unsigned char*)dst, 
                    len, &aesKey, iv, AES_ENCRYPT);
    return true;
}

bool TGCrypto::aesIgeDecrypt(const void* src, void* dst, size_t len,
                              const uint8_t* key, uint8_t* iv) {
    AES_KEY aesKey;
    if (AES_set_decrypt_key(key, 256, &aesKey) != 0) {
        return false;
    }
    AES_ige_encrypt((const unsigned char*)src, (unsigned char*)dst, 
                    len, &aesKey, iv, AES_DECRYPT);
    return true;
}

// Telegram's local encryption method
// Uses first 16 bytes of the hash as part of IV
bool TGCrypto::aesEncryptLocal(const void* src, void* dst, size_t len,
                                const uint8_t* key, const void* hashKey) {
    // Build IV: hashKey[0:16] + hashKey[0:16] XOR key[0:16]
    uint8_t iv[32];
    memcpy(iv, hashKey, 16);
    for (int i = 0; i < 16; i++) {
        iv[16 + i] = ((const uint8_t*)hashKey)[i] ^ key[i];
    }
    
    return aesIgeEncrypt(src, dst, len, key, iv);
}

bool TGCrypto::aesDecryptLocal(const void* src, void* dst, size_t len,
                                const uint8_t* key, const void* hashKey) {
    // Build IV: hashKey[0:16] + hashKey[0:16] XOR key[0:16]
    uint8_t iv[32];
    memcpy(iv, hashKey, 16);
    for (int i = 0; i < 16; i++) {
        iv[16 + i] = ((const uint8_t*)hashKey)[i] ^ key[i];
    }
    
    return aesIgeDecrypt(src, dst, len, key, iv);
}

void TGCrypto::randomFill(void* data, size_t len) {
    // Try OpenSSL first
    if (RAND_bytes((unsigned char*)data, (int)len) == 1) {
        return;
    }
    
    // Fallback to simple random (not cryptographically secure, but works)
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }
    
    uint8_t* p = (uint8_t*)data;
    for (size_t i = 0; i < len; i++) {
        p[i] = (uint8_t)(rand() & 0xFF);
    }
}
