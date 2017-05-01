/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "ball.h"
#include "accessor.h"
#include "defines.h"
#include "exploision.h"
#include "game.h"
#include "random.h"
#include "videosystem/videosystem.h"

#include <cmath>

const int INIT_ANGLE = 250;
const float INIT_BALL_SPEED = 1.5f;

CBall::CBall()
{
    m_nBack = 0;
    m_vecBrickIndex.reserve(8);
    m_balls.reserve(10);
    m_fbs.reserve(200);
    RemoveAll();
}

CBall::~CBall()
{
}

int CBall::Move(bool bBackWall, SDL_Rect rcRacket, int nRacketType, int& nPaddleX)
{
    int nBallsLose = 0;
    m_bBackWall = bBackWall;

    Uint32 dwTime = SDL_GetTicks();

    static Uint32 dwBlueTime = 0;
    if (dwBlueTime + 1000 < dwTime)
    {
        dwBlueTime = dwTime;
        if (m_type != eBallType::WHITE && --m_nPrevTypeCount == 0)
        {
            m_type = eBallType::WHITE;
        }
    }
    static Uint32 dwTimeAddFb = 0;
    bool bAddFB = false;
    if (dwTimeAddFb + 20 < dwTime)
    {
        dwTimeAddFb = dwTime;
        bAddFB = true;
    }

    // move balls
    for (size_t nPos = 0, size = m_balls.size(); nPos < size;)
    {
        auto& ball = m_balls[nPos];

        ball.bAlreadyImpact = false;
        if (ball.bIsCaptured == false)
        {
            if (ball.fSpeed != 0)
            {
                if (bAddFB == true)
                {
                    AddFBs(nPos);
                }
                float fSpeed = ball.fSpeed * g_fSpeedCorrection;
                ball.x += fSpeed * g_fSin[getAngle(ball)];
                ball.y -= fSpeed * g_fCos[getAngle(ball)];
                //if(getAngle(nPos) > 0 && getAngle(nPos) < 180) {
                // ��� �������� �� ������������ � ������ ��������
                if (IsThisBallOverObject(nPos, rcRacket.x, rcRacket.y, 16, rcRacket.h) > 0)
                {
                    nPaddleX += 5;
                    PlaySound(11);
                    if (nRacketType == CGame::RT_MAGNET)
                    {
                        ball.fOldSpeed = ball.fSpeed;
                        ball.fSpeed = 0;
                        ball.nYoffset = int(ball.y - rcRacket.y);
                    }
                    ball.nAngle = (210 + (120.0 / rcRacket.h) * ((rcRacket.y + rcRacket.h) - (ball.y + getDiameter(ball) / 2)));
                    if (ball.nAngle < 210)
                    {
                        ball.nAngle = 210;
                    }
                    else if (ball.nAngle > 329)
                    {
                        ball.nAngle = 329;
                    }
                }
                //}

                if (ball.bAlreadyImpact == false)
                {
                    // calculate object size
                    SDL_Rect rc;
                    memset(&rc, 0, sizeof(SDL_Rect));
                    m_vecBrickIndex.clear();
                    if (m_type != eBallType::BLUE)
                    {
                        for (size_t nBrick = 0; nBrick < a::ark()->m_bricks.size(); nBrick++)
                        {
                            int nX = a::ark()->m_bricks[nBrick].x;
                            int nY = a::ark()->m_bricks[nBrick].y;
                            if (IsThisBallOverObject(nPos, nX, nY, BRICK_W, BRICK_H) > 0)
                            {
                                if (m_type == eBallType::RED)
                                {
                                    a::ark()->DoImpact(nBrick, true);
                                    a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
                                }
                                else
                                {
                                    m_vecBrickIndex.push_back(nBrick);
                                    if (rc.x < nX)
                                    {
                                        rc.x = nX;
                                    }
                                    if (rc.y < nY)
                                    {
                                        rc.y = nY;
                                    }
                                    if (rc.w < nX + BRICK_W)
                                    {
                                        rc.w = nX + BRICK_W;
                                    }
                                    if (rc.h < nY + BRICK_H)
                                    {
                                        rc.h = nY + BRICK_H;
                                    }
                                }
                            }
                        }
                    }
                    int nIsOver;
                    if (m_type == eBallType::WHITE && (nIsOver = IsThisBallOverObject(nPos, rc.x, rc.y, rc.w - rc.x, rc.h - rc.y)) > 0)
                    {
                        //printf("%d x %d, %d x %d\n", rc.x, rc.y, rc.w - rc.x, rc.h - rc.y);
                        ball.bAlreadyImpact = true;
                        if (m_type == eBallType::WHITE)
                        {
                            //printf("Brick change angle %d", getAngle(nPos));
                            const int anAngles[8] = { 180, 360, 360, 180, 540, 360, 360, 540 };
                            int nAngle180 = (getAngle(ball) + 180) % 360;
                            //printf("Pos: %d - Angle: %d", nIsOver, getAngle(nPos));
                            ball.nAngle = anAngles[nIsOver - 1] - getAngle(ball);
                            //printf(" -> %d%s\n", getAngle(nPos), getAngle(nPos) > 359 ? " !" : "");
                            IncrementBallSpeed(nPos);
                            //printf(" -> %d\n", getAngle(nPos));
                            m_nBack = 1;
                            while (IsThisBallOverObject(nPos, rc.x, rc.y, rc.w - rc.x, rc.h - rc.y) > 0)
                            {
                                ball.x += (g_fSin[nAngle180]);
                                ball.y -= (g_fCos[nAngle180]);
                            }
                            m_nBack = 0;
                            ball.x = int(ball.x);
                            ball.y = int(ball.y);
                            //printf("ball pos corrected (%.2d x %.2d)\n", (int)ball.x, (int)ball.y);

                            for (size_t nBrickIndex = 0; nBrickIndex < m_vecBrickIndex.size(); nBrickIndex++)
                            {
                                a::ark()->DoImpact(m_vecBrickIndex[nBrickIndex], false);
                            }
                        }
                    }
                }

                ImpactWithWallAngle(nPos);

                if (ball.x > SCREEN_WIDTH)
                {
                    PlaySound(14);
                    nBallsLose++;
                    m_balls[nPos] = m_balls[--size];
                    m_balls.pop_back();
                    continue;
                }
            }
            else
            {
                ball.y = rcRacket.y + ball.nYoffset;
                if ((ball.x + getDiameter(ball)) > rcRacket.x)
                {
                    ball.x -= 2;
                }
                if ((ball.x + getDiameter(ball)) < rcRacket.x)
                {
                    ball.x++;
                }
            }
        }
        nPos++;
    }
    return nBallsLose;
}

void CBall::Draw(int nPaddleType)
{
    SDL_Rect rc;

    Uint32 dwTime = SDL_GetTicks();

    static Uint32 dwFrame = 0;
    static int nFrame = 0;
    if (dwFrame + 50 < dwTime)
    {
        dwFrame = dwTime;
        nFrame++;
        nFrame %= 10;
    }
    //  static Uint32   dwTimeAddFb = 0;
    //  bool                bAddFB      = false;
    //  if(dwTimeAddFb + 20 < dwTime) {
    //      dwTimeAddFb = dwTime;
    //      bAddFB      = true;
    //  }

    bool bFirst = true;
    //  static int  nStep   = 0;
    for (const auto& ball : m_balls)
    {
        // if(bAddFB == true)  AddFBs(nPos);

        // show vector
        if (nPaddleType == CGame::RT_MAGNET && ball.bIsCaptured == false && ((bFirst == true && ball.fSpeed == 0) || ball.fSpeed != 0))
        {
            if (bFirst == true && ball.fSpeed == 0)
            {
                bFirst = false;
            }
            rc.y = 0;
            rc.w = 4;
            rc.h = 4;
            int nFrame2 = 0;
            float x = ball.x + (getDiameter(ball) - 4) / 2;
            float y = ball.y + (getDiameter(ball) - 4) / 2;
            /*          x   += g_fSin[getAngle(ball)] * nStep;
                        y   -= g_fCos[getAngle(ball)] * nStep;
                        nStep++;
                        nStep   %= 15;*/
            do
            {
                rc.x = nFrame2++ * 4;
                nFrame2 %= 5;
                render(x, y, eImage::Vector, &rc);
                x += g_fSin[getAngle(ball)] * 15;
                y -= g_fCos[getAngle(ball)] * 15;
            } while (x > WALL_X1 && x + 4 < (m_bBackWall == true ? WALL_X2 : SCREEN_WIDTH) && y > WALL_Y1 && y + 4 < WALL_Y2);
        }

        switch (ball.nDiameter)
        {
        case 0:
            rc.y = 0;
            break;
        case 1:
            rc.y = 12;
            break;
        case 2:
            rc.y = 32;
            break;
        case 3:
            rc.y = 56;
            break;
        case 4:
            rc.y = 88;
            break;
        }
        rc.x = nFrame * getDiameter(ball);
        rc.y += (int)m_type * 128;
        rc.w = rc.h = getDiameter(ball);
        render(ball.x, ball.y, eImage::Balls, &rc);
    }

    static Uint32 dwFBtime = 0;
    bool bFBupdate = false;
    if (dwFBtime + 50 < dwTime)
    {
        dwFBtime = dwTime;
        bFBupdate = true;
    }
    rc.w = 12;
    rc.h = 12;
    for (size_t i = 0, size = m_fbs.size(); i < size;)
    {
        auto& fb = m_fbs[i];

        rc.x = 12 * (int)m_type;
        rc.y = 12 * fb.nFrame;
        render(fb.nX, fb.nY, eImage::Fb, &rc);
        if (bFBupdate == true && ++fb.nFrame == 6)
        {
            m_fbs[i] = m_fbs[--size];
            m_fbs.pop_back();
            continue;
        }
        i++;
    }
}

void CBall::RemoveAll()
{
    m_balls.clear();
    m_fbs.clear();
    m_type = eBallType::WHITE;
    m_nPrevTypeCount = 0;
}

void CBall::AddBall(int x, int y)
{
    sBall ball;
    ball.x = (float)x;
    ball.y = (float)y;
    ball.nYoffset = y - (int)a::ark()->m_nRacketY;
    ball.nDiameter = 1;
    ball.fSpeed = 0;
    ball.fOldSpeed = -1;
    ball.nImpacts = 0;
    ball.nAngle = INIT_ANGLE;
    ball.dwTimeCanAddFb = 0;
    ball.bAlreadyImpact = false;
    ball.bIsCaptured = false;
    m_balls.push_back(ball);
}

void CBall::AddBall(int x, int y, int nAngle)
{
    sBall ball;
    ball.x = (float)x;
    ball.y = (float)y;
    ball.nYoffset = 0;
    ball.nDiameter = 1;
    ball.fSpeed = INIT_BALL_SPEED;
    ball.fOldSpeed = -1;
    ball.nImpacts = 0;
    ball.nAngle = nAngle;
    ball.dwTimeCanAddFb = 0;
    ball.bAlreadyImpact = false;
    ball.bIsCaptured = false;
    m_balls.push_back(ball);
}

int CBall::IsThisBallOverObject(int nPos, int nX, int nY, int nWidth, int nHeight)
{
    const auto& ball = m_balls[nPos];

    // calculate first angle
    int nObjAngle = int(57.3 * atan((float)nWidth / (float)nHeight));
    int nObjXc = nX + nWidth / 2; // center of object
    int nObjYc = nY + nHeight / 2;
    int nBallR = getDiameter(ball) / 2; // ball radius
    int nBallXc = int(ball.x + nBallR); // ball center coords
    int nBallYc = int(ball.y + nBallR);
    int nCatet1 = nObjXc - nBallXc;
    int nCatet2 = nObjYc - nBallYc;

    float fDist1 = sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
    int nAngle = int(57.3 * asin(nCatet2 / fDist1));
    if (nCatet1 > 0)
    {
        nAngle = 90 + nAngle;
    }
    else
    {
        nAngle = 270 - nAngle;
    }
    nAngle = ((360 + nAngle) % 360);

    int nIsOver;
    float fDist2;
    if (nAngle < nObjAngle)
    {
        fDist2 = (nHeight / 2) / g_fCos[nAngle];
        nIsOver = 1;
    }
    else if (nAngle < 90)
    {
        fDist2 = (nWidth / 2) / g_fCos[90 - nAngle];
        nIsOver = 2;
    }
    else if (nAngle < 180 - nObjAngle)
    {
        fDist2 = (nWidth / 2) / g_fCos[nAngle - 90];
        nIsOver = 3;
    }
    else if (nAngle < 180)
    {
        fDist2 = (nHeight / 2) / g_fCos[180 - nAngle];
        nIsOver = 4;
    }
    else if (nAngle < 180 + nObjAngle)
    {
        fDist2 = (nHeight / 2) / g_fCos[nAngle - 180];
        nIsOver = 5;
    }
    else if (nAngle < 270)
    {
        fDist2 = (nWidth / 2) / g_fCos[270 - nAngle];
        nIsOver = 6;
    }
    else if (nAngle < 360 - nObjAngle)
    {
        fDist2 = (nWidth / 2) / g_fCos[nAngle - 270];
        nIsOver = 7;
    }
    else /*if(nAngle < 360) */
    {
        fDist2 = (nHeight / 2) / g_fCos[360 - nAngle];
        nIsOver = 8;
    }

    if (fDist2 + nBallR + m_nBack > fDist1)
    {
        return nIsOver;
    }

    return 0;
}

bool CBall::StartBall(int nTop, int nBottom)
{
    for (size_t i = 0, size = m_balls.size(); i < size; i++)
    {
        auto& ball = m_balls[i];

        if (ball.fSpeed == 0)
        {
            if (ball.fOldSpeed != -1)
            {
                ball.fSpeed = ball.fOldSpeed;
            }
            else
            {
                ball.fSpeed = INIT_BALL_SPEED;
            }
            return true;
        }
    }

    return false;
}

void CBall::ImpactWithWallAngle(int nPos)
{
    auto& ball = m_balls[nPos];

    if (ball.x <= WALL_X1)
    {
        if (m_type == eBallType::RED)
        {
            a::expl()->AddExploision(WALL_X1 - 16, (int)ball.y + getDiameter(ball) / 2 - 16, 0);
        }
        //g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * ball.x)));

        const auto angle = getAngle(ball);
        if ((angle >= 180 && angle < 270) || (angle >= 270 && angle < 360))
        {
            ball.nAngle = 360 - angle;
        }
        IncrementBallSpeed(nPos);
    }
    else if (ball.x + getDiameter(ball) >= WALL_X2 && m_bBackWall == true)
    {
        if (m_type == eBallType::RED)
        {
            a::expl()->AddExploision(WALL_X2 - 16, (int)ball.y + getDiameter(ball) / 2 - 16, 0);
        }
        //g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * ball.x)));

        const auto angle = getAngle(ball);
        if ((angle >= 0 && angle < 90) || (angle >= 90 && angle < 180))
        {
            ball.nAngle = 360 - angle;
        }
        IncrementBallSpeed(nPos);
    }
    else if (ball.y <= WALL_Y1)
    {
        if (m_type == eBallType::RED)
        {
            a::expl()->AddExploision((int)ball.x + getDiameter(ball) / 2 - 16, WALL_Y1 - 16, 0);
        }
        //g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * ball.x)));

        const auto angle = getAngle(ball);
        if (angle >= 0 && angle < 90)
        {
            ball.nAngle = 180 - angle;
        }
        else if (angle >= 270 && angle < 360)
        {
            ball.nAngle = 540 - angle;
        }
        IncrementBallSpeed(nPos);
    }
    else if (ball.y + getDiameter(ball) >= WALL_Y2)
    {
        if (m_type == eBallType::RED)
        {
            a::expl()->AddExploision((int)ball.x + getDiameter(ball) / 2 - 16, WALL_Y2 - 16, 0);
        }
        //g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * ball.x)));

        const auto angle = getAngle(ball);
        if (angle >= 90 && angle < 180)
        {
            ball.nAngle = 180 - angle;
        }
        else if (angle >= 180 && angle < 270)
        {
            ball.nAngle = 540 - angle;
        }
        IncrementBallSpeed(nPos);
    }

    ball.x = std::max<int>(ball.x, WALL_X1);
    ball.x = std::min<int>(ball.x, (m_bBackWall == true ? WALL_X2 - getDiameter(ball) : SCREEN_WIDTH + 1));
    ball.y = std::max<int>(ball.y, WALL_Y1);
    ball.y = std::min<int>(ball.y, (WALL_Y2 - getDiameter(ball)));
}

void CBall::ChangeBallSize(int nDelta)
{
    for (auto& ball : m_balls)
    {
        ball.nDiameter += nDelta;
        ball.nDiameter = std::min<int>(ball.nDiameter, 4);
        ball.nDiameter = std::max<int>(ball.nDiameter, 0);
    }
}

sBallDescription CBall::getDescription(size_t idx) const
{
    auto& ball = m_balls[idx];

    sBallDescription desc;

    desc.type = m_type;
    desc.x = ball.x;
    desc.y = ball.y;
    desc.diameter = getDiameter(ball);
    desc.bIsCaptured = ball.bIsCaptured;

    return desc;
}

void CBall::SetAllBallsToBlue()
{
    m_type = eBallType::BLUE;
    m_nPrevTypeCount = 8;
}

void CBall::SetAllBallsToFire()
{
    m_type = eBallType::RED;
    m_nPrevTypeCount = 8;
}

void CBall::SplitBalls()
{
    auto tempList = m_balls;

    m_balls.reserve(m_balls.size() + tempList.size());
    for (auto& ball : tempList)
    {
        ball.nAngle = (int)(ball.nAngle + 45) % 360;
        ball.bIsCaptured = false;
        m_balls.push_back(ball);
    }
}

void CBall::ChangeBallAngle(int nPos, bool bRotate)
{
    auto& ball = m_balls[nPos];

    if (ball.fSpeed == 0.0)
    {
        return;
    }
    if (bRotate == false)
    {
        ball.nAngle += 360 - 90 + a::rnd().Get(90);
    }
    else
    {
        ball.nAngle += 45; //180;
    }

    //ball.bAlreadyImpact = true;
}

void CBall::ChangeAllBallsSpeed(bool bIncrease)
{
    for (auto& ball : m_balls)
    {
        if (ball.fSpeed != 0.0f)
        {
            if (bIncrease == true)
            {
                if (ball.fSpeed < INIT_BALL_SPEED + 5.0f)
                {
                    ball.fSpeed += 0.5f;
                }
            }
            else
            {
                if (ball.fSpeed > INIT_BALL_SPEED)
                {
                    ball.fSpeed -= 0.5f;
                }
            }
        }
    }
}

bool CBall::RemoveOne(int& nX, int& nY)
{
    if (m_balls.size() > 0)
    {
        PlaySound(14);
        const auto& ball = m_balls.back();
        nX = ball.x;
        nY = ball.y;
        m_balls.pop_back();
        return true;
    }

    return false;
}

void CBall::IncrementBallSpeed(int nPos)
{
    auto& ball = m_balls[nPos];

    ball.nImpacts++;
    if (ball.nImpacts > 20)
    {
        ball.nImpacts = 0;
        if (ball.fSpeed < INIT_BALL_SPEED + 5.0)
        {
            ball.fSpeed += .1;
        }
    }
    ball.nAngle += 360;
}

void CBall::AddFBs(int nPos)
{
    auto& ball = m_balls[nPos];

    int nDiameter = getDiameter(ball);
    for (int i = 0; i < nDiameter / 6; i++)
    {
        int nR = nDiameter / 2 - 4;
        int nXb = ball.x + nDiameter / 2;
        int nYb = ball.y + nDiameter / 2;

        sFb fb;
        fb.type = m_type;
        fb.nX = nXb + nR - a::rnd().Get(nR * 2) - 6;
        fb.nY = nYb + nR - a::rnd().Get(nR * 2) - 6;
        fb.nFrame = 0;
        m_fbs.push_back(fb);
    }
}

void CBall::ChangeAngle(int nPos, bool bIncrease)
{
    auto& ball = m_balls[nPos];

    if (ball.fSpeed != 0.0)
    {
        float fAngle = 0.5 * g_fSpeedCorrection;
        if (bIncrease == true)
        {
            ball.nAngle += fAngle;
        }
        else
        {
            ball.nAngle += (360 - fAngle);
        }
        //if(ball.nAngle > 359) ball.nAngle -= 359;
    }
}

int CBall::getAngle(const sBall& ball) const
{
    return (int)(ball.nAngle + 360) % 360;
}

int CBall::getAngle(size_t idx) const
{
    return getAngle(m_balls[idx]);
}

void CBall::SlowDown()
{
    for (auto& ball : m_balls)
    {
        if (ball.fSpeed != 0.0)
        {
            ball.fSpeed = INIT_BALL_SPEED;
        }
    }
}

int CBall::getDiameter(const sBall& ball) const
{
    auto idx = ball.nDiameter;
    const int anDiameters[] = { 12, 20, 24, 32, 40 };
    if (idx >= 0 && idx < 5)
    {
        return anDiameters[idx];
    }
    return 0;
}

eBallType CBall::GetType()
{
    return m_type;
}

size_t CBall::GetTypeCount()
{
    return m_nPrevTypeCount;
}

/*!
   \fn CBall::BallCaptured(iny nIndex, bool bIsCaptured)
   bIsCaptured:
   true - monster HAND capture ball, move that ball with monster
    false   - monster release ball - start it some direction
 */
void CBall::BallCaptured(int nIndex, bool bIsCaptured)
{
    if (nIndex >= 0 && nIndex < (int)m_balls.size())
    {
        auto& ball = m_balls[nIndex];

        if (bIsCaptured == false && ball.bIsCaptured == true)
        {
            ball.fSpeed += 0.5;
            ball.nAngle = 30 + a::rnd().Get(120);
        }
        ball.bIsCaptured = bIsCaptured;
    }
}

void CBall::SetCapturedBallPos(int nIndex, float fX, float fY)
{
    if (nIndex >= 0 && nIndex < (int)m_balls.size())
    {
        auto& ball = m_balls[nIndex];

        if (ball.bIsCaptured == true)
        {
            int nRadius = getDiameter(ball) / 2;
            ball.x = fX + 26 - nRadius;
            ball.y = fY + 33 - nRadius;
        }
    }
}
