#pragma once

#include <SFML/Graphics.hpp>
#include "playable.h"

#include <vector>

class Graphics;
class Terrain;

struct intpair {
	intpair(int i, int j) {
		x = i;
		y = j;
	}
	int x, y;
};

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
	void calculateCFov();
	void generateCFov();

	std::vector<std::vector<struct intpair>*> cFovRings;
	
	std::vector<sf::Sprite*> tokens;

	std::vector<Playable*> characters;
	Graphics* graphics;
	Terrain* terrain;
	sf::Sprite** map;
};