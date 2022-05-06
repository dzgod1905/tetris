#pragma once
#ifndef Game_H_
#define Game_H_

#include <SDL.h>
#include <SDL_image.h>
#include <bits/stdc++.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

class Game
{
public:
	Game(){}
	~Game(){}

	void setCurrentTime(Uint32 t) {currentTime=t;}
	bool isRunning() {return running;}
	void shapeDefine();
    bool init(const char* tiltle);
    bool checkinside(int x,int y,SDL_Rect rect);
    SDL_Texture* loadTexture(std::string text,SDL_Color color);
    void setRectPos(SDL_Rect &rect,int x=0,int y=0,int w=30,int h=30);
    void moveRectPos(SDL_Rect &rect,int x,int y);
    void nextTetrisGenerate();
    void handleEvent();
    void restart();
    void checkGameOver();
    bool validMove();
    void updateGame();
    void loadScreen();
    void close();


private:
    enum{BasicW=928,BasicH=742};
    enum{cols=10,rows=20};
    enum{blockSide=30};
    enum{leftBorder=314,upBorder=72};

    SDL_Window* window=NULL;
    SDL_Renderer* gameRenderer=NULL;
    SDL_Texture* blockart,* background,* scorePointTexture,* designer,* pauseBackground,* menuBackground,* gameoverBackground,* howtoplayBackground,* soundOn,* soundOff;
    SDL_Rect srcR={0,0,blockSide,blockSide},destR={0,0,blockSide,blockSide},pauseButtonPlaying={828,15,81,81},soundButtonPlaying={710,15,81,81};
    SDL_Rect resumeButtonPause={363,326,202,64},restartButtonPause={363,430,202,64},quitButtonPause={363,534,202,64};
    SDL_Rect playButtonMenu={317,335,293,92},howtoplayButtonMenu={317,495,293,92};
    SDL_Rect backButtonHowtoplay={415,610,99,99};
    SDL_Rect restartButtonGameover={350,375,88,88},homeButtonGameover={490,375,88,88};
    SDL_Color scorePointColor={247,245,242},designerColor={0,0,0};

    TTF_Font* font=NULL;

    Mix_Music* backgroundMusic = NULL;
    Mix_Chunk* buttonSound = NULL,* gameoverSound=NULL;

    bool running=false,isRotate=false,gameOver=false,isSpace=false,isSwapping=false;
    bool isPlaying=false,isPausing=false,isMenu=true,isRestart=false,isHowtoplay=false,isGameover=false;
    bool isSound=true,isMusic=true;
    std::stringstream scorePointText;
    std::pair<int,int> currentShape[4],backup[4],nextShape[4],savedShape[4],downShape[4];
    int currentColor=1,xpos=0,basicDelay=1000,nextColor=1,savedColor=1,backupColor=1,shapecnt=0,downColor=7;
    long long score=0;
    unsigned int nextFrame=basicDelay;
    static const int allShape[7][4];
    int board[rows][cols]={0};
    Uint32 currentTime=0,lastTime=0;
};


#endif // !Game_H_
