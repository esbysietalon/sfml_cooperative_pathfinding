#include "stdafx.h"
#include "graphics.h"

Graphics::Graphics() {

}
Graphics::~Graphics() {

}
sf::Texture* Graphics::loadImage(std::string filePath) {
	if (textures.count(filePath) == 0) {
		sf::Texture* texture = new sf::Texture;
		texture->loadFromFile(filePath);
		textures[filePath] = texture;
	}
	return textures[filePath];
}