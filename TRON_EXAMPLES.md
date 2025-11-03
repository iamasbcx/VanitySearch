# TRON Address Generation Example

This document demonstrates how TRON address generation works in VanitySearch.

## Example 1: Simple TRON Address Generation

### Input: Private Key
```
Private Key (hex): 0000000000000000000000000000000000000000000000000000000000000001
```

### Step-by-Step Process:

1. **Generate Public Key (Uncompressed)**
   ```
   X: 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
   Y: 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
   ```

2. **Concatenate X and Y (64 bytes, no 0x04 prefix)**
   ```
   79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
   483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
   ```

3. **Keccak256 Hash (32 bytes)**
   ```
   [Keccak256 output - 32 bytes]
   Take last 20 bytes for address hash
   ```

4. **Add TRON Prefix (0x41)**
   ```
   41[20-byte address hash]
   Total: 21 bytes
   ```

5. **Calculate Checksum**
   ```
   SHA256(21 bytes) → hash1
   SHA256(hash1) → hash2
   Take first 4 bytes of hash2
   ```

6. **Append Checksum**
   ```
   21 bytes + 4 bytes = 25 bytes total
   ```

7. **Base58 Encode**
   ```
   Final TRON Address: T... (34 characters)
   ```

## Example 2: TRON vs Bitcoin Address

For the same private key:
```
Private Key: 0x0000000000000000000000000000000000000000000000000000000000000001
```

**Bitcoin Address (P2PKH):**
- Uses SHA256 + RIPEMD160
- Prefix: 0x00 (mainnet)
- Result: 1BgGZ9tcN4rm9KBzDn7KprQz87SZ26SAMH

**TRON Address:**
- Uses Keccak256
- Prefix: 0x41
- Result: T... (depends on Keccak256 output)

## Key Differences

| Feature | Bitcoin | TRON |
|---------|---------|------|
| Hash Algorithm | SHA256 + RIPEMD160 | Keccak256 |
| Public Key Format | Compressed/Uncompressed | Uncompressed only |
| Address Prefix | 0x00 (P2PKH) or 0x05 (P2SH) | 0x41 |
| Starts With | 1 or 3 | T |
| Length | Variable (25-34) | Fixed (34) |
| Checksum | SHA256 double hash | SHA256 double hash |
| Encoding | Base58 | Base58 |

## Code Usage

```cpp
#include "SECP256k1.h"

// Initialize
Secp256K1 secp;
secp.Init();

// Set private key
Int privKey;
privKey.SetBase16("0000000000000000000000000000000000000000000000000000000000000001");

// Generate public key
Point pubKey = secp.ComputePublicKey(&privKey);

// Generate TRON address
std::string tronAddr = secp.GetAddress(TRON, false, pubKey);
// Note: compressed parameter is ignored for TRON (always uses uncompressed)

// Generate Bitcoin address for comparison
std::string btcAddr = secp.GetAddress(P2PKH, true, pubKey);
```

## Vanity Address Search Examples

### Search for address starting with "TTest"
```bash
./vanitysearch TTest
```

### Search for address starting with "TMoon" or "TSun"
```bash
./vanitysearch -i addresses.txt

# addresses.txt:
# TMoon
# TSun
```

### Search with GPU acceleration
```bash
./vanitysearch -gpu -gpuId 0 TLucky
```

## Address Validation

Valid TRON addresses must:
1. Start with the letter 'T'
2. Be exactly 34 characters long
3. Use Base58 alphabet (no 0, O, I, l)
4. Have valid checksum

Example valid addresses:
- `TJRabPrwbZy45sbavfcjxwKTPmjC7Zqeew`
- `TAzRWqNvbvr8YdR4DtzVXxG7CqP45sP8rP`
- `TTest...` (to be found by VanitySearch)

## References

1. TRON Protocol Documentation
2. Keccak256 (Ethereum-style) specification
3. VanitySearch implementation in `SECP256K1.cpp`
4. OpenCL reference in `calc_addrs.cl`
