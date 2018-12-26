#include "floatcompare.h"
#include "stdafx.h"
#include <stdlib.h>
#include <cmath>
#include <algorithm>


bool floatEqualCustom(float a, float b, float epsilon) {
	float absA = abs(a);
	float absB = abs(b);
	float diff = abs(a - b);
	if (a == b) {
		return true;
	}
	else if (a == 0 || b == 0 || diff < FLT_MIN) {
		return diff < epsilon * FLT_MIN;
	}
	else {
		return diff / std::min(absA + absB, FLT_MAX) < epsilon;
	}
}