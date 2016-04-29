#pragma once

#include <list>

#include "QuadtreeNode.h"

class Enemy;

class Quadtree
{
	typedef QuadtreeNode node;

public:
	Quadtree(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY);
	~Quadtree();

	void draw();
	void drawOne(node* drawNode);

	void addEnemy(Enemy* newEnemy);
	std::list<Enemy*>* search(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY);
	std::list<Enemy*>* searchCircle(float X, float Y, float Radius);

private:
	node* m_root;
	std::list<Enemy*>* m_foundEnemies;
};