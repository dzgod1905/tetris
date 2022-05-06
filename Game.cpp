#include <iostream>
#include "Game.h"
/*
	0	1	2	3
	4	5	6	7
*/
const int Game::allShape[7][4] =
{
    0,1,2,3,	// I
    0,4,5,6,	// J
    2,6,5,4,	// L
    1,2,5,6,	// O
    2,1,5,4,	// S
    1,4,5,6,	// T
    0,1,5,6,	// Z
};

bool Game::init(const char* title)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)==0)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        window=SDL_CreateWindow(title,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,BasicW,BasicH,SDL_WINDOW_SHOWN);
        if (window==NULL) return false;
        gameRenderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
        if (gameRenderer==NULL) return false;
        SDL_SetRenderDrawColor(gameRenderer,255,255,255,255);
        int imgFlags=IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags)&imgFlags)) return false;
        if(TTF_Init()==-1) return false;
        font=TTF_OpenFont("res/font/FONT.ttf",36);
        if(font==NULL) return false;
        if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0) return false;

        SDL_Surface* loadedSurf=IMG_Load("res/image/background.jpg");
        background=SDL_CreateTextureFromSurface(gameRenderer, loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/blockart.jpg");
        blockart=SDL_CreateTextureFromSurface(gameRenderer, loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/pause.jpg");
        pauseBackground=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/mainmenu.jpg");
        menuBackground=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/gameover.jpg");
        gameoverBackground=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/howtoplay.jpg");
        howtoplayBackground=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/soundon.jpg");
        soundOn=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        loadedSurf=IMG_Load("res/image/soundoff.jpg");
        soundOff=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
        SDL_FreeSurface(loadedSurf);

        backgroundMusic=Mix_LoadMUS("res/sound/music.wav");
        if(backgroundMusic==NULL) return false;

        buttonSound=Mix_LoadWAV("res/sound/click.wav");
        if(buttonSound==NULL) return false;

        gameoverSound=Mix_LoadWAV("res/sound/gameover.wav");
        if(gameoverSound==NULL) return false;

        scorePointText<<score;
        savedShape[0].first=-1;
        nextTetrisGenerate();
        nextTetrisGenerate();
    }
    running = true;
    return true;
}

SDL_Texture* Game::loadTexture(std::string text,SDL_Color color)
{
    SDL_Surface* loadedSurf=TTF_RenderText_Solid(font,text.c_str(),color);
    if(loadedSurf==NULL)
    {
        SDL_FreeSurface(loadedSurf);
        return NULL;
    }
    SDL_Texture* newtexture=SDL_CreateTextureFromSurface(gameRenderer,loadedSurf);
    SDL_FreeSurface(loadedSurf);
    return newtexture;
}

void Game::nextTetrisGenerate()
{
    for(int i=0; i<4; i++) currentShape[i]=nextShape[i];
    for(int i=0; i<4; i++) downShape[i]=currentShape[i];
    currentColor=nextColor;
    nextColor =1+rand()%6;
    int n =rand()%7;
    if(shapecnt==8) n=0;
    if(n==0) shapecnt=0;
    else shapecnt++;
    for(int i=0; i<4; i++)
    {
        nextShape[i].first=allShape[n][i]%4;
        nextShape[i].second=allShape[n][i]/4;
    }
}

bool Game::checkinside(int x,int y,SDL_Rect rect)
{
    if(x<rect.x) return false;
    if(x>rect.x+rect.w) return false;
    if(y<rect.y) return false;
    if(y>rect.y+rect.h) return false;
    return true;
}

void Game::handleEvent()
{
    SDL_Event e;
    if(isPlaying)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
            {
                running=false;
                break;
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
                if(checkinside(x,y,pauseButtonPlaying))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    Mix_HaltMusic();
                    isPlaying=false;
                    isPausing=true;
                    break;
                }
                if(checkinside(x,y,soundButtonPlaying))
                {
                    if(isMusic) Mix_HaltMusic();
                    else Mix_PlayMusic(backgroundMusic,-1);
                    isSound=1-isSound;
                    isMusic=1-isMusic;
                    break;
                }
            }
            if(e.type==SDL_KEYDOWN)
            {
                int tmp=e.key.keysym.sym;
                if(tmp==SDLK_UP)
                {
                    isRotate=true;
                    break;
                }
                else if(tmp==SDLK_LEFT)
                {
                    xpos=-1;
                    break;
                }
                else if(tmp==SDLK_RIGHT)
                {
                    xpos=1;
                    break;
                }
                else if(tmp==SDLK_SPACE)
                {
                    isSpace=true;
                    break;
                }
                else if(tmp==SDLK_LSHIFT)
                {
                    isSwapping=true;
                    break;
                }
                else break;
            }
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_DOWN]) nextFrame = 50;
    }
    if(isPausing)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
            {
                running=false;
                break;
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
                if(checkinside(x,y,resumeButtonPause))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    if(isMusic) Mix_PlayMusic(backgroundMusic,-1);
                    isPlaying=true;
                    isPausing=false;
                    break;
                }
                if(checkinside(x,y,restartButtonPause))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    if(isMusic) Mix_PlayMusic(backgroundMusic,-1);
                    restart();
                    isPlaying=true;
                    isPausing=false;
                    break;
                }
                if(checkinside(x,y,quitButtonPause))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    isPausing=false;
                    isMenu=true;
                    break;
                }
            }
        }
    }
    if(isMenu)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
            {
                running=false;
                break;
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
                if(checkinside(x,y,playButtonMenu))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    if(isMusic) Mix_PlayMusic(backgroundMusic,-1);
                    isMenu=false;
                    isPlaying=true;
                    restart();
                    break;
                }
                if(checkinside(x,y,howtoplayButtonMenu))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    isMenu=false;
                    isHowtoplay=true;
                    break;
                }
            }
        }
    }
    if(isHowtoplay)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
            {
                running=false;
                break;
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
                if(checkinside(x,y,backButtonHowtoplay))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    isHowtoplay=false;
                    isMenu=true;
                    break;
                }
            }
        }
    }
    if(isGameover)
    {
        while (SDL_PollEvent(&e))
        {
            if(e.type==SDL_QUIT)
            {
                running=false;
                break;
            }
            if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x, y;
                SDL_GetMouseState( &x, &y );
                if(checkinside(x,y,restartButtonGameover))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    if(isMusic) Mix_PlayMusic(backgroundMusic,-1);
                    restart();
                    isGameover=false;
                    isPlaying=true;
                    break;
                }
                if(checkinside(x,y,homeButtonGameover))
                {
                    if(isSound) Mix_PlayChannel(-1,buttonSound,0);
                    isGameover=false;
                    isMenu=true;
                    break;
                }
            }
        }
    }
}

void Game::setRectPos(SDL_Rect& rect, int x, int y, int w, int h)
{
    rect= {x,y,w,h};
}

void Game::moveRectPos(SDL_Rect& rect, int x, int y)
{
    rect.x += x;
    rect.y += y;
}

void Game::restart()
{
    for(int i=0; i<rows; i++) for(int j=0; j<cols; j++) board[i][j]=0;
    savedShape[0].first=-1;
    score=0;
    scorePointText.str("");
    scorePointText.clear();
    scorePointText<<score;
    gameOver=false;
    nextTetrisGenerate();
    nextTetrisGenerate();
}

void Game::checkGameOver()
{
    int topRow=21;
    for(int i=0; i<4; i++) if(topRow>currentShape[i].second) topRow=currentShape[i].second;
    if(topRow<=2) gameOver=true;
}

bool Game::validMove()
{
    for (int i=0; i<4; i++)
    {
        if(currentShape[i].first< 0||currentShape[i].first>=cols||currentShape[i].second>=rows) return false;
        if(board[currentShape[i].second][currentShape[i].first]) return false;
    }
    return true;
}

void Game::updateGame()
{
    if(isPlaying)
    {
        // create backup for currentShape
        for (int i=0; i<4; i++) backup[i]=currentShape[i];
        backupColor=currentColor;
        // move shape
        if (xpos)
        {
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
            for (int i=0; i<4; i++) currentShape[i].first+=xpos;
            if (!validMove()) for (int i=0; i<4; i++) currentShape[i]=backup[i];
            for(int i=0; i<4; i++) backup[i]=currentShape[i];
            xpos=0;
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=downColor;
        }

        //rotate shape
        if (isRotate)
        {
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
            std::pair<int,int> center = currentShape[2];	// center of rotation
            for (int i=0; i<4; i++)
            {
                int xDis=currentShape[i].second-center.second;
                int yDis=currentShape[i].first-center.first;
                currentShape[i].first=center.first-xDis;
                currentShape[i].second=center.second+yDis;
            }
            if (!validMove()) for (int i = 0; i < 4; i++) currentShape[i] = backup[i];
            for(int i=0; i<4; i++) backup[i]=currentShape[i];
            isRotate=false;
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=downColor;
        }

        // swap with saved shape
        if(isSwapping)
        {
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
            if(savedShape[0].first==-1)
            {
                int top=21,left=1000000;
                for(int i=0; i<4; i++) top=std::min(top,currentShape[i].second),left=std::min(left,currentShape[i].first);
                for(int i=0; i<4; i++) savedShape[i]= {currentShape[i].first-left,currentShape[i].second-top};
                savedColor=currentColor;
                nextTetrisGenerate();
                for(int i=0; i<4; i++) backup[i]=currentShape[i];
                backupColor=currentColor;
            }
            else
            {
                int top=21,left=1000000;
                for(int i=0; i<4; i++) top=std::min(top,currentShape[i].second),left=std::min(left,currentShape[i].first);
                for(int i=0; i<4; i++) currentShape[i]= {savedShape[i].first+left,top+savedShape[i].second};
                currentColor=savedColor;
                if(!validMove())
                {
                    for(int i=0; i<4; i++) currentShape[i]=backup[i];
                    currentColor=backupColor;
                }
                else
                {
                    for(int i=0; i<4; i++) savedShape[i]= {backup[i].first-left,backup[i].second-top};
                    savedColor=backupColor;
                    for(int i=0; i<4; i++) backup[i]=currentShape[i];
                    backupColor=currentColor;
                }
            }
            for(int i=0; i<4; i++) downShape[i]=currentShape[i];
            isSwapping=false;
        }
        //std::cout << validMove() << "\n";

        //downShape generate
        for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
        while(validMove()) for(int i=0; i<4; i++) currentShape[i].second++;
        if(!validMove()) for(int i=0; i<4; i++)
            {
                currentShape[i].second--;
                board[currentShape[i].second][currentShape[i].first]=downColor;
                downShape[i]=currentShape[i];
                currentShape[i]=backup[i];
            }

        //nextFrame time
        if (currentTime-lastTime>nextFrame)
        {
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
            for (int i=0; i<4; i++) backup[i] = currentShape[i];
            for (int i=0; i<4; i++) currentShape[i].second++;
            if (!validMove()) // reach the bottom of the board
            {
                for (int i=0; i<4; i++) board[backup[i].second][backup[i].first]=currentColor;
                checkGameOver();
                if(gameOver)
                {
                    Mix_PlayChannel(-1,gameoverSound,0);
                    if(isMusic) Mix_HaltMusic();
                    isGameover=true;
                    isPlaying=false;
                }
                nextTetrisGenerate();
                if(isSpace) isSpace=false;
            }
            lastTime = currentTime;
            for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=downColor;
        }
        if(isSpace) nextFrame=0;
        else nextFrame=basicDelay;

        // check full lines
        for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=0;
        int currentRow=rows-1,eaten=0;
        for (int i=currentRow; i>=0; i--)
        {
            int cnt=0;
            for (int j=0; j<cols; j++)
            {
                if(board[i][j]) cnt++;
                board[currentRow][j]=board[i][j]; // set current row = test row
            }
            if (cnt<cols) currentRow--; // if current row cols = cols -> full row
            else eaten++;
        }
        if(eaten>0)
        {
            scorePointText.str("");
            scorePointText.clear();
            score+=1000*eaten+(eaten-1)*250;
            scorePointText<<score;
            for(int i=eaten; i>=0; i--)
            {
                for (int j=0; j<cols; j++)
                {
                    board[i][j]=0; // set current row = test row
                }
            }
            eaten=0;
        }
        for(int i=0; i<4; i++) board[downShape[i].second][downShape[i].first]=downColor;
    }
}

void Game::loadScreen()
{

    SDL_RenderClear(gameRenderer);
    if(isPlaying)
    {
        // background render
        SDL_RenderCopy(gameRenderer,background,NULL,NULL);

        //sound button render
        if(isSound) SDL_RenderCopy(gameRenderer,soundOn,NULL,&soundButtonPlaying);
        else SDL_RenderCopy(gameRenderer,soundOff,NULL,&soundButtonPlaying);

        // freeze shape render
        for (int i = 0; i < rows; i++) for (int j = 0; j < cols; j++) if (board[i][j])
                {
                    setRectPos(srcR, board[i][j] * blockSide);
                    setRectPos(destR, j * blockSide, i * blockSide);
                    moveRectPos(destR, leftBorder, upBorder);
                    SDL_RenderCopy(gameRenderer, blockart, &srcR, &destR);
                }

        // current shape render
        for (int i = 0; i < 4; i++)
        {
            setRectPos(srcR, currentColor * blockSide);
            setRectPos(destR, currentShape[i].first * blockSide, currentShape[i].second * blockSide);
            moveRectPos(destR, leftBorder, upBorder);
            SDL_RenderCopy(gameRenderer, blockart, &srcR, &destR);
        }

        //score point render
        int len=scorePointText.str().size();
        setRectPos(destR,164-len*15,567,len*30,84);
        scorePointTexture=loadTexture(scorePointText.str(),scorePointColor);
        SDL_RenderCopy(gameRenderer,scorePointTexture,NULL,&destR);


        // savedShape render
        if(savedShape[0].first!=-1) for (int i = 0; i < 4; i++)
            {
                setRectPos(srcR, savedColor * blockSide);
                setRectPos(destR, savedShape[i].first * blockSide, savedShape[i].second * blockSide);
                moveRectPos(destR,103,225);
                SDL_RenderCopy(gameRenderer, blockart, &srcR, &destR);
            }

        //nextShape render
        for (int i = 0; i < 4; i++)
        {
            setRectPos(srcR, nextColor * blockSide);
            setRectPos(destR, nextShape[i].first * blockSide, nextShape[i].second * blockSide);
            moveRectPos(destR,704,254);
            SDL_RenderCopy(gameRenderer, blockart, &srcR, &destR);
        }
    }

    if(isPausing) SDL_RenderCopy(gameRenderer,pauseBackground,NULL,NULL);
    if(isMenu) SDL_RenderCopy(gameRenderer,menuBackground,NULL,NULL);
    if(isGameover) SDL_RenderCopy(gameRenderer,gameoverBackground,NULL,NULL);
    if(isHowtoplay) SDL_RenderCopy(gameRenderer,howtoplayBackground,NULL,NULL);

    SDL_RenderPresent(gameRenderer);
}

void Game::close()
{
    SDL_DestroyTexture(blockart);
    SDL_DestroyTexture(background);
    SDL_DestroyTexture(scorePointTexture);
    SDL_DestroyRenderer(gameRenderer);

    TTF_CloseFont(font);

    Mix_FreeChunk(buttonSound);
    Mix_FreeChunk(gameoverSound);
    Mix_FreeMusic(backgroundMusic);

    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
