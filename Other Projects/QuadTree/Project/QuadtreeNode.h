#pragma once

#include <list>

#include "Vector2.h"

class Enemy;

class QuadtreeNode
{
public:
	QuadtreeNode(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY);
	~QuadtreeNode();

	enum E_nodeLocation
	{
		E_TOPLEFT,
		E_TOPRIGHT,
		E_BOTTOMRIGHT,
		E_BOTTOMLEFT
	};

	void subdivide();
	QuadtreeNode* pointInsideWhichNode(float x, float y);
	bool isPointInside(float x, float y);
	bool hasChildren();

	std::list<Enemy*> data;

	QuadtreeNode** children;

	Vector2 topLeftPoint;
	Vector2 bottomRightPoint;

private:
};

