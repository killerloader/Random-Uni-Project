#pragma once
#include <SFML/Graphics.hpp>

class fireBall
{
public:
	fireBall(float x_, float y_, float angle_, sf::Texture &Tex, sf::RenderWindow &RenderRef_);
	~fireBall();

	void draw();
	void step();

private:
	float x, y, angle;
	sf::RenderWindow &RenderRef;
	sf::Sprite fireballSpr;
};

