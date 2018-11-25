
#include "stdafx.h"
#include "animatedsprite.h"

AnimSprite::AnimSprite() {

}
AnimSprite::~AnimSprite()
{
}
AnimSprite::AnimSprite(sf::Texture* texture, int w, int h, int sx, int sy, int len) {
	frame.left = sx * w;
	frame.top = sy * h;
	frame.width = w;
	frame.height = h;
	sprite.setTexture(*texture);
	sprite.setTextureRect(frame);
	sfx = sx * w;
	sfy = sy * h;
	efx = sx * w + len * w;
	efy = sy * h;
}

void AnimSprite::resetFrame() {
	frame.left = sfx;
	frame.top = sfy;
	sprite.setTextureRect(frame);
}

void AnimSprite::update() {
	if (frame.left < efx) {
		frame.left += frame.width;
	}
	else {
		frame.left = sfx;
	}
	sprite.setTextureRect(frame);
}

sf::Sprite* AnimSprite::getSprite() {
	return &sprite;
}