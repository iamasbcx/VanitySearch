/*
 * Detailed TRON address generation step-by-step verification test
 */

#include "SECP256k1.h"
#include "Int.h"
#include "Point.h"
#include "hash/keccak256.h"
#include "hash/sha256.h"
#include "Base58.h"
#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

void printHex(const char* label, const unsigned char* data, int len) {
	printf("%s: ", label);
	for (int i = 0; i < len; i++) {
		printf("%02X", data[i]);
	}
	printf("\n");
}

void testTronAddressStepByStep(Secp256K1& secp) {
	printf("=== TRON Address Generation Step-by-Step Test ===\n\n");
	
	// Test with private key = 1
	Int privKey;
	privKey.SetInt32(1);
	
	printf("Step 1: Generate public key from private key\n");
	printf("  Private Key: 0x%s\n", privKey.GetBase16().c_str());
	
	Point pubKey = secp.ComputePublicKey(&privKey);
	printf("  Public Key X: %s\n", pubKey.x.GetBase16().c_str());
	printf("  Public Key Y: %s\n", pubKey.y.GetBase16().c_str());
	
	printf("\nStep 2: Extract uncompressed public key (64 bytes, no prefix)\n");
	unsigned char publicKeyBytes[64];
	pubKey.x.Get32Bytes(publicKeyBytes);
	pubKey.y.Get32Bytes(publicKeyBytes + 32);
	printHex("  Public Key (64 bytes)", publicKeyBytes, 64);
	
	printf("\nStep 3: Compute Keccak256 hash\n");
	unsigned char keccakHash[32];
	keccak256(publicKeyBytes, 64, keccakHash);
	printHex("  Keccak256 Hash", keccakHash, 32);
	
	printf("\nStep 4: Take last 20 bytes of Keccak256 hash\n");
	unsigned char hash160[20];
	memcpy(hash160, keccakHash + 12, 20);
	printHex("  Last 20 bytes", hash160, 20);
	
	printf("\nStep 5: Add TRON prefix 0x41\n");
	unsigned char addressBytes[25];
	addressBytes[0] = 0x41;
	memcpy(addressBytes + 1, hash160, 20);
	printHex("  With prefix (21 bytes)", addressBytes, 21);
	
	printf("\nStep 6: Compute double SHA256 checksum\n");
	unsigned char checksum[4];
	sha256_checksum(addressBytes, 21, checksum);
	printHex("  Checksum (4 bytes)", checksum, 4);
	
	printf("\nStep 7: Append checksum\n");
	memcpy(addressBytes + 21, checksum, 4);
	printHex("  Final bytes (25 bytes)", addressBytes, 25);
	
	printf("\nStep 8: Base58 encode\n");
	string tronAddr = EncodeBase58(addressBytes, addressBytes + 25);
	printf("  TRON Address: %s\n", tronAddr.c_str());
	printf("  Expected: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W\n");
	printf("  Status: %s\n", tronAddr == "TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W" ? "PASS" : "FAIL");
	
	// Verify using the library function
	printf("\nStep 9: Verify with library function\n");
	string libAddr = secp.GetAddress(TRON, false, pubKey);
	printf("  Library Result: %s\n", libAddr.c_str());
	printf("  Match: %s\n\n", libAddr == tronAddr ? "YES" : "NO");
}

int main() {
	// Initialize secp256k1 (using stack allocation)
	Secp256K1 secp;
	secp.Init();
	
	testTronAddressStepByStep(secp);
	
	printf("\n=== Additional Test Cases ===\n\n");
	
	// Test case: Known TRON address from documentation
	{
		Int privKey;
		privKey.SetInt32(1);
		Point pubKey = secp.ComputePublicKey(&privKey);
		string tronAddr = secp.GetAddress(TRON, false, pubKey);
		
		printf("Known Test Vector:\n");
		printf("  Private Key: 0x1\n");
		printf("  TRON Address: %s\n", tronAddr.c_str());
		printf("  Expected: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W\n");
		printf("  Result: %s\n\n", tronAddr == "TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W" ? "PASS" : "FAIL");
	}
	
	printf("=== All Tests Complete ===\n");
	return 0;
}
