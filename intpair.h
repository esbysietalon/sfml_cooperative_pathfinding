#pragma once
#include <vector>
#include "globals.h"
struct intpair {
	intpair(int i = 0, int j = 0) {
		x = i;
		y = j;
	}

	intpair & operator= (const intpair& rhs) {
		if (this != &rhs) {
			this->x = rhs.x;
			this->y = rhs.y;
		}
		return *this;
	}
	bool operator== (const intpair& rhs) const {
		return (this->x == rhs.x && this->y == rhs.y);
	}
	bool operator!= (const intpair& rhs) const {
		return !(*this == rhs);
	}
	int x, y;
};

struct fov_data {
	fov_data(intpair pt = intpair(0, 0)) {
		point = pt;
	}
	intpair point;
	float oAng, eAng; //origin angle edge angle 
};

struct floatrange {
	floatrange(double a = 0, double b = 0) {
		sAng = a;
		eAng = b;
		if (sAng < 0) {
			sAng += 2 * PI;
		}
		if (eAng < 0) {
			eAng += 2 * PI;
		}
	}
	double sAng, eAng;
};
