#pragma once

#include <functional>
#include <random>

namespace ulid {

// ULID is a 16 byte Universally Unique Lexicographically Sortable Identifier
struct ULID {
	uint8_t data[16];

	ULID() {
		for (int i = 0 ; i < 16 ; i++) {
			data[i] = 0;
		}
	}

	ULID(const ULID& other) {
		for (int i = 0 ; i < 16 ; i++) {
			data[i] = other.data[i];
		}
	}

	ULID& operator=(const ULID& other) {
		for (int i = 0 ; i < 16 ; i++) {
			data[i] = other.data[i];
		}
		return *this;
	}

	ULID(ULID&& other) {
		for (int i = 0 ; i < 16 ; i++) {
			data[i] = other.data[i];
			other.data[i] = 0;
		}
	}

	ULID& operator=(ULID&& other) {
		for (int i = 0 ; i < 16 ; i++) {
			data[i] = other.data[i];
			other.data[i] = 0;
		}
		return *this;
	}
};

// EncodeTime will encode the first 6 bytes of a uint8_t array to the passed
// timestamp
void EncodeTime(time_t timestamp, ULID& ulid) {
	ulid.data[0] = static_cast<uint8_t>(timestamp >> 40);
	ulid.data[1] = static_cast<uint8_t>(timestamp >> 32);
	ulid.data[2] = static_cast<uint8_t>(timestamp >> 24);
	ulid.data[3] = static_cast<uint8_t>(timestamp >> 16);
	ulid.data[4] = static_cast<uint8_t>(timestamp >> 8);
	ulid.data[5] = static_cast<uint8_t>(timestamp);
}

// EncodeEntropy will encode the last 10 bytes of the passed uint8_t array with
// the values generated using the passed random number generator.
void EncodeEntropy(const std::function<uint8_t()>& rng, ULID& ulid) {
	ulid.data[6] = rng();
	ulid.data[7] = rng();
	ulid.data[8] = rng();
	ulid.data[9] = rng();
	ulid.data[10] = rng();
	ulid.data[11] = rng();
	ulid.data[12] = rng();
	ulid.data[13] = rng();
	ulid.data[14] = rng();
	ulid.data[15] = rng();
}

// Encode will create an encoded ULID with a timestamp and a generator.
void Encode(time_t timestamp, const std::function<uint8_t()>& rng, ULID& ulid) {
	EncodeTime(timestamp, ulid);
	EncodeEntropy(rng, ulid);
}

// Crockford's Base32
const char Encoding[33] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

// MarshalTo will marshal a ULID to the passed character array.
//
// Implementation taken directly from oklog/ulid
// (https://sourcegraph.com/github.com/oklog/ulid@0774f81f6e44af5ce5e91c8d7d76cf710e889ebb/-/blob/ulid.go#L162-190)
//
// timestamp:
// dst[0]: first 3 bits of data[0]
// dst[1]: last 5 bits of data[0]
// dst[2]: first 5 bits of data[1]
// dst[3]: last 3 bits of data[1] + first 2 bits of data[2]
// dst[4]: bits 3-7 of data[2]
// dst[5]: last bit of data[2] + first 4 bits of data[3]
// dst[6]: last 4 bits of data[3] + first bit of data[4]
// dst[7]: bits 2-6 of data[4]
// dst[8]: last 2 bits of data[4] + first 3 bits of data[5]
// dst[9]: last 5 bits of data[5]
//
// entropy:
// follows similarly, except now all components are set to 5 bits.
void MarshalTo(const ULID& ulid, char dst[26]) {
	// 10 byte timestamp
	dst[0] = Encoding[(ulid.data[0] & 224) >> 5];
	dst[1] = Encoding[ulid.data[0] & 31];
	dst[2] = Encoding[(ulid.data[1] & 248) >> 3];
	dst[3] = Encoding[((ulid.data[1] & 7) << 2) | ((ulid.data[2] & 192) >> 6)];
	dst[4] = Encoding[(ulid.data[2] & 62) >> 1];
	dst[5] = Encoding[((ulid.data[2] & 1) << 4) | ((ulid.data[3] & 240) >> 4)];
	dst[6] = Encoding[((ulid.data[3] & 15) << 1) | ((ulid.data[4] & 128) >> 7)];
	dst[7] = Encoding[(ulid.data[4] & 124) >> 2];
	dst[8] = Encoding[((ulid.data[4] & 3) << 3) | ((ulid.data[5] & 224) >> 5)];
	dst[9] = Encoding[ulid.data[5] & 31];

	// 16 bytes of entropy
	dst[10] = Encoding[(ulid.data[6] & 248) >> 3];
	dst[11] = Encoding[((ulid.data[6] & 7) << 2) | ((ulid.data[7] & 192) >> 6)];
	dst[12] = Encoding[(ulid.data[7] & 62) >> 1];
	dst[13] = Encoding[((ulid.data[7] & 1) << 4) | ((ulid.data[8] & 240) >> 4)];
	dst[14] = Encoding[((ulid.data[8] & 15) << 1) | ((ulid.data[9] & 128) >> 7)];
	dst[15] = Encoding[(ulid.data[9] & 124) >> 2];
	dst[16] = Encoding[((ulid.data[9] & 3) << 3) | ((ulid.data[10] & 224) >> 5)];
	dst[17] = Encoding[ulid.data[10] & 31];
	dst[18] = Encoding[(ulid.data[11] & 248) >> 3];
	dst[19] = Encoding[((ulid.data[11] & 7) << 2) | ((ulid.data[12] & 192) >> 6)];
	dst[20] = Encoding[(ulid.data[12] & 62) >> 1];
	dst[21] = Encoding[((ulid.data[12] & 1) << 4) | ((ulid.data[13] & 240) >> 4)];
	dst[22] = Encoding[((ulid.data[13] & 15) << 1) | ((ulid.data[14] & 128) >> 7)];
	dst[23] = Encoding[(ulid.data[14] & 124) >> 2];
	dst[24] = Encoding[((ulid.data[14] & 3) << 3) | ((ulid.data[15] & 224) >> 5)];
	dst[25] = Encoding[ulid.data[15] & 31];
}

// Marshal will marshal a ULID to a std::string.
std::string Marshal(const ULID& ulid) {
	char data[27];
	data[26] = '\0';
	MarshalTo(ulid, data);
	return std::string(data);
}

};  // namespace ulid
