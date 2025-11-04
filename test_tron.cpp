/*
 * Simple TRON address generation test
 */

#include "SECP256k1.h"
#include "Int.h"
#include "Point.h"
#include <stdio.h>
#include <string>

using namespace std;

int main() {
	printf("=== TRON Address Generation Test ===\n\n");
	
	// Initialize secp256k1 (using stack allocation)
	Secp256K1 secp;
	secp.Init();
	
	// Test case 1: Private key = 1
	{
		Int privKey;
		privKey.SetInt32(1);
		Point pubKey = secp.ComputePublicKey(&privKey);
		string tronAddr = secp.GetAddress(TRON, false, pubKey);
		string privKeyHex = privKey.GetBase16();
		
		printf("Test 1: Private Key = 1\n");
		printf("  Private Key (HEX): %s\n", privKeyHex.c_str());
		printf("  Public Key X: %s\n", pubKey.x.GetBase16().c_str());
		printf("  Public Key Y: %s\n", pubKey.y.GetBase16().c_str());
		printf("  TRON Address: %s\n", tronAddr.c_str());
		printf("  Expected: TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W\n");
		printf("  Status: %s\n\n", tronAddr == "TMVQGm1qAQYVdetCeGRRkTWYYrLXt4u51W" ? "PASS" : "FAIL");
	}
	
	// Test case 2: Small private key
	{
		Int privKey;
		privKey.SetBase16("0000000000000000000000000000000000000000000000000000000000000002");
		Point pubKey = secp.ComputePublicKey(&privKey);
		string tronAddr = secp.GetAddress(TRON, false, pubKey);
		string privKeyHex = privKey.GetBase16();
		
		printf("Test 2: Private Key = 2\n");
		printf("  Private Key (HEX): %s\n", privKeyHex.c_str());
		printf("  Public Key X: %s\n", pubKey.x.GetBase16().c_str());
		printf("  Public Key Y: %s\n", pubKey.y.GetBase16().c_str());
		printf("  TRON Address: %s\n\n", tronAddr.c_str());
	}
	
	// Test case 3: Random private key
	{
		Int privKey;
		privKey.SetBase16("18E14A7B6A307F426A94F8114701E7C8E774E7F9A47E2C2035DB29A206321725");
		Point pubKey = secp.ComputePublicKey(&privKey);
		string tronAddr = secp.GetAddress(TRON, false, pubKey);
		string privKeyHex = privKey.GetBase16();
		
		printf("Test 3: Random Private Key\n");
		printf("  Private Key (HEX): %s\n", privKeyHex.c_str());
		printf("  Public Key X: %s\n", pubKey.x.GetBase16().c_str());
		printf("  Public Key Y: %s\n", pubKey.y.GetBase16().c_str());
		printf("  TRON Address: %s\n\n", tronAddr.c_str());
	}
	
	// Test case 4: Another known key
	{
		Int privKey;
		privKey.SetBase16("4646464646464646464646464646464646464646464646464646464646464646");
		Point pubKey = secp.ComputePublicKey(&privKey);
		string tronAddr = secp.GetAddress(TRON, false, pubKey);
		string privKeyHex = privKey.GetBase16();
		
		printf("Test 4: Repeating Pattern Key\n");
		printf("  Private Key (HEX): %s\n", privKeyHex.c_str());
		printf("  Public Key X: %s\n", pubKey.x.GetBase16().c_str());
		printf("  Public Key Y: %s\n", pubKey.y.GetBase16().c_str());
		printf("  TRON Address: %s\n\n", tronAddr.c_str());
	}
	
	printf("=== Test Complete ===\n");
	
	return 0;
}
