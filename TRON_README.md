# TRON Address Generation Support

## Overview

This implementation adds TRON (TRC20) address generation support to VanitySearch. TRON addresses use a different hashing algorithm (Keccak256) compared to Bitcoin addresses (SHA256+RIPEMD160).

## TRON Address Generation Algorithm

1. Generate secp256k1 public key from private key (uncompressed, 64 bytes)
2. Hash the public key with Keccak256 → 32 bytes
3. Take the last 20 bytes of the Keccak256 hash
4. Prepend TRON address prefix byte `0x41` → 21 bytes total
5. Calculate checksum: Double SHA256 of the 21 bytes
6. Take first 4 bytes of the checksum
7. Append checksum to address: 21 + 4 = 25 bytes
8. Encode in Base58 → Final TRON address (starts with 'T')

## Implementation Status

### ✅ Completed (CPU)
- Keccak256 hash implementation (C++)
- TRON address type definition
- TRON address generation in SECP256K1 class
- Address recognition for TRON (addresses starting with 'T')
- Integration with existing vanity search infrastructure
- Build system updates

### 🔄 In Progress (GPU)
- Keccak256 CUDA implementation (placeholder created)
- GPU kernel integration for TRON addresses
- Performance optimization for GPU mining

## Usage

### Searching for TRON Vanity Addresses

```bash
# Search for a TRON address starting with "TTest"
./vanitysearch TTest

# Search for addresses from a file
./vanitysearch -i tron_addresses.txt -o found.txt

# Use specific GPU
./vanitysearch -gpu -gpuId 0 TMyVanity
```

### TRON Address Format

Valid TRON addresses:
- Start with the letter 'T'
- Are 34 characters long when Base58 encoded
- Example: `TJRabPrwbZy45sbavfcjxwKTPmjC7Zqeew`

## Files Modified/Added

### New Files
- `hash/keccak256.h` - Keccak256 hash header
- `hash/keccak256.cpp` - Keccak256 implementation
- `GPU/GPUKeccak256.h` - GPU Keccak256 (placeholder)

### Modified Files
- `SECP256k1.h` - Added TRON address type definition
- `SECP256K1.cpp` - Implemented TRON GetHash160 and GetAddress
- `Vanity.cpp` - Added TRON address recognition
- `main.cpp` - Updated usage/help text
- `Makefile` - Added keccak256.cpp to build

## Technical Details

### Address Type Constants
```cpp
#define P2PKH  0   // Bitcoin P2PKH
#define P2SH   1   // Bitcoin P2SH  
#define BECH32 2   // Bitcoin Bech32
#define TRON   3   // TRON addresses
```

### Keccak256 vs SHA3-256
Note: This implementation uses Keccak256 (Ethereum-style), which is different from the final SHA3-256 standard. TRON uses the same Keccak256 as Ethereum.

## Building

### CPU Only (for testing)
```bash
make clean
make gpu=0
```

### With GPU Support
```bash
make clean  
make all
```

## Testing

Test TRON address generation:
```bash
# Generate a TRON address from a known private key
./vanitysearch -check
```

## Known Limitations

1. **GPU Support**: Full CUDA implementation for Keccak256 is in progress
2. **Performance**: CPU-only implementation is functional but slower than GPU
3. **Testing**: Comprehensive test suite for TRON addresses needed

## Future Enhancements

- [ ] Complete CUDA Keccak256 optimization
- [ ] Add SSE/AVX optimizations for CPU Keccak256
- [ ] Benchmark TRON vs Bitcoin address generation performance
- [ ] Add support for TRON vanity contract addresses
- [ ] Implement batch address generation
- [ ] Add comprehensive test suite

## Reference

The TRON address generation algorithm is based on:
- calc_addrs.cl (OpenCL reference implementation)
- TRON Protocol documentation
- Ethereum address generation (similar Keccak256 usage)

## Contributing

When contributing TRON-related code:
1. Ensure compatibility with existing Bitcoin address generation
2. Add appropriate test cases
3. Document any algorithm-specific optimizations
4. Follow the existing code style

## License

Same as VanitySearch - GNU General Public License v3.0
