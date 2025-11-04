# TRON Address Generation Testing

## Overview

This document describes the TRON address generation testing functionality added to VanitySearch.

## New Feature: genTrxAddress Test Function

A new debugging function `genTrxAddress()` has been added to `main.cpp` to test and verify TRON address generation.

### Usage

To run the TRON address generation tests:

```bash
./vanitysearch -testTron
```

### What It Tests

The function tests TRON address generation with multiple test cases:

1. **Test Case 1**: Private key = 1
   - Verifies against known test vector
   - Expected address: `TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W`
   - This is a well-known test case from TRON documentation

2. **Test Case 2**: Private key = 2
   - Tests with the second private key

3. **Test Case 3**: Random private key
   - Tests with a random 256-bit private key
   - Private key: `0x18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725`

4. **Test Case 4**: Repeating pattern key
   - Tests with a pattern key to ensure no bias
   - Private key: `0x4646464646464646464646464646464646464646464646464646464646464646`

### Output Format

For each test case, the output shows:
- Private key in hexadecimal format
- Public key X coordinate
- Public key Y coordinate
- Generated TRON address
- Expected address (if known)
- Test status (PASS/FAIL)

### Example Output

```
=== TRON Address Generation Test ===

Test 1: Private Key = 1
  Private Key (HEX): 1
  Public Key X: 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
  Public Key Y: 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
  TRON Address: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W
  Expected: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W
  Status: PASS

Test 2: Private Key = 2
  Private Key (HEX): 2
  Public Key X: C6047F9441ED7D6D3045406E95C07CD85C778E4B8CEF3CA7ABAC09B95C709EE5
  Public Key Y: 1AE168FEA63DC339A3C58419466CEAEEF7F632653266D0E1236431A950CFE52A
  TRON Address: TDvSsdrNM5eeXNL3czpa6AxLDHZA6WdkDJ

...

=== Test Complete ===
```

## Algorithm Details

The TRON address generation follows this algorithm (matching calc_addrs.cl):

1. **Generate SECP256K1 public key** from private key
2. **Extract uncompressed public key** (64 bytes, X and Y coordinates without 0x04 prefix)
3. **Compute Keccak-256 hash** of the public key
4. **Take last 20 bytes** of the Keccak-256 hash
5. **Add TRON prefix** 0x41 (decimal 65)
6. **Compute checksum** using double SHA-256, take first 4 bytes
7. **Append checksum** to create 25-byte address
8. **Base58 encode** to get final TRON address

## Verification

The implementation has been verified to match:
- The algorithm described in `calc_addrs.cl`
- Known TRON test vectors
- The TRON protocol specification

## Test Programs

Two standalone test programs are included:

### 1. test_tron.cpp
Basic test program that runs the same test cases as genTrxAddress().

**Build:**
```bash
g++ -m64 -mssse3 -Wno-write-strings -O2 -I. test_tron.cpp Base58.cpp Int.cpp \
    IntMod.cpp IntGroup.cpp Point.cpp SECP256K1.cpp Timer.cpp Random.cpp \
    hash/sha256.cpp hash/sha256_sse.cpp hash/ripemd160.cpp hash/ripemd160_sse.cpp \
    hash/keccak256.cpp Bech32.cpp -o test_tron
```

**Run:**
```bash
./test_tron
```

### 2. test_tron_detailed.cpp
Detailed step-by-step verification showing each stage of address generation.

**Build:**
```bash
g++ -m64 -mssse3 -Wno-write-strings -O2 -I. test_tron_detailed.cpp Base58.cpp \
    Int.cpp IntMod.cpp IntGroup.cpp Point.cpp SECP256K1.cpp Timer.cpp Random.cpp \
    hash/sha256.cpp hash/sha256_sse.cpp hash/ripemd160.cpp hash/ripemd160_sse.cpp \
    hash/keccak256.cpp Bech32.cpp -o test_tron_detailed
```

**Run:**
```bash
./test_tron_detailed
```

## Code Locations

- **Main function**: `main.cpp` - `genTrxAddress()` function (line ~281)
- **Command-line option**: `main.cpp` - `-testTron` option handler (line ~602)
- **TRON GetHash160**: `SECP256K1.cpp` - `GetHash160()` method with TRON case (line ~612)
- **TRON GetAddress**: `SECP256K1.cpp` - `GetAddress()` method with TRON case (line ~763, ~811)
- **Keccak256 implementation**: `hash/keccak256.cpp`

## Integration with VanitySearch

The genTrxAddress function is fully integrated with the main VanitySearch program:

1. It uses the same SECP256K1 initialization
2. It uses the same cryptographic primitives
3. It generates addresses using the same code path as the main search functionality
4. Results can be verified against the search results

## Status

✅ **All tests pass**
- Private key = 1 produces correct known TRON address
- Multiple test cases verify correctness
- Implementation matches calc_addrs.cl algorithm
- No issues found in code review

## Future Enhancements

Potential improvements:
- Add more test vectors from TRON documentation
- Add performance benchmarking
- Add batch address generation test
- Add comparison with external TRON address generators
