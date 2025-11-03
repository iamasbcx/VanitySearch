typedef union {
	uchar b[200];
	ulong q[25];
	uint d[50];
} ethhash;

#define TH_ELT(t, c0, c1, c2, c3, c4, d0, d1, d2, d3, d4) \
{ \
    t = rotate((ulong)(d0 ^ d1 ^ d2 ^ d3 ^ d4), (ulong)1) ^ (c0 ^ c1 ^ c2 ^ c3 ^ c4); \
}

#define THETA(s00, s01, s02, s03, s04, \
              s10, s11, s12, s13, s14, \
              s20, s21, s22, s23, s24, \
              s30, s31, s32, s33, s34, \
              s40, s41, s42, s43, s44) \
{ \
    TH_ELT(t0, s40, s41, s42, s43, s44, s10, s11, s12, s13, s14); \
    TH_ELT(t1, s00, s01, s02, s03, s04, s20, s21, s22, s23, s24); \
    TH_ELT(t2, s10, s11, s12, s13, s14, s30, s31, s32, s33, s34); \
    TH_ELT(t3, s20, s21, s22, s23, s24, s40, s41, s42, s43, s44); \
    TH_ELT(t4, s30, s31, s32, s33, s34, s00, s01, s02, s03, s04); \
    s00 ^= t0; s01 ^= t0; s02 ^= t0; s03 ^= t0; s04 ^= t0; \
    s10 ^= t1; s11 ^= t1; s12 ^= t1; s13 ^= t1; s14 ^= t1; \
    s20 ^= t2; s21 ^= t2; s22 ^= t2; s23 ^= t2; s24 ^= t2; \
    s30 ^= t3; s31 ^= t3; s32 ^= t3; s33 ^= t3; s34 ^= t3; \
    s40 ^= t4; s41 ^= t4; s42 ^= t4; s43 ^= t4; s44 ^= t4; \
}

#define RHOPI(s00, s01, s02, s03, s04, \
              s10, s11, s12, s13, s14, \
              s20, s21, s22, s23, s24, \
              s30, s31, s32, s33, s34, \
              s40, s41, s42, s43, s44) \
{ \
	t0  = rotate(s10, (ulong) 1);  \
	s10 = rotate(s11, (ulong)44); \
	s11 = rotate(s41, (ulong)20); \
	s41 = rotate(s24, (ulong)61); \
	s24 = rotate(s42, (ulong)39); \
	s42 = rotate(s04, (ulong)18); \
	s04 = rotate(s20, (ulong)62); \
	s20 = rotate(s22, (ulong)43); \
	s22 = rotate(s32, (ulong)25); \
	s32 = rotate(s43, (ulong) 8); \
	s43 = rotate(s34, (ulong)56); \
	s34 = rotate(s03, (ulong)41); \
	s03 = rotate(s40, (ulong)27); \
	s40 = rotate(s44, (ulong)14); \
	s44 = rotate(s14, (ulong) 2); \
	s14 = rotate(s31, (ulong)55); \
	s31 = rotate(s13, (ulong)45); \
	s13 = rotate(s01, (ulong)36); \
	s01 = rotate(s30, (ulong)28); \
	s30 = rotate(s33, (ulong)21); \
	s33 = rotate(s23, (ulong)15); \
	s23 = rotate(s12, (ulong)10); \
	s12 = rotate(s21, (ulong) 6); \
	s21 = rotate(s02, (ulong) 3); \
	s02 = t0; \
}

#define KHI(s00, s01, s02, s03, s04, \
            s10, s11, s12, s13, s14, \
            s20, s21, s22, s23, s24, \
            s30, s31, s32, s33, s34, \
            s40, s41, s42, s43, s44) \
{ \
    t0 = s00 ^ (~s10 &  s20); \
    t1 = s10 ^ (~s20 &  s30); \
    t2 = s20 ^ (~s30 &  s40); \
    t3 = s30 ^ (~s40 &  s00); \
    t4 = s40 ^ (~s00 &  s10); \
    s00 = t0; s10 = t1; s20 = t2; s30 = t3; s40 = t4; \
    \
    t0 = s01 ^ (~s11 &  s21); \
    t1 = s11 ^ (~s21 &  s31); \
    t2 = s21 ^ (~s31 &  s41); \
    t3 = s31 ^ (~s41 &  s01); \
    t4 = s41 ^ (~s01 &  s11); \
    s01 = t0; s11 = t1; s21 = t2; s31 = t3; s41 = t4; \
    \
    t0 = s02 ^ (~s12 &  s22); \
    t1 = s12 ^ (~s22 &  s32); \
    t2 = s22 ^ (~s32 &  s42); \
    t3 = s32 ^ (~s42 &  s02); \
    t4 = s42 ^ (~s02 &  s12); \
    s02 = t0; s12 = t1; s22 = t2; s32 = t3; s42 = t4; \
    \
    t0 = s03 ^ (~s13 &  s23); \
    t1 = s13 ^ (~s23 &  s33); \
    t2 = s23 ^ (~s33 &  s43); \
    t3 = s33 ^ (~s43 &  s03); \
    t4 = s43 ^ (~s03 &  s13); \
    s03 = t0; s13 = t1; s23 = t2; s33 = t3; s43 = t4; \
    \
    t0 = s04 ^ (~s14 &  s24); \
    t1 = s14 ^ (~s24 &  s34); \
    t2 = s24 ^ (~s34 &  s44); \
    t3 = s34 ^ (~s44 &  s04); \
    t4 = s44 ^ (~s04 &  s14); \
    s04 = t0; s14 = t1; s24 = t2; s34 = t3; s44 = t4; \
}

#define IOTA(s00, r) { s00 ^= r; }

__constant ulong keccakf_rndc[24] = {
	0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
	0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
	0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
	0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
	0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
	0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
	0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
	0x8000000000008080, 0x0000000080000001, 0x8000000080008008
};

void sha3_keccakf(ethhash * const h)
{
ulong * const st = h->q;
	h->d[33] ^= 0x80000000;
	ulong t0, t1, t2, t3, t4;
	for (int i = 0; i < 24; ++i) {
		THETA(st[0], st[5], st[10], st[15], st[20], st[1], st[6], st[11], st[16], st[21], st[2], st[7], st[12], st[17], st[22], st[3], st[8], st[13], st[18], st[23], st[4], st[9], st[14], st[19], st[24]);
		RHOPI(st[0], st[5], st[10], st[15], st[20], st[1], st[6], st[11], st[16], st[21], st[2], st[7], st[12], st[17], st[22], st[3], st[8], st[13], st[18], st[23], st[4], st[9], st[14], st[19], st[24]);
		KHI(st[0], st[5], st[10], st[15], st[20], st[1], st[6], st[11], st[16], st[21], st[2], st[7], st[12], st[17], st[22], st[3], st[8], st[13], st[18], st[23], st[4], st[9], st[14], st[19], st[24]);
		IOTA(st[0], keccakf_rndc[i]);
	}
}


// Base SHA-256 context
#define H0 0x6a09e667
#define H1 0xbb67ae85
#define H2 0x3c6ef372
#define H3 0xa54ff53a
#define H4 0x510e527f
#define H5 0x9b05688c
#define H6 0x1f83d9ab
#define H7 0x5be0cd19

// String convert macro
#define STR(s) #s
#define XSTR(s) STR(s)

// Methods
//  << : bitshift left
//  >> : bitshift right
//  ^  : bitwise XOR
//  ~  : bitwise NOT
//  &  : bitwise AND
//  |  : bitwise OR

inline uint rotr(uint x, int n) // Rotate right
{
  return (x >> n) | (x << (32 - n));
}
inline uint ch(uint x, uint y, uint z) // Choice based on x
{
  return (x & y) ^ (~x & z);
}
inline uint maj(uint x, uint y, uint z) // Majority of bits in x, y
{
  return (x & y) ^ (x & z) ^ (y & z);
}
inline uint sig0(uint x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
inline uint sig1(uint x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }
inline uint csig0(uint x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
inline uint csig1(uint x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }

void sha256_to_uchar(const uint len, const uchar *key, uchar *output) {
  // Initialize
  int qua;                     // Message schedule step quantity
  int mod;                     // Message schedule step modulus
  uint A, B, C, D, E, F, G, H; // Compression targets
  uint T1, T2;                 // Compression temp
  uint W[80];                  // Message schedule
  const uint K[64] =           // Cube roots of first 64 primes
      {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
       0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
       0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
       0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
       0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
       0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
       0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
       0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
       0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
       0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
       0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

// Reset algorithm
#pragma unroll
  for (int i = 0; i < 80; i++) {
    W[i] = 0x00000000;
  }

  // Create message block
  qua = len / 4;

  mod = len % 4;
  for (int i = 0; i < qua; i++) {
    W[i] = (key[i * 4 + 0]) << 24;
    W[i] |= (key[i * 4 + 1]) << 16;
    W[i] |= (key[i * 4 + 2]) << 8;
    W[i] |= (key[i * 4 + 3]);
  }

  // Pad remaining uint
  if (mod == 0) {
    W[qua] = 0x80000000;
  } else if (mod == 1) {
    W[qua] = (key[qua * 4]) << 24;
    W[qua] |= 0x800000;
  } else if (mod == 2) {
    W[qua] = (key[qua * 4]) << 24;
    W[qua] |= (key[qua * 4 + 1]) << 16;
    W[qua] |= 0x8000;
  } else {
    W[qua] = (key[qua * 4]) << 24;
    W[qua] |= (key[qua * 4 + 1]) << 16;
    W[qua] |= (key[qua * 4 + 2]) << 8;
    W[qua] |= 0x80;
  }

  W[15] = len * 8; // Add key length

// Run message schedule
#pragma unroll
  for (int i = 16; i < 64; i++) {
    W[i] = sig1(W[i - 2]) + W[i - 7] + sig0(W[i - 15]) + W[i - 16];
  }

  // Prepare compression
  A = H0;
  B = H1;
  C = H2;
  D = H3;
  E = H4;
  F = H5;
  G = H6;
  H = H7;

// Compress
#pragma unroll
  for (int i = 0; i < 64; i++) {
    // Compress temporary
    T1 = H + csig1(E) + ch(E, F, G) + K[i] + W[i];
    T2 = csig0(A) + maj(A, B, C);

    // Rotate over, override H
    H = G;
    G = F;
    F = E;
    E = D + T1;
    D = C;
    C = B;
    B = A;
    A = T1 + T2;
  }

  W[0] = A + H0;
  W[1] = B + H1;
  W[2] = C + H2;
  W[3] = D + H3;
  W[4] = E + H4;
  W[5] = F + H5;
  W[6] = G + H6;
  W[7] = H + H7;
  for (int i = 0; i < 8; i++) {
    output[i * 4 + 0] = (W[i] >> 24) & 0xFF;
    output[i * 4 + 1] = (W[i] >> 16) & 0xFF;
    output[i * 4 + 2] = (W[i] >> 8) & 0xFF;
    output[i * 4 + 3] = W[i] & 0xFF;
  }
}
void split_tron_to_20(__private  uchar *tron,__private  uchar *output) {
  uint j = 0;
  for (uint i = 0; i < 25; i++) {
    if (i <= 10 || i > 15) {
      output[j] = tron[i];
      j++;
    }
  }
}
void ethhash_to_tronsplithash(__private const uchar *ethhashs,__private  uchar *tornhash) {
  uchar hash0[21];
  uchar hash1[32];
  uchar hash2[32];
  uchar hash3[25];
  for (uint i = 0; i < 20; i++) {
    hash0[i + 1] = ethhashs[i];
  }
  hash0[0] = 65; // add 41
  sha256_to_uchar(sizeof(hash0), hash0, hash1);
  sha256_to_uchar(sizeof(hash1), hash1, hash2);
  for (uint i = 0; i < 21; i++) {
    hash3[i] = hash0[i];
  }
  hash3[21] = hash2[0];
  hash3[22] = hash2[1];
  hash3[23] = hash2[2];
  hash3[24] = hash2[3];
  split_tron_to_20(hash3, tornhash);
}

void ethhash_to_tronhash(__global const uchar *ethhash2,__private  uchar *tornhash) {
  uchar hash0[21];
  uchar hash1[32];
  uchar hash2[32];
  for (uint i = 0; i < 20; i++) {
    hash0[i + 1] = ethhash2[i];
  }
  hash0[0] = 65; // add 41
  sha256_to_uchar(sizeof(hash0), hash0, hash1);
  sha256_to_uchar(sizeof(hash1), hash1, hash2);
  for (uint i = 0; i < 21; i++) {
    tornhash[i] = hash0[i];
  }
  tornhash[21] = hash2[0];
  tornhash[22] = hash2[1];
  tornhash[23] = hash2[2];
  tornhash[24] = hash2[3];
}
void ucharArrayToHexStr(const uchar *input, size_t length, char *output) {
  const char HEX_CHARS[] = "0123456789abcdef";

  for (size_t i = 0; i < length; i++) {
    output[i * 2] = HEX_CHARS[(input[i] >> 4) & 0x0F];
    output[i * 2 + 1] = HEX_CHARS[input[i] & 0x0F];
  }
  output[length * 2] = '\0';
  printf("hash : %s\n", output);
}
__constant  char alphabet[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void base58_encode(const uchar *input, char *output, int input_len) {

  __private uint digits[32] = {0};
  int digit_count = 1;
  for (int i = 0; i < input_len; i++) {
    uint carry = input[i];
    for (int j = 0; j < digit_count; j++) {
      carry += digits[j] << 8;
      digits[j] = carry % 58;
      carry /= 58;
    }
    while (carry) {
      digits[digit_count++] = carry % 58;
      carry /= 58;
    }
  }

  int zero_count = 0;
  while (zero_count < input_len && input[zero_count] == 0) {
    zero_count++;
  }
  int output_idx = 0;
  output[output_idx++] = alphabet[digits[digit_count - 1]];
  for (int i = digit_count - 2; i >= 0; i--) {
    if (zero_count > 0) {
      zero_count--;
    } else {
      output[output_idx++] = alphabet[digits[i]];
    }
  }
  output[output_idx] = '\0';
}

kernel void sha256single_kernel(uint len, global uchar *key,
                                global uchar *result) {

  // for (uint i = 0; i < len; i++) {
  //   printf("%u \n", key[i]);
  // // }
  // ethhash_to_tronhash(key, result);
  // char output[34];
  // base58encode(result, output, 25);
  // char torn_hash_split[20];
  // uint j = 0;
	// for (uint i = 0; i < 34; i++){
	// 	if(i<10 || i>=24){
	// 		torn_hash_split[j] = output[i];
	// 		j++;
	// 	}
	// }
  // uint* const torn_hash_uint = torn_hash_split;

  // char* const b = torn_hash_uint;

  // for (int i = 0; i < 20; i++) {
  //   printf("%u \n", b[i]);
  // }

  // const uint* pv = result;
  // for (uint j = 0; j < 7; j++) {
  //   printf(" %u \n", pv[j]);
  // }
  // char str1[50];
  // ucharArrayToHexStr(key, 25, result);
  // // reverseUcharArray(result, 20);
  // uint *a = result;
  // printf("%u %u %u %u %u\n", a[0], a[1], a[2], a[3], a[4]);
  // printf("%s\n",str1);
  // for (uint i = 0; i < 6; i++) {
  //   printf("%u \n", a[i]);
  // }
}


#define MP_WORDS 8
#define MP_BITS 32
#define bswap32(n) (rotate(n & 0x00FF00FF, 24U)|(rotate(n, 8U) & 0x00FF00FF))
typedef uint mp_word;
typedef struct {
	mp_word d[MP_WORDS];
} mp_number;
__constant const mp_number mod = { {0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff} };
__constant const mp_number tripleNegativeGx = { {0xbb17b196, 0xf2287bec, 0x76958573, 0xf82c096e, 0x946adeea, 0xff1ed83e, 0x1269ccfa, 0x92c4cc83 } };
__constant const mp_number doubleNegativeGy = { {0x09de52bf, 0xc7705edf, 0xb2f557cc, 0x05d0976e, 0xe3ddeeae, 0x44b60807, 0xb2b87735, 0x6f8a4b11} };
__constant const mp_number negativeGy = { {0x04ef2777, 0x63b82f6f, 0x597aabe6, 0x02e84bb7, 0xf1eef757, 0xa25b0403, 0xd95c3b9a, 0xb7c52588 } };
mp_word mp_sub(mp_number* const r, const mp_number* const a, const mp_number* const b) {
	mp_word t, c = 0;
	for (mp_word i = 0; i < MP_WORDS; ++i) {
		t = a->d[i] - b->d[i] - c;
		c = t > a->d[i] ? 1 : (t == a->d[i] ? c : 0);

		r->d[i] = t;
	}
	return c;
}
mp_word mp_sub_mod(mp_number* const r) {
	mp_number mod = { {0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff} };
	mp_word t, c = 0;
	for (mp_word i = 0; i < MP_WORDS; ++i) {
		t = r->d[i] - mod.d[i] - c;
		c = t > r->d[i] ? 1 : (t == r->d[i] ? c : 0);

		r->d[i] = t;
	}
	return c;
}
void mp_mod_sub(mp_number* const r, const mp_number* const a, const mp_number* const b) {
	mp_word i, t, c = 0;
	for (i = 0; i < MP_WORDS; ++i) {
		t = a->d[i] - b->d[i] - c;
		c = t < a->d[i] ? 0 : (t == a->d[i] ? c : 1);

		r->d[i] = t;
	}
	if (c) {
		c = 0;
		for (i = 0; i < MP_WORDS; ++i) {
			r->d[i] += mod.d[i] + c;
			c = r->d[i] < mod.d[i] ? 1 : (r->d[i] == mod.d[i] ? c : 0);
		}
	}
}

void mp_mod_sub_const(mp_number* const r, __constant const mp_number* const a, const mp_number* const b) {
	mp_word i, t, c = 0;

	for (i = 0; i < MP_WORDS; ++i) {
		t = a->d[i] - b->d[i] - c;
		c = t < a->d[i] ? 0 : (t == a->d[i] ? c : 1);

		r->d[i] = t;
	}

	if (c) {
		c = 0;
		for (i = 0; i < MP_WORDS; ++i) {
			r->d[i] += mod.d[i] + c;
			c = r->d[i] < mod.d[i] ? 1 : (r->d[i] == mod.d[i] ? c : 0);
		}
	}
}

void mp_mod_sub_gx(mp_number* const r, const mp_number* const a) {
	mp_word i, t, c = 0;

	t = a->d[0] - 0x16f81798; c = t < a->d[0] ? 0 : (t == a->d[0] ? c : 1); r->d[0] = t;
	t = a->d[1] - 0x59f2815b - c; c = t < a->d[1] ? 0 : (t == a->d[1] ? c : 1); r->d[1] = t;
	t = a->d[2] - 0x2dce28d9 - c; c = t < a->d[2] ? 0 : (t == a->d[2] ? c : 1); r->d[2] = t;
	t = a->d[3] - 0x029bfcdb - c; c = t < a->d[3] ? 0 : (t == a->d[3] ? c : 1); r->d[3] = t;
	t = a->d[4] - 0xce870b07 - c; c = t < a->d[4] ? 0 : (t == a->d[4] ? c : 1); r->d[4] = t;
	t = a->d[5] - 0x55a06295 - c; c = t < a->d[5] ? 0 : (t == a->d[5] ? c : 1); r->d[5] = t;
	t = a->d[6] - 0xf9dcbbac - c; c = t < a->d[6] ? 0 : (t == a->d[6] ? c : 1); r->d[6] = t;
	t = a->d[7] - 0x79be667e - c; c = t < a->d[7] ? 0 : (t == a->d[7] ? c : 1); r->d[7] = t;

	if (c) {
		c = 0;
		for (i = 0; i < MP_WORDS; ++i) {
			r->d[i] += mod.d[i] + c;
			c = r->d[i] < mod.d[i] ? 1 : (r->d[i] == mod.d[i] ? c : 0);
		}
	}
}

void mp_mod_sub_gy(mp_number* const r, const mp_number* const a) {
	mp_word i, t, c = 0;
	t = a->d[0] - 0xfb10d4b8; c = t < a->d[0] ? 0 : (t == a->d[0] ? c : 1); r->d[0] = t;
	t = a->d[1] - 0x9c47d08f - c; c = t < a->d[1] ? 0 : (t == a->d[1] ? c : 1); r->d[1] = t;
	t = a->d[2] - 0xa6855419 - c; c = t < a->d[2] ? 0 : (t == a->d[2] ? c : 1); r->d[2] = t;
	t = a->d[3] - 0xfd17b448 - c; c = t < a->d[3] ? 0 : (t == a->d[3] ? c : 1); r->d[3] = t;
	t = a->d[4] - 0x0e1108a8 - c; c = t < a->d[4] ? 0 : (t == a->d[4] ? c : 1); r->d[4] = t;
	t = a->d[5] - 0x5da4fbfc - c; c = t < a->d[5] ? 0 : (t == a->d[5] ? c : 1); r->d[5] = t;
	t = a->d[6] - 0x26a3c465 - c; c = t < a->d[6] ? 0 : (t == a->d[6] ? c : 1); r->d[6] = t;
	t = a->d[7] - 0x483ada77 - c; c = t < a->d[7] ? 0 : (t == a->d[7] ? c : 1); r->d[7] = t;
	if (c) {
		c = 0;
		for (i = 0; i < MP_WORDS; ++i) {
			r->d[i] += mod.d[i] + c;
			c = r->d[i] < mod.d[i] ? 1 : (r->d[i] == mod.d[i] ? c : 0);
		}
	}
}

mp_word mp_add(mp_number* const r, const mp_number* const a) {
	mp_word c = 0;
	for (mp_word i = 0; i < MP_WORDS; ++i) {
		r->d[i] += a->d[i] + c;
		c = r->d[i] < a->d[i] ? 1 : (r->d[i] == a->d[i] ? c : 0);
	}
	return c;
}

mp_word mp_add_mod(mp_number* const r) {
	mp_word c = 0;
	for (mp_word i = 0; i < MP_WORDS; ++i) {
		r->d[i] += mod.d[i] + c;
		c = r->d[i] < mod.d[i] ? 1 : (r->d[i] == mod.d[i] ? c : 0);
	}
	return c;
}

mp_word mp_add_more(mp_number* const r, mp_word* const extraR, const mp_number* const a, const mp_word* const extraA) {
	const mp_word c = mp_add(r, a);
	*extraR += *extraA + c;
	return *extraR < *extraA ? 1 : (*extraR == *extraA ? c : 0);
}

mp_word mp_gte(const mp_number* const a, const mp_number* const b) {
	mp_word l = 0, g = 0;

	for (mp_word i = 0; i < MP_WORDS; ++i) {
		if (a->d[i] < b->d[i]) l |= (1 << i);
		if (a->d[i] > b->d[i]) g |= (1 << i);
	}

	return g >= l;
}

void mp_shr_extra(mp_number* const r, mp_word* const e) {
	r->d[0] = (r->d[1] << 31) | (r->d[0] >> 1);
	r->d[1] = (r->d[2] << 31) | (r->d[1] >> 1);
	r->d[2] = (r->d[3] << 31) | (r->d[2] >> 1);
	r->d[3] = (r->d[4] << 31) | (r->d[3] >> 1);
	r->d[4] = (r->d[5] << 31) | (r->d[4] >> 1);
	r->d[5] = (r->d[6] << 31) | (r->d[5] >> 1);
	r->d[6] = (r->d[7] << 31) | (r->d[6] >> 1);
	r->d[7] = (*e << 31) | (r->d[7] >> 1);
	*e >>= 1;
}

void mp_shr(mp_number* const r) {
	r->d[0] = (r->d[1] << 31) | (r->d[0] >> 1);
	r->d[1] = (r->d[2] << 31) | (r->d[1] >> 1);
	r->d[2] = (r->d[3] << 31) | (r->d[2] >> 1);
	r->d[3] = (r->d[4] << 31) | (r->d[3] >> 1);
	r->d[4] = (r->d[5] << 31) | (r->d[4] >> 1);
	r->d[5] = (r->d[6] << 31) | (r->d[5] >> 1);
	r->d[6] = (r->d[7] << 31) | (r->d[6] >> 1);
	r->d[7] >>= 1;
}

mp_word mp_mul_word_add_extra(mp_number* const r, const mp_number* const a, const mp_word w, mp_word* const extra) {
	mp_word cM = 0;
	mp_word cA = 0;
	mp_word tM = 0;

	for (mp_word i = 0; i < MP_WORDS; ++i) {
		tM = (a->d[i] * w + cM);
		cM = mul_hi(a->d[i], w) + (tM < cM);

		r->d[i] += tM + cA;
		cA = r->d[i] < tM ? 1 : (r->d[i] == tM ? cA : 0);
	}

	*extra += cM + cA;
	return *extra < cM ? 1 : (*extra == cM ? cA : 0);
}

void mp_mul_mod_word_sub(mp_number* const r, const mp_word w, const bool withModHigher) {
	mp_number mod = { { 0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff} };
	mp_number modhigher = { {0x00000000, 0xfffffc2f, 0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff} };

	mp_word cM = 0;
	mp_word cS = 0;
	mp_word tS = 0;
	mp_word tM = 0;
	mp_word cA = 0;

	for (mp_word i = 0; i < MP_WORDS; ++i) {
		tM = (mod.d[i] * w + cM);
		cM = mul_hi(mod.d[i], w) + (tM < cM);

		tM += (withModHigher ? modhigher.d[i] : 0) + cA;
		cA = tM < (withModHigher ? modhigher.d[i] : 0) ? 1 : (tM == (withModHigher ? modhigher.d[i] : 0) ? cA : 0);

		tS = r->d[i] - tM - cS;
		cS = tS > r->d[i] ? 1 : (tS == r->d[i] ? cS : 0);

		r->d[i] = tS;
	}
}

void mp_mod_mul(mp_number* const r, const mp_number* const X, const mp_number* const Y) {
	mp_number Z = { {0} };
	mp_word extraWord;

	for (int i = MP_WORDS - 1; i >= 0; --i) {
		extraWord = Z.d[7]; Z.d[7] = Z.d[6]; Z.d[6] = Z.d[5]; Z.d[5] = Z.d[4]; Z.d[4] = Z.d[3]; Z.d[3] = Z.d[2]; Z.d[2] = Z.d[1]; Z.d[1] = Z.d[0]; Z.d[0] = 0;
		bool overflow = mp_mul_word_add_extra(&Z, X, Y->d[i], &extraWord);
		mp_mul_mod_word_sub(&Z, extraWord, overflow);
	}

	*r = Z;
}

void mp_mod_inverse(mp_number* const r) {
	mp_number A = { { 1 } };
	mp_number C = { { 0 } };
	mp_number v = mod;

	mp_word extraA = 0;
	mp_word extraC = 0;

	while (r->d[0] || r->d[1] || r->d[2] || r->d[3] || r->d[4] || r->d[5] || r->d[6] || r->d[7]) {
		while (!(r->d[0] & 1)) {
			mp_shr(r);
			if (A.d[0] & 1) {
				extraA += mp_add_mod(&A);
			}

			mp_shr_extra(&A, &extraA);
		}

		while (!(v.d[0] & 1)) {
			mp_shr(&v);
			if (C.d[0] & 1) {
				extraC += mp_add_mod(&C);
			}

			mp_shr_extra(&C, &extraC);
		}

		if (mp_gte(r, &v)) {
			mp_sub(r, r, &v);
			mp_add_more(&A, &extraA, &C, &extraC);
		}
		else {
			mp_sub(&v, &v, r);
			mp_add_more(&C, &extraC, &A, &extraA);
		}
	}

	while (extraC) {
		extraC -= mp_sub_mod(&C);
	}

	v = mod;
	mp_sub(r, &v, &C);
}

typedef struct {
	mp_number x;
	mp_number y;
} point;

void point_add(point* const r, point* const p, point* const o) {
	mp_number tmp;
	mp_number newX;
	mp_number newY;

	mp_mod_sub(&tmp, &o->x, &p->x);

	mp_mod_inverse(&tmp);

	mp_mod_sub(&newX, &o->y, &p->y);
	mp_mod_mul(&tmp, &tmp, &newX);

	mp_mod_mul(&newX, &tmp, &tmp);
	mp_mod_sub(&newX, &newX, &p->x);
	mp_mod_sub(&newX, &newX, &o->x);

	mp_mod_sub(&newY, &p->x, &newX);
	mp_mod_mul(&newY, &newY, &tmp);
	mp_mod_sub(&newY, &newY, &p->y);

	r->x = newX;
	r->y = newY;
}

typedef struct {
	uint found;
	uint foundId;
	uchar foundHash[20];
} result;

void profanity_init_seed(__global const point* const precomp, point* const p, bool* const pIsFirst, const size_t precompOffset, const ulong seed) {
	point o;

	for (uchar i = 0; i < 8; ++i) {
		const uchar shift = i * 8;
		const uchar byte = (seed >> shift) & 0xFF;

		if (byte) {
			o = precomp[precompOffset + i * 255 + byte - 1];
			if (*pIsFirst) {
				*p = o;
				*pIsFirst = false;
			}
			else {
				point_add(p, p, &o);
			}
		}
	}
}


// ===== device-side splitmix64（OpenCL/C / 可移植） =====
static inline ulong splitmix64_step(ulong* state) {
	ulong z = (*state += 0x9E3779B97F4A7C15UL);
	z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9UL;
	z = (z ^ (z >> 27)) * 0x94D049BB133111EBUL;
	return z ^ (z >> 31);
}

// 从 base_seed(64-bit) 和 tid 生成 32 字节到 out32
static inline void derive_seed_bytes_for_thread(ulong base_seed, unsigned int tid, unsigned char* out32) {
	// 初始化 state：混合 base_seed 与线程 id
	ulong state = base_seed ^ ((ulong)tid * 0x9E3779B97F4A7C15UL);

	// 生成 4 个 splitmix64 输出 -> 共 32 字节
	for (int i = 0; i < 4; ++i) {
		ulong v = splitmix64_step(&state);
		// 拆成 8 字节（小端顺序）
		for (int b = 0; b < 8; ++b) {
			unsigned char by = (unsigned char)((v >> (8 * b)) & 0xFFu);
			// 映射 0 -> 1，避免原实现中跳过 byte==0 导致熵损失
			out32[i * 8 + b] = (by == 0) ? 1 : by;
		}
	}
}

__kernel void profanity_init(__global const point* const precomp, __global mp_number* const pDeltaX, __global mp_number* const pPrevLambda, __global result* const pResult, const ulong4 seed) {

	const size_t id = get_global_id(0);
	point p;
	bool bIsFirst = true;

	// 我们使用 seed.s0 ^ seed.s1 ^ seed.s2 ^ (seed.s3 + id) 作为 base seed 以增加熵
	ulong host_combined = (ulong)seed.s0;
	host_combined ^= ((ulong)seed.s1 << 1);
	host_combined ^= ((ulong)seed.s2 << 7);
	host_combined ^= ((ulong)seed.s3 << 17);
	host_combined ^= (ulong)id; // 确保与线程 id 强关联

	unsigned char derived[32];
	derive_seed_bytes_for_thread(host_combined, (unsigned int)id, derived);

	// 用 derived 的 4 个 8-byte block 当作四个 seed 参数来替代原来直接使用的 seed.x/y/z/(w+id)
	ulong s0 = 0, s1 = 0, s2 = 0, s3 = 0;
	for (int i = 0; i < 8; ++i) s0 |= ((ulong)derived[i]) << (8 * i);
	for (int i = 0; i < 8; ++i) s1 |= ((ulong)derived[8 + i]) << (8 * i);
	for (int i = 0; i < 8; ++i) s2 |= ((ulong)derived[16 + i]) << (8 * i);
	for (int i = 0; i < 8; ++i) s3 |= ((ulong)derived[24 + i]) << (8 * i);

	// 下面使用修改后的 profanity_init_seed_by_bytes（或复用原函数但传入 s0..s3）
	// 推荐直接调用现有 profanity_init_seed，但把它重载为接受 ulong
	profanity_init_seed(precomp, &p, &bIsFirst, 8 * 255 * 0, (ulong)s0);
	profanity_init_seed(precomp, &p, &bIsFirst, 8 * 255 * 1, (ulong)s1);
	profanity_init_seed(precomp, &p, &bIsFirst, 8 * 255 * 2, (ulong)s2);
	profanity_init_seed(precomp, &p, &bIsFirst, 8 * 255 * 3, (ulong)s3);

	// ... 剩余逻辑保持不变
	mp_number tmp1, tmp2;
	point tmp3;

	mp_mod_sub_gx(&tmp1, &p.x);
	mp_mod_inverse(&tmp1);
	mp_mod_sub(&tmp2, &p.y, &p.y);
	mp_mod_mul(&tmp1, &tmp1, &tmp2);

	tmp3 = precomp[0];
	point_add(&p, &tmp3, &p);

	mp_mod_sub_gx(&p.x, &p.x);

	pDeltaX[id] = p.x;
	pPrevLambda[id] = tmp1;

	for (uchar i = 0; i < PROFANITY_MAX_SCORE + 1; ++i) {
		pResult[i].found = 0;
	}
}

__kernel void profanity_inverse(__global const mp_number* const pDeltaX, __global mp_number* const pInverse) {
	const size_t id = get_global_id(0) * PROFANITY_INVERSE_SIZE;

	mp_number negativeDoubleGy = { {0x09de52bf, 0xc7705edf, 0xb2f557cc, 0x05d0976e, 0xe3ddeeae, 0x44b60807, 0xb2b87735, 0x6f8a4b11 } };

	mp_number copy1, copy2;
	mp_number buffer[PROFANITY_INVERSE_SIZE];
	mp_number buffer2[PROFANITY_INVERSE_SIZE];

	buffer[0] = pDeltaX[id];
	for (uint i = 1; i < PROFANITY_INVERSE_SIZE; ++i) {
		buffer2[i] = pDeltaX[id + i];
		mp_mod_mul(&buffer[i], &buffer2[i], &buffer[i - 1]);
	}

	copy1 = buffer[PROFANITY_INVERSE_SIZE - 1];
	mp_mod_inverse(&copy1);
	mp_mod_mul(&copy1, &copy1, &negativeDoubleGy);

	for (uint i = PROFANITY_INVERSE_SIZE - 1; i > 0; --i) {
		mp_mod_mul(&copy2, &copy1, &buffer[i - 1]);
		mp_mod_mul(&copy1, &copy1, &buffer2[i]);
		pInverse[id + i] = copy2;
	}

	pInverse[id] = copy1;
}



__kernel void profanity_iterate(
	__global mp_number* const pDeltaX,
	__global mp_number* const pInverse,
	__global mp_number* const pPrevLambda)
{
	const size_t id = get_global_id(0);

	mp_number negativeGx = { {0xe907e497, 0xa60d7ea3, 0xd231d726, 0xfd640324, 0x3178f4f8, 0xaa5f9d6a, 0x06234453, 0x86419981 } };

	ethhash h = { { 0 } };

	mp_number dX = pDeltaX[id];
	mp_number tmp = pInverse[id];
	mp_number lambda = pPrevLambda[id];

	mp_mod_sub(&lambda, &tmp, &lambda);

	mp_mod_mul(&tmp, &lambda, &lambda);

	mp_mod_sub(&dX, &dX, &tmp);
	mp_mod_sub_const(&dX, &tripleNegativeGx, &dX);

	pDeltaX[id] = dX;
	pPrevLambda[id] = lambda;

	mp_mod_mul(&tmp, &lambda, &dX);
	mp_mod_sub_const(&tmp, &negativeGy, &tmp);

	mp_mod_sub(&dX, &dX, &negativeGx);

	h.d[0] = bswap32(dX.d[MP_WORDS - 1]);
	h.d[1] = bswap32(dX.d[MP_WORDS - 2]);
	h.d[2] = bswap32(dX.d[MP_WORDS - 3]);
	h.d[3] = bswap32(dX.d[MP_WORDS - 4]);
	h.d[4] = bswap32(dX.d[MP_WORDS - 5]);
	h.d[5] = bswap32(dX.d[MP_WORDS - 6]);
	h.d[6] = bswap32(dX.d[MP_WORDS - 7]);
	h.d[7] = bswap32(dX.d[MP_WORDS - 8]);
	h.d[8] = bswap32(tmp.d[MP_WORDS - 1]);
	h.d[9] = bswap32(tmp.d[MP_WORDS - 2]);
	h.d[10] = bswap32(tmp.d[MP_WORDS - 3]);
	h.d[11] = bswap32(tmp.d[MP_WORDS - 4]);
	h.d[12] = bswap32(tmp.d[MP_WORDS - 5]);
	h.d[13] = bswap32(tmp.d[MP_WORDS - 6]);
	h.d[14] = bswap32(tmp.d[MP_WORDS - 7]);
	h.d[15] = bswap32(tmp.d[MP_WORDS - 8]);
	h.d[16] ^= 0x01;

	sha3_keccakf(&h);

	pInverse[id].d[0] = h.d[3];
	pInverse[id].d[1] = h.d[4];
	pInverse[id].d[2] = h.d[5];
	pInverse[id].d[3] = h.d[6];
	pInverse[id].d[4] = h.d[7];
}

void profanity_result_update(
	const size_t id,
	__global const uchar* const hash,
	__global result* const pResult,
	const uchar score,
	const uchar scoreMax)
{
	if (score && score > scoreMax) {
		uchar hasResult = atomic_inc(&pResult[score].found);
		if (hasResult == 0) {
			pResult[score].foundId = id;
			for (int i = 0; i < 20; ++i) {
				pResult[score].foundHash[i] = hash[i];
			}
		}
	}
}

inline uchar has_repeated_chars(const char* str, uint len, const uchar repeatCount)
{
	uint count = 1;
	for (uint i = 1; i < len; ++i)
	{
		if (str[i] == str[i - 1])
		{
			count++;
			if (count >= repeatCount)
				return 1;
		}
		else
			count = 1;
	}
	return 0;
}

inline uchar has_sequential_chars(const char* str, uint len, const uchar repeatCount)
{
	uint count_inc = 1;
	uint count_dec = 1;

	for (uint i = 1; i < len; ++i)
	{
		// 递增序列，如 abcdef / 123456
		if (str[i] == str[i - 1] + 1)
		{
			count_inc++;
			if (count_inc >= repeatCount)
				return 1; // 连续递增
		}
		else
		{
			count_inc = 1;
		}

		// 递减序列，如 fedcba / 654321
		if (str[i] == str[i - 1] - 1)
		{
			count_dec++;
			if (count_dec >= repeatCount)
				return 1; // 连续递减
		}
		else
		{
			count_dec = 1;
		}
	}
	return 0;
}


__kernel void profanity_score_matching(
	__global mp_number* const pInverse,
	__global result* const pResult,
	__constant const uchar* const data1,
	__constant const uchar* const data2,
	const uchar scoreMax,
	const uchar matchingCount,
	const uchar prefixCount,
	const uchar suffixCount,
	const uchar allCount)
{
	const size_t id = get_global_id(0);
	__global const uchar* hash = (__global const uchar*)pInverse[id].d;

	__global uchar* const hash_temp = (__global uchar*)pInverse[id].d;

	uchar tron_hash[25];
	ethhash_to_tronhash(hash_temp, tron_hash);
	char tron_hash_address[34];
	base58_encode(tron_hash, tron_hash_address, 25);

	char matchingHash[20];
	uint j = 0;
	for (uint i = 0; i < 34; i++) {
		if (i < 10 || i >= 24) {
			matchingHash[j] = tron_hash_address[i];
			j++;
		}
	}
	for (uint j = 0; j < matchingCount; j++) {
		uint scorePrefix = 0;
		uint scoreSuffix = 0;
		uint scoreTotal = 0;
		uint dataIndex = 0;

		if (allCount != 0)
		{
			uchar hasRepeat = has_repeated_chars(tron_hash_address, 34, allCount);
			uchar hasRepeat2 = has_sequential_chars(tron_hash_address, 34, allCount);

			if (hasRepeat || hasRepeat2)
			{
				scoreTotal = scoreMax + 1;
				profanity_result_update(id, hash, pResult, scoreTotal, scoreMax);
				break;
			}
		}


		if (prefixCount > 0) {
			for (uint i = 0; i < 10; ++i) {
				dataIndex = j * 20 + i;
				if (data1[dataIndex] > 0 && (matchingHash[i] & data1[dataIndex]) == data2[dataIndex]) {
					++scorePrefix;
				}
				else {
					break;
				}
			}
		}


		if (suffixCount > 0) {
			for (uint i = 19; i > 10; --i) {
				dataIndex = j * 20 + i;
				if (data1[dataIndex] > 0 && (matchingHash[i] & data1[dataIndex]) == data2[dataIndex]) {
					++scoreSuffix;
				}
				else {
					break;
				}
			}
		}


		if (scorePrefix >= prefixCount && scoreSuffix >= suffixCount) {
			scoreTotal = scoreMax + 1;
			profanity_result_update(id, hash, pResult, scoreTotal, scoreMax);
			break;
		}


	}
}