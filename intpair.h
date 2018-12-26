#pragma once
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
