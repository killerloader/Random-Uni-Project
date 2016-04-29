#pragma once

class Texture;

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
class Player
{
public:
	Player();
	~Player();

	void Update(float dt);
	void Draw();

private:
	Texture *m_pTexture;
};

