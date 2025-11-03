# TRON Address Generation Usage

This modified version of VanitySearch supports TRON address generation based on the Keccak-256 algorithm from the newtron project.

## Building

```bash
make all
```

Note: Requires CUDA toolkit for GPU support. CPU-only builds will compile successfully but cannot link the final executable without GPU libraries.

## Usage

### Search for TRON addresses

```bash
./vanitysearch -tron Txxx
```

This will search for TRON addresses starting with "Txxx".

### Example

```bash
# Search for a TRON address starting with "TABC"
./vanitysearch -tron TABC

# Search with GPU ID selection
./vanitysearch -tron -gpuId 0 TABC

# Search from input file
./vanitysearch -tron -i addresses.txt

# Stop when address is found
./vanitysearch -tron -stop TABC
```

## TRON Address Format

- TRON addresses always start with 'T'
- Generated using Keccak-256 hash of the public key
- Uses Base58 encoding with 0x41 prefix
- Example: `TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W`

## Implementation Details

### Algorithm Flow:
1. Generate SECP256K1 key pair
2. Extract 64-byte public key (X and Y coordinates)
3. Compute Keccak-256 hash of public key
4. Take last 20 bytes of hash
5. Add 0x41 prefix
6. Compute double SHA256 checksum
7. Base58 encode to get final address

### Files Modified:
- `hash/keccak256.cpp/h` - CPU Keccak-256 implementation
- `GPU/GPUKeccak256.h` - GPU Keccak-256 implementation
- `SECP256K1.cpp` - TRON address generation
- `GPU/GPUCompute.h` - GPU kernel integration
- `main.cpp` - Command-line interface
- `Vanity.cpp` - Address validation

## Testing

Successfully tested with private key:
```
Private Key: 0x0000000000000000000000000000000000000000000000000000000000000001
TRON Address: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W
```

## Credits

Based on:
- Original VanitySearch by JeanLucPons
- TRON algorithm from iamasbcx/newtron project
- calc_addrs.cl Keccak-256 implementation
