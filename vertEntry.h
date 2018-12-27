#pragma once
#include "intpair.h"
#include "floatcompare.h"
struct key {
	key(float a = 0, float b = 0) {
		k1 = a;
		k2 = b;
	}
	float k1;
	float k2;
	key& operator= (const key& other) {
		if (this != &other) {
			this->k1 = other.k1;
			this->k2 = other.k2;
		}
		return *this;
	}
	bool operator== (const key& other) const {
		if (floatEqualCustom(this->k1, other.k1, 0.000000001) && floatEqualCustom(this->k2, other.k2, 0.000000001))
			return true;
		return false;
	}
	bool operator!= (const key& other) const {
		return !(*this == other);
	}
	friend bool operator< (const key& lhs, const key& rhs) {
		return (lhs.k1 < rhs.k1 || (floatEqualCustom(lhs.k1, rhs.k1, 0.000000001) && lhs.k2 < rhs.k2));
	}
};

struct vertEntry {
	vertEntry(intpair s, key k) {
		vertex = s;
		key = k;
	}
	intpair vertex;
	key key;
	vertEntry& operator= (const vertEntry& other) {
		if (this != &other) {
			this->vertex = other.vertex;
			this->key = other.key;
		}
		return *this;
	}
	bool operator== (const vertEntry& other) const {
		return ((this->vertex == other.vertex));
	}
	bool operator!= (const vertEntry& other) const {
		return !(*this == other);
	}
	friend bool operator< (const vertEntry& lhs, const vertEntry& rhs) {
		return (lhs != rhs) && (lhs.key.k1 < rhs.key.k1 || (floatEqualCustom(lhs.key.k1, rhs.key.k1, 0.000000001) && lhs.key.k2 < rhs.key.k2));
	}
};

/*struct entry_compare {
	bool operator() (const vertEntry& lhs, const vertEntry& rhs) const {
		return (lhs != rhs) && (lhs.key.k1 < rhs.key.k1 || (floatEqualCustom(lhs.key.k1, rhs.key.k1, 0.00001) && lhs.key.k2 < rhs.key.k2));
	}
};*/