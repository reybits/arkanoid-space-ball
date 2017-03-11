/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"

#include <SDL.h>
#include <vector>

class CBall final
{
public:
    CBall();
    ~CBall();

    int IsThisBallOverObject(int nPos, int nX, int nY, int nWidth, int nHeight);
    bool RemoveOne(int& nX, int& nY);
    void ChangeAllBallsSpeed(bool bIncrease);
    void ChangeBallAngle(int nPos, bool bRotate);
    void SetAllBallsToBlue();
    void SetAllBallsToFire();
    void SplitBalls();

    sBallDescription getDescription(size_t idx) const;
    void ChangeBallSize(int nDelta);
    size_t GetBallsCount() const { return m_balls.size(); }
    bool StartBall(int nTop, int nBottom);
    int Move(bool bBackWall, SDL_Rect rcRacket, int nRacketType, int& nPaddleX);
    void AddBall(int x, int y);
    void AddBall(int x, int y, int nAngle);
    void RemoveAll();
    void Draw(int nPaddleType);
    void ChangeAngle(int nPos, bool bIncrease);
    void SlowDown();
    eBallType GetType();
    size_t GetTypeCount();
    void BallCaptured(int nIndex, bool bIsCaptured);
    void SetCapturedBallPos(int nIndex, float fX, float fY);

    int getAngle(size_t idx) const;

private:
    void AddFBs(int nPos);
    void IncrementBallSpeed(int nPos);
    void ImpactWithWallAngle(int nPos);

    struct sBall;
    int getAngle(const sBall& ball) const;
    int getDiameter(const sBall& ball) const;

private:
    bool m_bBackWall;
    eBallType m_type;
    int m_nPrevTypeCount;
    struct sBall
    {
        float x, y;
        int nYoffset;
        int nDiameter;
        int nImpacts;
        float nAngle;
        float fSpeed, fOldSpeed;
        bool bAlreadyImpact;
        Uint32 dwTimeCanAddFb;
        bool bIsCaptured; // is ball captured by monster HAND?
    };
    std::vector<sBall> m_balls;
    struct sFb
    {
        eBallType type;
        int nX, nY;
        int nFrame;
    };
    std::vector<sFb> m_fbs;
    std::vector<int> m_vecBrickIndex;
    int m_nBack;
};
