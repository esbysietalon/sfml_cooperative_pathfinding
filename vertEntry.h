#pragma once
#include "intpair.h"
#include "floatcompare.h"
struct key {
	key(float a = 0, float b = 0) {
		k1 = a;
		k2 = a;
	}
	float k1;
	float k2;
};

struct vertEntry {
	vertEntry(int i, int j, key k) {
		vertex.x = i;
		vertex.y = j;
		key = k;
	}
	struct intpair vertex;
	key key;
};

struct entry_compare {
	bool operator() (const vertEntry& lhs, const vertEntry& rhs) const {
		return (lhs.key.k1 < rhs.key.k1 || (floatEqualCustom(lhs.key.k1, lhs.key.k2, 0.00001) && lhs.key.k2 < rhs.key.k2));
	}
};