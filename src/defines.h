/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstdint>

#ifndef MAX_PATH
    #define MAX_PATH    4096
#endif

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

#define WALL_X1 40
#define WALL_X2 (SCREEN_WIDTH - 26)
#define WALL_Y1 90
#define WALL_Y2 (SCREEN_HEIGHT - 40)

#define RACKET_X      (WALL_X2 - 30)
#define BRICK_X       (WALL_X1 + (WALL_Y2 - WALL_Y1 - BRICKS_HEIGHT * BRICK_H) / 2)
#define BRICK_Y       (WALL_Y1 + (WALL_Y2 - WALL_Y1 - BRICKS_HEIGHT * BRICK_H) / 2)
#define BRICK_W       20
#define BRICK_H       30
#define BRICKS_WIDTH  13
#define BRICKS_HEIGHT 11

#define DESIRED_FPS 60

enum { APPS_INTRO, APPS_MAINMENU, APPS_GAME, APPS_SHOULDGETNAME, APPS_EDITOR, APPS_EXIT };
enum { BOX_NONE,
       BOX_0,     BOX_1,     BOX_2,     BOX_3,     BOX_4,     BOX_5,     BOX_6,     BOX_7,     BOX_8,     BOX_9,   // static series
       BOX_10,    BOX_11,    BOX_12,    BOX_13,    BOX_14,    BOX_15,     BOX_16,    BOX_17,    BOX_18,    BOX_19,
       BOX_20,    BOX_21,    BOX_22,    BOX_23,    BOX_24,    BOX_25,    BOX_26,    BOX_27,    BOX_28,    BOX_29,
       BOX_30,    BOX_31,    BOX_32,    BOX_33,    BOX_34,    BOX_35,    BOX_36,    BOX_37,    BOX_38,    BOX_39,
       BOX_40,    BOX_41,    BOX_42,    BOX_43,    BOX_44,    BOX_45,    BOX_46,    BOX_47,    BOX_48,    BOX_49,  // animated series
       BOX_DBL_0, BOX_DBL_1, BOX_DBL_2, BOX_DBL_3, BOX_DBL_4, BOX_DBL_5, BOX_DBL_6, BOX_DBL_7, BOX_DBL_8, BOX_DBL_9,
       BOX_TRD_0, BOX_TRD_1, BOX_TRD_2, BOX_TRD_3, BOX_TRD_4, BOX_TRD_5, BOX_TRD_6, BOX_TRD_7, BOX_TRD_8, BOX_TRD_9,
       BOX_FOU_0, BOX_FOU_1, BOX_FOU_2, BOX_FOU_3, BOX_FOU_4, BOX_FOU_5, BOX_FOU_6, BOX_FOU_7, BOX_FOU_8, BOX_FOU_9,
       BOX_IM_0,  BOX_IM_1,  BOX_IM_2,
       BOX_MOV_0, BOX_MOV_1, BOX_MOV_2,
       BOX_SHOOT_0, BOX_SHOOT_1,
       BOX_BOMB_0, BOX_BOMB_1,
       BOX_END
     };

struct _BRICK
{
    float   fX, fY;             // brick's position on screen in px
    unsigned char byType;//, byDesiredType;
    int     nCountToDie;        // count to die brick
    int     nFrame;
    int     nAnimateType;           // -1 - don't animate, 0 - forward, 1 - backward
    bool    bDir;                   // for movable bricks
    int     nDirChangeCount;    //
    int     nCountToShoot;      // for shooting bricks
};

struct sSave
{
    char mag[6];
    int level;
    int score;
    int lives;
    int scoreToAdditionalBall;
    int bonusLevelType;
    int bonusLevelTicks;
    int getReadyBallsLose;
    int bricksCount;
    int bonusesCount;
};

struct SDL_Surface;
struct SDL_Rect;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;
struct Mix_Chunk;

extern SDL_Surface*  g_psurfScreen;

extern int g_bppList[5];
struct sOptions
{
    int musicVolume = 3;
    int soundVolume = 4;
    uint32_t bppIdx = 0;
    bool fullscreen = false;
    bool opengl = true;
    bool showFps = false;
    bool tutorialMode = true;
    bool autoBonusMode = true;
};

struct sHighscores
{
    uint32_t lastPos;
    uint32_t lastScore;
    uint32_t lastLevel;
    struct sEntry
    {
        char name[100];
        uint32_t level;
        uint32_t score;
    };
    sEntry entries[10];
};

extern float        g_fSpeedCorrection;
extern float        g_fCos[];
extern float        g_fSin[];
extern int          g_nGameMode;
extern unsigned     g_modState;
extern bool         g_bMouseRB;
extern bool         g_bMouseLB;
extern float        g_nMouseDX;
extern float        g_nMouseDY;
extern int          g_nCursorX;
extern int          g_nCursorY;
extern bool         g_bIsCursorVisible;

extern SDL_Surface* m_pEnergyHole;
extern SDL_Surface* m_pMonstCopter;
extern SDL_Surface* m_pMonstPatrol;
extern SDL_Surface* m_pMonstEye;
extern SDL_Surface* m_pMonstBlackHole;
extern SDL_Surface* m_pMonstFighter;
extern SDL_Surface* m_pMonstHand;
extern SDL_Surface* m_pMonstStone1;
extern SDL_Surface* m_pMonstStone2;
extern SDL_Surface* m_pMonstTurbine;
extern SDL_Surface* m_pMonstUfo;
extern SDL_Surface* m_pMonstWreckage1;
extern SDL_Surface* m_pMonstWreckage2;
extern SDL_Surface* m_pBall;
extern SDL_Surface* m_pVector;
extern SDL_Surface* m_pFB;
extern SDL_Surface* m_pBullet;
extern SDL_Surface* m_pExploision;
extern SDL_Surface* m_pBricks;
extern SDL_Surface* m_pBricksMov;
extern SDL_Surface* m_pBricksMovBullets;
extern SDL_Surface* m_pBonuses;
extern SDL_Surface* g_pBonusesSmall;
extern SDL_Surface* g_pBonusesAura;
extern SDL_Surface* g_pGameBG;
extern SDL_Surface* m_pGameBGanims;
extern SDL_Surface* m_pRacket;
extern SDL_Surface* m_pGameWall;
extern SDL_Surface* m_pRacket;
extern SDL_Surface* m_pBackground;
extern SDL_Surface* m_pBackground2;
extern SDL_Surface* m_pMainMenuIcons;
extern SDL_Surface* g_pTransp;
extern SDL_Surface* g_pTutorialDlg;
extern SDL_Surface* g_pSinusString;
extern SDL_Surface* g_pOptions;
//extern SDL_Surface* g_pBGStars;

extern Mix_Chunk*    g_apSnd[];
extern Mix_Music*    g_apMod[];
extern int          g_nVolumeM;
extern int          g_nVolumeS;

extern void EnableCursor(bool bEnable);
extern SDL_Surface* LoadImage(const char* pchFileName, const unsigned nColorKey = 0xff000000);
extern void Blit(const int nX, const int nY, SDL_Surface* pImg, SDL_Rect* pSrc);
extern void BlitStretch(int nX, int nY, SDL_Surface* pImg, SDL_Rect* pSrc, float fScale);
extern void SetRect(SDL_Rect* pRc, int nX, int nY, int nW, int nH);
extern char* MakePath(const char* pchFileName);
extern bool IsKeyPressed(int nKey);
extern bool IsKeyStateChanged(int nKey);
extern void FadeScreen();
extern void SwitchFullscreen();
extern void PlayMusic(bool bFromFirst);
extern int PlaySound(int nSndIndex, int nLoopsCount = 0);
extern void StopSound(int& nChannel);
extern void SetVolumeMusic(int nVolume);
extern void SetVolumeSound(int nVolume);
