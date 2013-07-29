/* 
 *  Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 *  rights reserved.
 *  
 *  License to copy and use this software is granted provided that it
 *  is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 *  Algorithm" in all material mentioning or referencing this software
 *  or this function.
 *  
 *  License is also granted to make and use derivative works provided
 *  that such works are identified as "derived from the RSA Data
 *  Security, Inc. MD5 Message-Digest Algorithm" in all material
 *  mentioning or referencing the derived work.
 *  
 *  RSA Data Security, Inc. makes no representations concerning either
 *  the merchantability of this software or the suitability of this
 *  software for any particular purpose. It is provided "as is"
 *  without express or implied warranty of any kind.
 *  
 *  These notices must be retained in any copies of any part of this
 *  documentation and/or software.
 */

#pragma once

#include <cstdint>
#include <string>
#include <iostream>

class MD5
{
public:
	MD5();
	MD5(const std::string& text);
	void        update(const unsigned char *buf, size_t length);
	void        update(const char *buf, size_t length);
	MD5&        finalize();
	std::string hexdigest() const;
	friend std::ostream& operator << (std::ostream&, MD5 md5);

private:
	void init();
	enum {blocksize = 64}; // VC6 won't eat a const static int here

	void transform(const uint8_t block[blocksize]);
	static void decode(uint32_t output[], const uint8_t input[], size_t len);
	static void encode(uint8_t output[], const uint32_t input[], size_t len);

	bool     finalized;
	uint8_t  buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
	uint32_t count[2];          // 64bit counter for number of bits (lo, hi)
	uint32_t state[4];          // digest so far
	uint8_t  digest[16];        // the result

	// low level logic operations
	static inline uint32_t F(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t G(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t H(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t I(uint32_t x, uint32_t y, uint32_t z);
	static inline uint32_t rotate_left(uint32_t x, int n);
	static inline void FF(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void GG(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void HH(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
	static inline void II(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac);
};

std::string md5(const std::string str);
