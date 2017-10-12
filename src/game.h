/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "bonus.h"
#include "generatelevel.h"
#include "vector.h"
#include "defines.h"

#include <string>
#include <vector>

class CGame final
{
    friend class CGenerateLevel;
    friend class CBall;
    friend class CBullet;
    friend class CMonster;

public:
    CGame(const sOptions& options);
    ~CGame();

    enum
    {
        GS_GAME_ACTIVE,
        /*GS_GAME_RESTORE, */ GS_GAME_PAUSED,
        GS_GET_READY,
        GS_GAME_OVER,
        GS_GAME_AREYOUSURE_EXIT,
        GS_GAME_REMINDER1,
        GS_GAME_REMINDER2
    };
    enum _RACKET_TYPE
    {
        RT_NORMAL,
        RT_LASER,
        RT_MISSILE,
        RT_PLASMA,
        RT_MAGNET,
        RT_ENGINE
    };
    void SetPause();
    bool DrawReminder(int nReminderType);
    void AddScore(int nScore);
    void DoImpact(sBrick& brick, bool bRemoveAll);
    //void DoImpact2(int nBx, int nBy, int nBrick, bool bRemoveAll);
    void ProcessBonus(CBonus::eType nBonusType);
    int getScore() const
    {
        return m_nScore;
    }
    int getLevel() const
    {
        return m_nCurrentLevel;
    }
    bool DrawScreen();
    void InitNewGame(bool bIsCustomLevels);
    void Unload();
    bool DrawGetReady();
    void addGetReadyInfo(const char* pchString);
    void RemoveOneLives();
    void DrawBackground();
    void RestoreGame();
    void SendEsc();
    void LoadBackground();
    void FreeBackground();

    const sVector<float>& getPaddlePosition() const
    {
        return m_paddlePosition;
    }

private:
    void DrawPause();
    void _Cheater();
    void ChangeBrick(sBrick& brick, Uint8 byToBrickType, bool bRemoveAll = false);
    void DoShoot();
    void DrawStatistic();
    bool DoGameOver();
    void DoGameActive();
    void DrawPaddle();
    void InitLevel(int nLevel, bool bRestore = false);
    void DoBomb(const sBrick& target);
    void DrawBrick(size_t idx, bool bNextFrameAnim);
    void DrawBricks();
    void ResetAll();
    void DrawBrickBullets();
    void MoveBrickBullets();
    int CalcBrickBulletsAngle(int nIndex, int nX, int nY);
    bool IsEmptyBrickPos(int nSkipPos, int nX, int nY);
    void DrawAreYouSure();
    //void DrawRestoreGame();

private:
    const sOptions& m_options;
    int m_nLaserX;
    std::vector<sBrick> m_bricks;
    int m_nLevelPrev; // store previous level number: -1 - usualy, -2 - inform, that we restore game
    int m_nBonusLevelType;
    int m_nBonusLevelTicks;
    int m_nSndWall; // store channel number for stop
    CGenerateLevel m_GenLev;
    Uint32 m_dwUnregisterdTime;
    int m_nScoreToAdditionalBall;
    Uint8 m_byChar;
    struct sBrickBullet
    {
        int nType;
        int nAngle;
        int nFrame;
        float fX, fY;
    };
    std::vector<sBrickBullet> m_vecBrickBullets;
    int m_nTotalBonusesInLevel;
    int m_nCanMovePaddleCount;
    bool m_bPaddleIsInvert;
    int m_nPaddleInvertCount;
    bool m_bCanMovePaddle;
    int m_nCountSelfDestruct;
    bool m_bSelfDestructStarted;
    int m_nCountBackWall;
    bool m_bBackWall;
    int m_nAddNewMonsterCount;
    int m_nSlowDownCount;
    bool m_bSlowDown;
    int m_nClockFrame;
    int m_nLives;
    int m_nGameState;
    sVector<float> m_paddlePosition;
    int m_nRacketSize;
    int m_nRacketType;
    int m_nPaddleMissileCount;
    int m_nScore;
    int m_nBrickCount;
    int m_nCurrentLevel;
    int m_nLevelImage;
    int m_nGetReadyScore;
    int m_nGetReadyMonsters;
    int m_nGetReadyBonuses;
    int m_nGetReadyBricks;
    int m_nGetReadyBallsLose;
    Uint32 m_dwCombosTime;
    int m_nCombosBricks;
    std::string m_getReadyInfo;
    bool m_bTutorialPause;
    bool m_bIsCustomLevels;

    SDL_Surface* m_background = nullptr;
};
