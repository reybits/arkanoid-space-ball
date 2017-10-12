/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"
#include "resource.h"

#include <vector>

class CMonster final
{
    friend class CGame;

public:
    CMonster();
    ~CMonster();

    enum class eMonsters : unsigned
    {
        PATROL,
        COPTER,
        EYE,
        BLACKHOLE,
        FIGHTER,
        HAND,
        TURBINE,
        UFO,
        STONE1,
        STONE2,

        WRECKAGE1,
        WRECKAGE2,

        Count
    };
    bool IsAcrossBall(const sBallDescription& desc, bool remove);
    bool IsAcrossBall(const sBallDescription& desc);
    bool IsAcrossPaddle(int nX, int nY, int nWidth, int nHeight);
    bool IsAcrossPaddle2(int nX, int nY, int nWidth, int nHeight);
    bool IsAcross(int nX, int nY, int nWidth, int nHeight, bool remove);
    void Move(bool bBackWall, int nPaddleY, int nPaddleHeight);
    void AddMonster(int x, int y, eMonsters type);
    void DrawMonsters();
    void DrawWreckage();
    void RemoveAll();

private:
    void ChangeMonsterAngle(size_t idx);
    void RemoveByPos(size_t idx);

private:
    int m_nSndPatrol;
    bool m_bBackWall;
    struct sMonster
    {
        eMonsters type;
        float x;
        float y;
        int w, h;
        int frame, framesCount;
        int angle;
        float speed; // wreckage related
        eImage img;
        int nCaptureMode;       // eMonsters::HAND: 0 - none, 1 - ball captured, 2 - ball released
        int nCapturedBallIndex; // eMonsters::HAND: captured ball index
    };
    std::vector<sMonster> m_wreckage;
    std::vector<sMonster> m_monsters;
};
