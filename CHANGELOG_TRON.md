# Changelog - TRON Address Support

## [Unreleased] - TRON Address Generation

### Added
- **TRON address generation support** based on calc_addrs.cl algorithm
- Keccak256 hash implementation (`hash/keccak256.h`, `hash/keccak256.cpp`)
- TRON address type constant (`TRON = 3`)
- GPU Keccak256 placeholder (`GPU/GPUKeccak256.h`)
- Comprehensive documentation:
  - `TRON_README.md` - Full feature documentation
  - `TRON_EXAMPLES.md` - Usage examples and comparisons
- TRON address recognition in Vanity class (addresses starting with 'T')
- Command-line help text for TRON usage

### Modified
- `SECP256k1.h` - Added TRON address type definition
- `SECP256K1.cpp` - Implemented TRON address generation:
  - `GetHash160()` - Added TRON case using Keccak256
  - `GetAddress()` - Added TRON address formatting (0x41 prefix, double SHA256 checksum)
- `Vanity.cpp` - Updated `initAddress()` to recognize TRON addresses
- `main.cpp` - Updated usage/help text
- `Makefile` - Added keccak256.cpp to build
- `README.md` - Added TRON feature information

### Technical Details

#### TRON Address Algorithm Implementation
```
1. Generate secp256k1 public key (uncompressed, 64 bytes)
   - X coordinate: 32 bytes
   - Y coordinate: 32 bytes
   
2. Keccak256 hash of public key
   - Input: 64 bytes (no 0x04 prefix)
   - Output: 32 bytes
   
3. Extract address hash
   - Take last 20 bytes of Keccak256 output
   
4. Add TRON prefix
   - Prepend 0x41 byte → 21 bytes total
   
5. Calculate checksum
   - SHA256(21 bytes) → intermediate hash
   - SHA256(intermediate) → final hash
   - Take first 4 bytes
   
6. Create final address
   - Append 4-byte checksum to 21-byte address → 25 bytes
   
7. Base58 encode
   - Result: 34-character TRON address starting with 'T'
```

#### Key Differences from Bitcoin
| Aspect | Bitcoin | TRON |
|--------|---------|------|
| Hash | SHA256+RIPEMD160 | Keccak256 |
| Public Key | Compressed/Uncompressed | Uncompressed only |
| Prefix | 0x00/0x05 | 0x41 |
| First Char | 1, 3, or bc1 | T |

### Performance Notes
- **CPU Implementation**: Fully functional, single-threaded
- **GPU Implementation**: Placeholder created, optimization pending
- **Keccak256 Performance**: Standard implementation, AVX2/SSE optimizations possible

### Compatibility
- ✅ Compatible with existing Bitcoin address generation
- ✅ Can search for mixed address types in separate runs
- ✅ Same command-line interface
- ⚠️ GPU acceleration not yet optimized for TRON

### Testing Status
- ✅ Code compiles successfully
- ✅ Core algorithms implemented
- ⚠️ Full integration testing pending (requires CUDA environment)
- ⚠️ Performance benchmarks pending

### Known Issues
1. GPU Keccak256 is placeholder implementation only
2. No SSE/AVX optimizations for CPU Keccak256 yet
3. Limited testing without CUDA environment

### Future Enhancements
- Complete CUDA Keccak256 implementation
- Add SSE/AVX optimizations for Keccak256
- Implement batch processing optimizations
- Add comprehensive test suite
- Performance benchmarking
- Support for TRON contract addresses

### Migration Notes
- No breaking changes to existing functionality
- TRON support is additive
- Existing Bitcoin address searches work as before
- New address type recognized automatically by first character

### Usage Examples

#### Basic Usage
```bash
# Search for TRON vanity address
./vanitysearch TTest

# With GPU (once optimized)
./vanitysearch -gpu -gpuId 0 TMyAddress

# Multiple targets
./vanitysearch -i tron_targets.txt -o found.txt
```

#### Input File Format
```
# tron_targets.txt
TTest
TMoon
TSun12
```

### References
- Base implementation: calc_addrs.cl OpenCL reference
- Algorithm source: TRON Protocol specification
- Keccak256: Ethereum-compatible implementation
- Base58 encoding: Bitcoin-compatible implementation

### Contributors
- Initial implementation: Based on calc_addrs.cl algorithm
- Integration: VanitySearch framework

---

## Version Information
- VanitySearch Base: 1.17 Linux with BitCrack integration
- TRON Support: Initial implementation (CPU)
- Date: 2025-11

