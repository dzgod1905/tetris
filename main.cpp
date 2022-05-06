#include "Game.h"
#include <iostream>

int main(int argc, char* args[])
{
    using namespace std;
    srand(time(NULL));
    Game* game=new Game();
    const char* title="Basic Tetris game";
    if(game->init(title))
    {
        while(game->isRunning())
        {
            game->setCurrentTime(SDL_GetTicks());
            game->handleEvent();
            game->updateGame();
            game->loadScreen();
        }
    }
    game->close();
    return 0;
}
