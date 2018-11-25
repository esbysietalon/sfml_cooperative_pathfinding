#include "stdafx.h"
#include "playable.h"
#include "globals.h"

Playable::Playable() {

}

Playable::Playable(sf::Texture* texture, int w, int h, int x, int y) {
	currSprite = new AnimSprite(texture, w, h, 0, 0, 0);
	currSprite->getSprite()->setPosition(x, y);
	_texture = texture;
	_width = w;
	_height = h;
	_x = x;
	_y = y;
	setFlagState({ animation_t::IDLE });
	for (int i = 0; i < MAX_ANIMS; i++) {
		animations[i] = NULL;
	}
	animations[getFlagState({ animation_t::IDLE })] = currSprite;
	brain = new Brain(this);
	isPlayer = false;
}

Playable::~Playable()
{
}

bool Playable::isControlled()
{
	return isPlayer;
}

void Playable::setControl(bool control)
{
	isPlayer = control;
}

int intpow(int base, int exponent) {
	int out = 1;
	while (exponent > 0) {
		exponent--;
		out *= base;
	}
	return out;
}

void Playable::addAnimation(int sx, int sy, int len, animation_t flagSet[]) {
	AnimSprite* sprite = new AnimSprite(_texture, _width, _height, sx, sy, len);
	animations[getFlagState(flagSet)] = sprite;
}
void Playable::addAnimation(int sx, int sy, int len, std::initializer_list<animation_t> flagSet) {
	AnimSprite* sprite = new AnimSprite(_texture, _width, _height, sx, sy, len);
	animations[getFlagState(flagSet)] = sprite;
}

int Playable::getFlagState(animation_t flagSet[])
{
	int out = 0;
	for (int i = 0; i < animation_t::NUM_FLAGS; i++) {
		if (flagSet[i] > -1) {
			out += intpow(2, flagSet[i]);
		}
	}
	return out;
}
int Playable::getFlagState(std::initializer_list<animation_t> list) {
	int out = 0;
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			out += intpow(2, list.begin()[i]);
		}
	}
	//fprintf(stderr, "%d - list\n", out);
	return out;
}

void Playable::setFlagState(std::initializer_list<animation_t> list) {
	for (int i = 0; i < animation_t::NUM_FLAGS; i++) {
		_flagSet[i] = animation_t::INVALID;
	}
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			_flagSet[list.begin()[i]] = list.begin()[i];
		}
	}
}
bool Playable::areFlagsSet(std::initializer_list<animation_t> list)
{
	for(int i = 0; i < list.size(); i++){
		if(list.begin()[i] > animation_t::INVALID){
			if(_flagSet[list.begin()[i]] <= animation_t::INVALID){
				return false;
			}
		}
	}
	return true;
}
void Playable::addFlags(std::initializer_list<animation_t> list) {
	for (int i = 0; i < list.size(); i++) {
if (list.begin()[i] > animation_t::INVALID) {
	_flagSet[list.begin()[i]] = list.begin()[i];
}
	}
}
void Playable::removeFlags(std::initializer_list<animation_t> list) {
	for (int i = 0; i < list.size(); i++) {
		if (list.begin()[i] > animation_t::INVALID) {
			_flagSet[list.begin()[i]] = animation_t::INVALID;
		}
	}
}
void Playable::manageAnimations() {
	int flagState = getFlagState(_flagSet);
	//fprintf(stderr, "%d\n", flagState);
	if (flagState > 0) {
		if (animations[flagState] != NULL) {
			if (prevFlagState != flagState) {
				currSprite->resetFrame();
				currSprite = animations[getFlagState(_flagSet)];
				prevFlagState = flagState;
			}
			//fprintf(stderr, " non-null animation access \n");
		}
		else {
			currSprite = animations[getFlagState({ animation_t::IDLE })];
		}
	}
	else {
		currSprite = animations[getFlagState({ animation_t::IDLE })];
	}

}
sf::Sprite* Playable::getSprite()
{
	return currSprite->getSprite();
}

void Playable::update() {
	//CONTROLLING (MOVING) AND ANIMATING PLAYER SPRITE
	//fprintf(stderr, "player_update\n");
	if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
		addFlags({ animation_t::FACE_RIGHT });
	}
	if (areFlagsSet({ animation_t::MOVE_LEFT })) {
		addFlags({ animation_t::FACE_LEFT });
	}
	removeFlags({ animation_t::MOVE_LEFT, animation_t::MOVE_RIGHT });
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		_y--;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_RIGHT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });

	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		_x--;
		addFlags({ animation_t::MOVE_LEFT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_RIGHT, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		_y++;
		if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			addFlags({ animation_t::MOVE_RIGHT });
			removeFlags({ animation_t::MOVE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_LEFT })) {
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT });
		}
		else {
			addFlags({ animation_t::MOVE_LEFT });
		}
		removeFlags({ animation_t::IDLE, animation_t::FACE_RIGHT, animation_t::FACE_LEFT });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		_x++;
		addFlags({ animation_t::MOVE_RIGHT });
		removeFlags({ animation_t::IDLE, animation_t::MOVE_LEFT, animation_t::FACE_RIGHT,animation_t::FACE_LEFT });
	}
	manageAnimations();
	currSprite->getSprite()->setPosition(_x, _y);
	currSprite->update();
}

int Playable::getSocial() {
	return brain->getSocial();
}

void Playable::queueThought(struct order_t* thought) {
	brain->queueThought(thought);
}

void Playable::interruptThought(struct order_t* thought) {
	brain->interruptThought(thought);
}

void Playable::registerEntity(Playable* playable) {
	brain->registerEntity(playable);
}

void Playable::emptyRegistry() {
	brain->emptyRegistry();
}

void Playable::update_AI()
{
	
	//THINKING - BRAIN ACTIVITY
	/*int randInt = rand();
	if (actionTimer.getElapsedTime().asSeconds() * 50 > randInt) {
		brain->think();
		currMove = brain->nextMove();
		goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
		xspeed = -cos(goalAngle);
		yspeed = -sin(goalAngle);
		actionTimer.restart();
	}*/
	//fprintf(stderr, "pre-think\n");
	brain->think();
	//fprintf(stderr, "post-think\n");
	//if (currMove == NULL) {
	currMove = brain->nextMove();
	//fprintf(stderr, "%d\n", currMove->type);
	if (currMove != NULL) {
		//fprintf(stderr, "x: %f, y: %f, goalX: %d, goalY: %d\n", getSprite()->getPosition().x, getSprite()->getPosition().y, currMove->x, currMove->y);
		goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
		xspeed = -cos(goalAngle);
		yspeed = -sin(goalAngle);
	}
	//}
	/*if (currMove != NULL) {
		float distSq = (currMove->x - getSprite()->getPosition().x) * (currMove->x - getSprite()->getPosition().x) + (currMove->y - getSprite()->getPosition().y) * (currMove->y - getSprite()->getPosition().y);
		if (distSq <= BASE_NE_DIST) {
			currMove = brain->nextMove();
			goalAngle = atan2(getSprite()->getPosition().y - currMove->y, getSprite()->getPosition().x - currMove->x);
			xspeed = -cos(goalAngle);
			yspeed = -sin(goalAngle);
		}
	}*/
	/*
	
	if (currMove != NULL) {
		float distSq = (currMove->x - getSprite()->getPosition().x) * (currMove->x - getSprite()->getPosition().x) + (currMove->y - getSprite()->getPosition().y) * (currMove->y - getSprite()->getPosition().y);
		if (distSq <= BASE_NE_DIST){
			xspeed = 0;
			yspeed = 0;
		}
	}
	*/
	//MOVING AND ANIMATING THE SPRITE
	if(xspeed * xspeed > 0 && yspeed * yspeed > 0){
		removeFlags({ animation_t::IDLE, animation_t::FACE_LEFT, animation_t::FACE_RIGHT });
		if (yspeed != 0) {
			if(areFlagsSet({animation_t::MOVE_LEFT})){
				addFlags({ animation_t::MOVE_LEFT });
				removeFlags({ animation_t::MOVE_RIGHT });
			}else if(areFlagsSet({ animation_t::MOVE_RIGHT })){
				addFlags({ animation_t::MOVE_RIGHT });
				removeFlags({ animation_t::MOVE_LEFT });
			}else{
				addFlags({ animation_t::MOVE_RIGHT });
			}
		}
		_x += xspeed;
		if (xspeed > 0) {
			addFlags({ animation_t::MOVE_RIGHT});
			removeFlags({ animation_t::MOVE_LEFT, animation_t::IDLE });
		}else{
			addFlags({ animation_t::MOVE_LEFT });
			removeFlags({ animation_t::MOVE_RIGHT, animation_t::IDLE });
		}
		_y += yspeed;
		
	}else{
		if(areFlagsSet({animation_t::MOVE_LEFT})){
			removeFlags({ animation_t::MOVE_LEFT });
			addFlags({ animation_t::FACE_LEFT });
		}
		else if (areFlagsSet({ animation_t::MOVE_RIGHT })) {
			removeFlags({ animation_t::MOVE_RIGHT });
			addFlags({ animation_t::FACE_RIGHT });
		}
	}
	manageAnimations();
	currSprite->getSprite()->setPosition(_x, _y);
	currSprite->update();
}
