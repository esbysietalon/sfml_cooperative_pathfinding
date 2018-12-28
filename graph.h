#pragma once
#include "intpair.h"
#include "playable.h"
#include "globals.h"

class Playable;

class Graph {
public:
	Graph(Playable*** map, int W, int H, Playable* owner);
	Graph();
	~Graph();
	
	void pred(intpair node, intpair** arr, int* outlen);
	void succ(intpair node, intpair** arr, int* outlen);
	bool isIn(intpair node);
	bool isFree(intpair node);

	int width();
	int height();

	float costAdj(intpair start, intpair end);

	void getMapCopy(Playable*** map, intpair o);
private:
	Playable*** rmap;
	Playable* host;
	int mapW, mapH;
};