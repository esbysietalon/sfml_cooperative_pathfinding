#pragma once
#include "stdafx.h"
#include "playable.h"
#include "globals.h"
#include "dstarlite.h"
#include <random>
#include <deque>

class Playable;
enum move_t;

enum order_type_t{ MOVE=1, SAY, FOLLOW };


struct order_t {
	order_type_t type;
	Playable* target;
	int x, y;
};

class Brain {
public:
	Brain();
	~Brain();
	Brain(Playable* playable);
	//order_t* nextMove();
	move_t nextStep();
	void think();
	void say(Playable* target);
	void queueThought(struct order_t* thought);
	void interruptThought(struct order_t* thought);
	void registerEntity(Playable* entity);
	void emptyRegistry();

	void meander(int* x, int* y);

	int getSocial();
	
	void see();
	void setRMap(Playable*** rMap);
	
private:
	int memoryMap[LMAP_W * LMAP_H];
	Playable* sightMap[4 * (BASE_SIGHT_RANGE+1) * (BASE_SIGHT_RANGE+1)];
	Playable* lemoryMap[LMAP_W * LMAP_H];
	Playable*** rmap;

	int lemX = LMAP_W / 2;
	int lemY = LMAP_H / 2;

	void printLemory();
	
	Playable* _host;
	
	std::deque<move_t>* pathFind(int x, int y);
	std::deque<move_t>* _currPath;
	int pathIndex;
	
	std::deque<order_t*>* _thoughtQueue;
	
	int social;
	double randCoeff;

	std::vector<Playable*> _sensedEntities;

	

};