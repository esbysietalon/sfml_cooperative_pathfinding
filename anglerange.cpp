#include "stdafx.h"
#include "anglerange.h"

AngleRange::AngleRange()
{
}

AngleRange::~AngleRange()
{
}

int AngleRange::inRange(double ang)
{
	if(ang < 0){
		ang += 2 * PI;
	}
	for (int i = 0; i < ranges.size(); i++) {
		floatrange currRange = ranges.at(i);
		if (ang >= currRange.sAng && ang <= currRange.eAng) {
			return i;
		}
	}
	return -1;
}

double AngleRange::inRange(floatrange angrang)
{
	if (angrang.eAng < angrang.sAng && angrang.eAng < 0 && angrang.sAng >= 0) {
		angrang.eAng += 2 * PI;
		//fprintf(stderr, "adjusting a2 [%f,%f]\n", a1, a2);
	}
	else if (angrang.sAng < 0 && angrang.eAng <= 0) {
		angrang.sAng += 2 * PI;
		angrang.eAng += 2 * PI;
		//fprintf(stderr, "adjusting a1 and a2 [%f,%f]\n", a1, a2);
	}

	int range = inRange(angrang.sAng);
	if (range != -1) {
		return (double)std::min(1.0, (ranges.at(range).eAng - angrang.sAng) / (angrang.eAng - angrang.sAng));
	}
	range = inRange(angrang.eAng);
	if (range != -1) {
		return (double)std::min(1.0, (angrang.eAng - ranges.at(range).sAng) / (angrang.eAng - angrang.sAng));
	}
	return 0.0;
}

void AngleRange::addRange(double a1, double a2)
{
	//fprintf(stderr, "adding range(%f,%f)\n", a1, a2);
	if(a2 < a1 && a2 < 0 && a1 >= 0){
		a2 += 2 * PI;
		//fprintf(stderr, "adjusting a2 [%f,%f]\n", a1, a2);
	}else if(a1 < 0 && a2 <= 0){
		a1 += 2 * PI;
		a2 += 2 * PI;
		//fprintf(stderr, "adjusting a1 and a2 [%f,%f]\n", a1, a2);
	}
	ranges.push_back(floatrange(a1, a2));
	consolidate();
}

void AngleRange::consolidate()
{
	for (int i = 0; i < ranges.size(); i++) {
		floatrange first = ranges.at(i);
		double cSAng = INFINITY;
		double cEAng = INFINITY;
		for (int j = 0; j < ranges.size(); j++) {
			if (i == j)
				continue;
			floatrange second = ranges.at(j);
			
			if (first.sAng >= second.sAng && first.sAng <= second.eAng) {
				cSAng = second.sAng;
			}
			if (first.eAng >= second.sAng && first.eAng <= second.eAng) {
				cEAng = second.eAng;
			}
			if (cSAng != INFINITY || cEAng != INFINITY) {
				if (cSAng == INFINITY)
					cSAng = first.sAng;
				if (cEAng == INFINITY)
					cEAng = first.eAng;
				ranges.erase(ranges.begin() + i);
				if (i < j) {
					ranges.erase(ranges.begin() + j - 1);
				}else{
					ranges.erase(ranges.begin() + j);
				}
				ranges.push_back(floatrange(cSAng, cEAng));
				i--;
				break;
			}
		}
		
	}
}
