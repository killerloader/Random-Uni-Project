#ifndef ENEMY_H
#define ENEMY_H

#include "Vector2.h"

class SpriteBatch;
class Texture;

class Enemy
{
public:
	Enemy(float posX, float posY);
	~Enemy();

	void Draw(SpriteBatch *pSpritebatch);

	// Set the enemy to be angry at the player.
	// This will change its appearance.
	void SetAngry(bool bAngry);

	const Vector2& GetPos();

private:
	Texture* m_pNormalTexture;
	Texture* m_pAngryTexture;
	
	Vector2 m_Pos;
	bool m_bAngry;
};

#endif

