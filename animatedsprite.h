#pragma once
#define MAX_PRIORITY 5
#define TIER_SPACE 64
#include <SFML/Graphics.hpp>



class AnimSprite {
public:
	AnimSprite();
	~AnimSprite();
	AnimSprite(sf::Texture* spritesheet, int w, int h, int sx, int sy, int len);
	
	void update();
	void resetFrame();
	sf::Sprite* getSprite();
private:
	sf::IntRect frame;
	sf::Sprite sprite;
	int sfx, sfy, efx, efy;
	
};