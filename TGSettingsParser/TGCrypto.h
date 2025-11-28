/*
 * TGCrypto - Cryptographic functions for Telegram settings
 * Based on Telegram Desktop (https://github.com/telegramdesktop/tdesktop)
 * 
 * Compatible with VS2010 (uses OpenSSL for crypto)
 */

#ifndef TG_CRYPTO_H
#define TG_CRYPTO_H

#include <stdint.h>

// Required OpenSSL headers
// For VS2010, use OpenSSL 1.0.2 compiled with VS2010
#ifdef _WIN32
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

class TGCrypto {
public:
    // SHA1 hash
    static void sha1(const void* data, size_t len, uint8_t* result);
    
    // SHA256 hash
    static void sha256(const void* data, size_t len, uint8_t* result);
    
    // SHA512 hash
    static void sha512(const void* data, size_t len, uint8_t* result);
    
    // MD5 hash (stateful)
    static void md5Init();
    static void md5Update(const void* data, size_t len);
    static void md5Final(uint8_t* result);
    
    // PBKDF2-HMAC-SHA1 key derivation
    static bool pbkdf2_hmac_sha1(
        const uint8_t* password, size_t passwordLen,
        const uint8_t* salt, size_t saltLen,
        int iterations,
        uint8_t* key, size_t keyLen);
    
    // PBKDF2-HMAC-SHA512 key derivation (modern method)
    static bool pbkdf2_hmac_sha512(
        const uint8_t* password, size_t passwordLen,
        const uint8_t* salt, size_t saltLen,
        int iterations,
        uint8_t* key, size_t keyLen);
    
    // AES-256-IGE encryption/decryption (Telegram's method)
    // key: 32 bytes (256 bits)
    // iv: 32 bytes (aesKey = first 16 bytes of sha1 hash)
    static bool aesEncryptLocal(const void* src, void* dst, size_t len,
                                 const uint8_t* key, const void* iv);
    static bool aesDecryptLocal(const void* src, void* dst, size_t len,
                                 const uint8_t* key, const void* iv);
    
    // AES-256-IGE mode
    static bool aesIgeEncrypt(const void* src, void* dst, size_t len,
                               const uint8_t* key, uint8_t* iv);
    static bool aesIgeDecrypt(const void* src, void* dst, size_t len,
                               const uint8_t* key, uint8_t* iv);
    
    // Random fill
    static void randomFill(void* data, size_t len);
};

#endif // TG_CRYPTO_H
