#pragma once
#include <vector>
#include <algorithm>
#include "intpair.h"

class AngleRange {
public:
	AngleRange();
	~AngleRange();
	int inRange(double ang);
	double inRange(floatrange angrang);
	void addRange(double a1, double a2);
	void subRange(double a1, double a2);
private:
	std::vector<floatrange> ranges;
	void consolidate();
};