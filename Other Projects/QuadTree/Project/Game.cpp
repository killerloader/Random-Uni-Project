/*-----------------------------------------------------------------------------
Author:			Richard Stern, based on code by Elizabeth Rowlands			
Description:	Finite State machine
-----------------------------------------------------------------------------*/

#include "Game.h"
#include "RenderManager.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include "quadTree.h"
#include "Enemy.h"

#include <list>
#include <stdio.h>
#include <time.h>

// Used to arrange enemies in a square grid that fills most of the screen
#define SCREEN_PADDING_X 200
#define SCREEN_PADDING_Y 200
#define ENEMY_SPACING 100

Game::Game(unsigned int windowWidth, unsigned int windowHeight, bool fullscreen, const char *title) : Application(windowWidth, windowHeight, fullscreen, title)
{
	m_nFramesPassed = 0;
	m_fTimePassed = 0.0f;
	m_fFPS = 0.0f;

	SetVSync(false);
	srand((unsigned int)time(nullptr));

	// Load in the textures that we want to use
	m_backgroundTexture = new Texture("./Images/background.png");
	m_mouseTexture = new Texture("./Images/Player.png");
	
	// Create the font for use with draw string
	m_font = new Font("./Fonts/calibri_24px.fnt");

	// Defaults for mouse X and Y
	m_v2MousePos.x = 0.0f;
	m_v2MousePos.y = 0.0f;
	m_fMouseRadius = 100.0f;

	Vector2 v2LevelTopLeft;
	v2LevelTopLeft.x = 0.0f;
	v2LevelTopLeft.y = 0.0f;
	Vector2 v2LevelBottomRight;
	v2LevelBottomRight.x = (float)windowWidth;
	v2LevelBottomRight.y = (float)windowHeight;

	// Spawn a grid of slightly randomly placed enemies.
	// First calculate how many enemies we can fit on the screen.
	m_MaxEnemiesPerRow = (windowWidth - SCREEN_PADDING_X) / ENEMY_SPACING;
	m_MaxEnemiesPerColumn = (windowHeight - SCREEN_PADDING_Y) / ENEMY_SPACING;
	m_EnemyCount = m_MaxEnemiesPerRow * m_MaxEnemiesPerColumn;

	//Instantiate the quadtree.
	gameTree = new Quadtree(0,0, v2LevelBottomRight.x, v2LevelBottomRight.y);

	// Create the enemies
	m_apEnemyList = new Enemy*[m_MaxEnemiesPerRow * m_MaxEnemiesPerColumn];
	for(int x = 0; x < m_MaxEnemiesPerRow; ++x)
	{
		for(int y = 0; y < m_MaxEnemiesPerColumn; ++y)
		{
			int index = (y * m_MaxEnemiesPerRow) + x;
			float posX = (float)SCREEN_PADDING_X + (x * ENEMY_SPACING);
			float posY = (float)SCREEN_PADDING_Y + (y * ENEMY_SPACING);

			posX += -ENEMY_SPACING + rand() % (ENEMY_SPACING * 2);
			posY += -(ENEMY_SPACING / 2) + rand() % (ENEMY_SPACING);

			m_apEnemyList[index] = new Enemy(posX, posY);

			//Add enemy to quadtree.
			gameTree->addEnemy(m_apEnemyList[index]);
		}
	}

}

Game::~Game()
{
	// DELETE EVERYTHING!!!
	for(int i = 0; i < m_EnemyCount; ++i)
	{
		delete m_apEnemyList[i];
	}
	delete m_apEnemyList;

	delete gameTree;

	delete m_backgroundTexture;
	delete m_mouseTexture;

	delete m_font;
}

void Game::Update(float deltaTime)
{
	// Calculate FPS
	m_fTimePassed += deltaTime;
	m_nFramesPassed++;
	if(m_fTimePassed > 0.25f && m_nFramesPassed > 5)
	{
		m_fFPS = ((float)m_nFramesPassed / m_fTimePassed);
		m_fTimePassed = 0.0f;
		m_nFramesPassed = 0;
	}

	// Get the mouse X and Y position
	int mouseX;
	int mouseY;
	Input::Instance()->GetMouseXY(&mouseX, &mouseY);
	m_v2MousePos.x = (float)mouseX;
	m_v2MousePos.y = (float)mouseY;

	// Update all the enemies
	UpdateEnemies(deltaTime);
}

void Game::Draw()
{
	// clear the back buffer
	ClearScreen();
	
	RenderManager* pRender = RenderManager::Instance();
	pRender->Begin();

	// Draw the background
	pRender->DrawSprite(m_backgroundTexture, SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Draw mouse texture
	pRender->DrawSprite(m_mouseTexture, m_v2MousePos.x, m_v2MousePos.y);

	// Draw info text
	char buffer[512];
	sprintf_s(buffer, 512, "Enemies within range of the mouse turn red.\nThis is linearly checking the distance to every enemy which is slow!\n(The distance check has been artificially slowed down in this project to make this even more obvious)\nA Quadtree would reduce the number of distance checks and would make this a lot faster.\nFPS: %.2f", m_fFPS);
	pRender->DrawText(m_font, buffer, 50, 30);

	// Draw enemies
	for(int i = 0; i < m_EnemyCount; ++i)
	{
		m_apEnemyList[i]->Draw(m_spritebatch);
	}

	gameTree->draw();

	pRender->End();
}

// Update all the enemies so they become aggresive if the mouse cursor is within a certain distance of them.
// Change them back to normal once the mouse leaves that area
void Game::UpdateEnemies(float deltatime)
{
	//Reset all enemies
	for(int i = 0; i < m_EnemyCount; ++i)
	{
		m_apEnemyList[i]->SetAngry(false);
	}

	//Linear trawl through all the enemies in the level to check if they are close to the mouse
	//If so make them angry. This is slow!
	/*for(int i = 0; i < m_EnemyCount; ++i)
	{
		float fDist = Vector2::GetDistance(m_apEnemyList[i]->GetPos(), m_v2MousePos);
		if(fDist < m_fMouseRadius)
			m_apEnemyList[i]->SetAngry(true);
	}*/

	std::list<Enemy*>* enemyList = gameTree->searchCircle(m_v2MousePos.x, m_v2MousePos.y, 50);

	std::list<Enemy*>::iterator i = enemyList->begin();
	for (i; i != enemyList->end(); i++)
	{
		(*i)->SetAngry(true);
	}

	//Use Quadtree here instead to find enemies that are roughly nearby and then do the distance check just on those ones to confirm.
	
	//<BEGIN PSEUDOCODE>
	//enemyList = QuadTree.Get(mousePos, m_fMouseRadius);
	//LOOP through enemies in enemyList
		//float fDist = Vector2::GetDistance(m_apEnemyList[i]->GetPos(), m_v2MousePos);
		//if(fDist < m_fMouseRadius)
			//m_apEnemyList[i]->SetAngry(true);
	//END LOOP
	//<END PSEUDOCODE>
}

