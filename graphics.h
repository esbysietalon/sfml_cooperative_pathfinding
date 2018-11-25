#pragma once

#include <SFML/Graphics.hpp>
#include <map>
class Graphics {
	public:
		Graphics();
		~Graphics();
		sf::Texture* loadImage(std::string filePath);
	private:
		std::map<std::string, sf::Texture*> textures;
};