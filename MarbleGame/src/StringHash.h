#ifndef _STRING_HASH_H_
#define _STRING_HASH_H_

#include <stdint.h>

typedef uint64_t Hash;

// Calculates the hash for a null-terminated string.
inline Hash StringHash(const char* c) {
	//FNV-1a
#define FNV_prime 1099511628211
#define offset_basis 14695981039346656037

	Hash value = offset_basis;

	while (*c) {
		value = value ^ (*c);
		value = value * FNV_prime;
		c++;
	}

	return value;

#undef FNV_prime
#undef offset_basis
}

#endif // _STRING_HASH_H_