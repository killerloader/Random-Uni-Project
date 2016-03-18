#include "fireBall.h"

fireBall::fireBall(float x_, float y_, float angle_, sf::Texture &Tex, sf::RenderWindow &RenderRef_) : RenderRef(RenderRef_)
{
	fireballSpr.setTexture(Tex);
	fireballSpr.setPosition(sf::Vector2f(x_, y_));
	fireballSpr.setRotation(angle_);
	x = x_;
	y = y_;
	angle = angle_;
}

fireBall::~fireBall()
{
	
}

void fireBall::draw()
{
	RenderRef.draw(fireballSpr);
}

void fireBall::step()
{
	
}