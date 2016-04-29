#include "Enemy.h"
#include "RenderManager.h"
#include "Texture.h"

Enemy::Enemy(float posX, float posY)
{
	m_pNormalTexture = new Texture("./Images/agent_search.png");
	m_pAngryTexture = new Texture("./Images/agent_angry.png");
	m_Pos.x = posX;
	m_Pos.y = posY;
	m_bAngry = false;
}

Enemy::~Enemy()
{
	delete m_pNormalTexture;
	delete m_pAngryTexture;
}

void Enemy::Draw(SpriteBatch *pSpritebatch)
{
	RenderManager* pRender = RenderManager::Instance();
	if(m_bAngry)
		pRender->DrawSprite(m_pAngryTexture, m_Pos.x, m_Pos.y);
	else
		pRender->DrawSprite(m_pNormalTexture, m_Pos.x, m_Pos.y);
}

// Set the enemy to be angry at the player.
// This will change its appearance.
void Enemy::SetAngry(bool bAngry)
{
	//Change the enemy's state.
	m_bAngry = bAngry;
}

const Vector2& Enemy::GetPos()
{
	return m_Pos;
}

