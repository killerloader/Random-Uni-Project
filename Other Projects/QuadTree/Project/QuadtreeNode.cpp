#include "QuadtreeNode.h"
#include "Enemy.h"
#include <iostream>

#define maxChildren 4

QuadtreeNode::QuadtreeNode(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY)
{
	//Create all children node and set them to nullptr.
	children = new QuadtreeNode*[maxChildren];
	for (int i = 0; i < maxChildren; i++)
		children[i] = nullptr;

	topLeftPoint.x = topLeftX;
	topLeftPoint.y = topLeftY;
	bottomRightPoint.x = bottomRightX;
	bottomRightPoint.y = bottomRightY;
}

//Divides the node into four nodes (non-adaptive)
void QuadtreeNode::subdivide()
{
	float widthHeight = (bottomRightPoint.x - topLeftPoint.x);
	Vector2 middleLoc;
	middleLoc.x = topLeftPoint.x + widthHeight / 2;
	middleLoc.y = topLeftPoint.y + widthHeight / 2;

	for (int i = 0; i < maxChildren; i++)
	{
		if (children[i] == nullptr)
		{
			Vector2 startPos;
			Vector2 endPos;

			switch (i)
			{
			case E_TOPLEFT:
				startPos = topLeftPoint;
				endPos = middleLoc;
				break;
			case E_TOPRIGHT:
				startPos.x = middleLoc.x;
				startPos.y = topLeftPoint.y;
				endPos.x = bottomRightPoint.x;
				endPos.y = middleLoc.y;
				break;
			case E_BOTTOMRIGHT:
				startPos = middleLoc;
				endPos = bottomRightPoint;
				break;
			case E_BOTTOMLEFT:
				startPos.x = topLeftPoint.x;
				startPos.y = middleLoc.y;
				endPos.x = middleLoc.x;
				endPos.y = bottomRightPoint.y;
				break;
			}
			children[i] = new QuadtreeNode(startPos.x, startPos.y, endPos.x, endPos.y);
		}
	}
}

QuadtreeNode* QuadtreeNode::pointInsideWhichNode(float x, float y)
{
	//Won't return a cell that doesn't exist.

	for (int i = 0; i < maxChildren; i++)
		if (children[i] != nullptr)
			if (children[i]->isPointInside(x, y))
				return children[i];
	return nullptr;
}

bool QuadtreeNode::hasChildren()
{
	for (int i = 0; i < maxChildren; i++)
		if (children[i] != nullptr)
			return true;
	return false;
}

bool QuadtreeNode::isPointInside(float x, float y)
{
	return	x >= topLeftPoint.x &&
		x <= bottomRightPoint.x &&
		y >= topLeftPoint.y &&
		y <= bottomRightPoint.y;
}

QuadtreeNode::~QuadtreeNode()
{

}