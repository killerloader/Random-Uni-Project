#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "Vector2.h"
#include "RenderManager.h"
#include <cmath>

static bool CircleSquareCollision(const Vector2& v2CircleCentre, float fRadius, const Vector2& v2SquareTopLeft, const Vector2& v2SquareBottomRight)
{
	float fSquareWidth = v2SquareBottomRight.x - v2SquareTopLeft.x;
	float fSquareHeight = v2SquareBottomRight.y - v2SquareTopLeft.y;
	float fSquareCentreX = v2SquareTopLeft.x + fSquareWidth * 0.5f;
	float fSquareCentreY = v2SquareTopLeft.y + fSquareHeight * 0.5f;

	float fDistanceX = abs(v2CircleCentre.x - fSquareCentreX);
	float fDistanceY = abs(v2CircleCentre.y - fSquareCentreY);

	if (fDistanceX > (fSquareWidth * 0.5f + fRadius))
		return false;

	if (fDistanceY > (fSquareHeight * 0.5f + fRadius))
		return false;

	if (fDistanceX <= (fSquareWidth * 0.5f))
		return true;

	if (fDistanceY <= (fSquareHeight * 0.5f))
		return true;

	float fCornerDistance_sq = powf((fDistanceX - fSquareWidth * 0.5f), 2) + powf((fDistanceY - fSquareHeight * 0.5f), 2);

	return (fCornerDistance_sq <= powf(fRadius, 2));
}

static void DrawSquare(Vector2& v2TopLeft, Vector2& v2BottomRight)
{
	RenderManager* pRender = RenderManager::Instance();
	pRender->DrawLine(v2TopLeft.x, v2TopLeft.y, v2BottomRight.x, v2TopLeft.y);
	pRender->DrawLine(v2TopLeft.x, v2BottomRight.y, v2BottomRight.x, v2BottomRight.y);
	pRender->DrawLine(v2TopLeft.x, v2TopLeft.y, v2TopLeft.x, v2BottomRight.y);
	pRender->DrawLine(v2BottomRight.x, v2TopLeft.y, v2BottomRight.x, v2BottomRight.y);
}

#endif