#pragma once
#include <SFML/Graphics.hpp>
#include "stdafx.h"
#include "animatedsprite.h"
#include "brain.h"

#define MAX_ANIMS 64 // 2 ^ NUM_FLAGS
enum animation_t { IDLE, MOVE_LEFT, MOVE_RIGHT, FACE_LEFT, FACE_RIGHT, BUSY, NUM_FLAGS, INVALID = -1 };
enum move_t { NONE, N, S = -1, E = 4, W = -4, NE = 5, NW = -3, SE = 3, SW = -5 };


class Brain;

class Playable {

public:
	

	Playable();
	Playable(sf::Texture* texture, int w, int h, int x, int y);
	~Playable();

	bool isControlled();
	void setControl(bool control);

	void update();
	void update_AI();

	int getX();
	int getY();

	sf::Sprite* getSprite();
	void addAnimation(int sx, int sy, int len, animation_t flagSet[]);
	void addAnimation(int sx, int sy, int len, std::initializer_list<animation_t> flagSet);

	int getFlagState(animation_t flagSet[]);
	int getFlagState(std::initializer_list<animation_t> flagSet);

	void addFlags(std::initializer_list<animation_t> list);
	void removeFlags(std::initializer_list<animation_t> list);
	void setFlagState(std::initializer_list<animation_t> flagSet);
	bool areFlagsSet(std::initializer_list<animation_t> flagSet);

	int getSocial();
	void queueThought(struct order_t* thought);
	void interruptThought(struct order_t* thought);

	void see();
	void registerEntity(Playable* playable);
	void emptyRegistry();

	void clipToGrid();
private:
	Brain* brain;

	AnimSprite* currSprite;
	AnimSprite* animations[MAX_ANIMS];
	
	sf::Texture* _texture;
	int _width, _height;
	float _x, _y;
	double xspeed = 0, yspeed = 0;
	struct order_t* currMove = NULL;
	int prevFlagState = 0;
	bool isPlayer;
	animation_t _flagSet[NUM_FLAGS];

	move_t currStep = move_t::NONE;
	int moveGoalX, moveGoalY;
	
	sf::Clock actionTimer;
	void manageAnimations();

	float orientation;
	float delta_orientation;

	double goalAngle = 0;
	int distSq = 0;

	int bufferFrames = 0;
};