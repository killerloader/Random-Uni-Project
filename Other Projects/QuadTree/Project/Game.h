/*-----------------------------------------------------------------------------
Author:			Richard Stern, based on code by Elizabeth Rowlands			
Description:	Finite State machine
-----------------------------------------------------------------------------*/

#ifndef GAME1_H
#define GAME1_H

#include "Application.h"
#include "Vector2.h"

class SpriteBatch;
class Enemy;
class Quadtree;

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 768

class Game : public Application
{
public:
	// assets loaded in constructor
	Game(unsigned int windowWidth, unsigned int windowHeight, bool fullscreen, const char *title);

	// assets destroyed in destructor
	virtual ~Game();

	// update / draw called each frame automaticly
	virtual void Update(float deltaTime);
	virtual void Draw();

protected:
	SpriteBatch *m_spritebatch;
	
private:
	void UpdateEnemies(float deltatime);

	Texture *m_backgroundTexture;
	Texture *m_mouseTexture;

	Enemy** m_apEnemyList;
	int m_MaxEnemiesPerRow;
	int m_MaxEnemiesPerColumn;
	int m_EnemyCount;

	int m_nFramesPassed;
	float m_fTimePassed;
	float m_fFPS;

	Font *m_font;

	Quadtree* gameTree;

	Vector2 m_v2MousePos;
	float m_fMouseRadius;
};

#endif