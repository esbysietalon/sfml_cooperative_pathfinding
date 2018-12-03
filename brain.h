#pragma once
#include "stdafx.h"
#include "playable.h"
#include "globals.h"
#include <random>
#include <deque>
#define LMAP_W 65
#define LMAP_H 37


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
private:
	Playable* _host;
	
	std::deque<move_t>* pathFind(int x, int y);
	std::deque<move_t>* _currPath;
	int pathIndex;
	
	std::deque<order_t*> _thoughtQueue;
	
	int social;
	double randCoeff;

	std::vector<Playable*> _sensedEntities;

	int localMap[LMAP_W * LMAP_H];

};