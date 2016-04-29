#include "Game.h"
#include <crtdbg.h>

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void main()
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	Game *pGame = new Game(1024, 768, false, "Game");
	pGame->RunGame();
	delete pGame;
};