#pragma once

#include <SFML/Graphics.hpp>
#include "playable.h"

#include <vector>

class Graphics;
class Terrain;

class Core {
public:
	Core();
	~Core();
private:
	void update();
	void load();
	void draw(sf::RenderWindow* window);
	void generateNPCs(int num);
	void mainLoop();
	std::vector<Playable*> characters;
	Graphics* graphics;
	Terrain* terrain;
	sf::Sprite** map;
};