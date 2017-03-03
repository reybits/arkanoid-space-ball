/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "arkanoidsb.h"
#include "accessor.h"
#include "ball.h"
#include "defines.h"
#include "exploision.h"
#include "game.h"
#include "monster.h"
#include "random.h"
#include "tutorialdlg.h"

#include <cmath>

CMonster::CMonster()
{
    m_nSndPatrol = -1;
    m_vecMonster.reserve(20);
    m_vecMonster2.reserve(10);
    RemoveAll();
}

CMonster::~CMonster()
{
}

void CMonster::RemoveAll()
{
    StopSound(m_nSndPatrol);
    m_vecMonster.clear();
    m_vecMonster2.clear();
}

void CMonster::Draw()
{
    static Uint32 curTime = 0;
    bool updated = false;
    if (curTime + 30 < SDL_GetTicks())
    {
        curTime = SDL_GetTicks();
        updated = true;
    }

    SDL_Rect rc;
    rc.y = 0;
    for (auto& v : m_vecMonster)
    {
        if (updated == true && v.nCaptureMode == 0)
        {
            v.nFrame++;
            v.nFrame %= v.nFramesCount;
        }
        rc.x = v.nFrame * v.w;
        rc.w = v.w;
        rc.h = v.h;
        Blit(v.x, v.y, v.img, &rc);
    }
}

void CMonster::Draw2()
{
    SDL_Rect rc2;
    SetRect(&rc2, WALL_X1, WALL_Y1, SCREEN_WIDTH - WALL_X1, WALL_Y2 - WALL_Y1);
    SDL_SetClipRect(g_psurfScreen, &rc2);

    static Uint32 curTime = 0;
    bool updated = false;
    if (curTime + 50 < SDL_GetTicks())
    {
        curTime = SDL_GetTicks();
        updated = true;
    }

    SDL_Rect rc;
    rc.y = 0;
    for (auto& v : m_vecMonster2)
    {
        if (updated == true)
        {
            v.nFrame++;
            v.nFrame %= v.nFramesCount;
        }
        rc.x = v.nFrame * v.w;
        rc.w = v.w;
        rc.h = v.h;
        Blit(v.x, v.y, v.img, &rc);
    }

    SDL_SetClipRect(g_psurfScreen, 0);
}

void CMonster::AddMonster(int x, int y, int nType)
{
    if (nType >= 0 && nType < MONST_END)
    {
        sMonster monster;
        monster.nType           = nType;
        monster.x               = (float)x;
        monster.y               = (float)y;
        monster.nFrame          = 0;
        monster.nCaptureMode    = 0;
        //monster.nCapturedBallIndex    = -1;
        monster.nAngle          = a::rnd().Get(360);
        switch (nType)
        {
        case MONST_PATROL:
            if (m_nSndPatrol == -1 && a::ark()->m_nGameState == CGame::GS_GAME_ACTIVE)
            {
                m_nSndPatrol  = PlaySound(2);
            }
            monster.nFramesCount    = 10;
            monster.img        = m_pMonstPatrol;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_COPTER:
            monster.nFramesCount    = 5;
            monster.img        = m_pMonstCopter;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_EYE:
            monster.nFramesCount    = 30;
            monster.img        = m_pMonstEye;
            monster.w               = 32;
            monster.h               = 32;
            break;

        case MONST_BLACKHOLE:
            monster.nFramesCount    = 5;
            monster.img        = m_pMonstBlackHole;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_FIGHTER:
            monster.nFramesCount    = 10;
            monster.img        = m_pMonstFighter;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_HAND:
            monster.nFramesCount    = 30;
            monster.img        = m_pMonstHand;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_TURBINE:
            monster.nFramesCount    = 20;
            monster.img        = m_pMonstTurbine;
            monster.w               = 32;
            monster.h               = 32;
            break;

        case MONST_UFO:
            monster.nFramesCount    = 10;
            monster.img        = m_pMonstUfo;
            monster.w               = 64;
            monster.h               = 48;
            break;

        case MONST_STONE1:
            monster.nFramesCount    = 30;
            monster.img        = m_pMonstStone1;
            monster.w               = 32;
            monster.h               = 24;
            break;

        case MONST_STONE2:
            monster.nFramesCount    = 30;
            monster.img        = m_pMonstStone2;
            monster.w               = 32;
            monster.h               = 24;
            break;
        }

        m_vecMonster.push_back(monster);
        a::tutDlg()->AddDialog(int(monster.x + monster.w / 2), int(monster.y + monster.h / 2), 1, nType);
    }
}

void CMonster::AddMonster2()
{
    sMonster    monster;
    monster.nType   = a::rnd().Get(2);
    switch (monster.nType)
    {
    case M_WRECKAGE1:
        monster.nFramesCount    = 10;
        monster.img        = m_pMonstWreckage1;
        monster.w               = 64;
        monster.h               = 48;
        break;
    case M_WRECKAGE2:
        monster.nFramesCount    = 30;
        monster.img        = m_pMonstWreckage2;
        monster.w               = 64;
        monster.h               = 48;
        break;
    }
    monster.fSpeed  = 0.5 + a::rnd().Get(10) / 5.0;
    monster.x       = -monster.w;
    monster.y       = WALL_Y1 + a::rnd().Get(WALL_Y2 - WALL_Y1);
    monster.nFrame  = a::rnd().Get(monster.nFramesCount);
    monster.nAngle  = 70 + a::rnd().Get(40);
    m_vecMonster2.push_back(monster);
    //printf("added (total: %d)\n", m_vecMonster.size());
}

void CMonster::Move(bool bBackWall, int nPaddleY, int nPaddleHeight)
{
    SDL_Rect rc;
    m_bBackWall = bBackWall;
    float fSpeed = g_fSpeedCorrection * 0.7;

    for (size_t i = 0, size = m_vecMonster.size(); i < size; i++)
    {
        auto& v = m_vecMonster[i];

        if (v.nType == MONST_PATROL)
        {
            v.x += (2 * g_fSpeedCorrection);
            if (v.x > SCREEN_WIDTH)
            {
                RemoveByPos(i--);
                size--;
            }
        }
        else if (v.nType == MONST_COPTER)
        {
            int nAngle = 0;
            int nPos = 0;
            float minDist = std::numeric_limits<float>::max();
            bool bFindBall = false;
            while (a::ball()->GetPositionAndDiameter(rc, nPos) == true)
            {
                const float c1 = (rc.x + rc.w * 0.5f) - (v.x + 32);
                const float c2 = (rc.y + rc.w * 0.5f) - (v.y + 24);
                const float dist = sqrtf(c1 * c1 + c2 * c2);
                if (dist < minDist && rc.h != CBall::TYPE_BLUE)
                {
                    bFindBall = true;
                    minDist = dist;
                    nAngle  = (int)(57.3 * asinf(c2 / dist));
                    if (c1 > 0)
                    {
                        nAngle    = (90 + nAngle) % 360;
                    }
                    else
                    {
                        nAngle    = (270 - nAngle) % 360;
                    }
                }
            }
            if (bFindBall == true)
            {
                for (int nIterations = 0; nIterations < 2; nIterations++)
                {
                    if (v.nAngle < nAngle)
                    {
                        if (abs(v.nAngle - nAngle) >= 180)
                        {
                            v.nAngle--;
                        }
                        else
                        {
                            v.nAngle++;
                        }
                    }
                    else
                    {
                        if (abs(v.nAngle - nAngle) < 180)
                        {
                            v.nAngle--;
                        }
                        else
                        {
                            v.nAngle++;
                        }
                    }
                    v.nAngle  += 360;
                    v.nAngle  %= 360;
                    if (nAngle == v.nAngle)
                    {
                        break;
                    }
                }
                //v.nAngle    %= 360;
                v.x += fSpeed * g_fSin[v.nAngle];
                v.y -= fSpeed * g_fCos[v.nAngle];
            }
        }
        else if (v.nType == MONST_HAND)
        {
            if (v.nCaptureMode == 0)
            {
                int nAngle = 0;
                int nPos = 0;
                float minDist = std::numeric_limits<float>::max();
                bool bFindBall = false;
                while (a::ball()->GetPositionAndDiameter(rc, nPos) == true)
                {
                    const float c1 = (rc.x + rc.w * 0.5f) - (v.x + 32);
                    const float c2 = (rc.y + rc.w * 0.5f) - (v.y + 24);
                    const float dist = sqrtf(c1 * c1 + c2 * c2);
                    if (dist < 3)
                    {
                        v.nCaptureMode       = 1;
                        v.nCapturedBallIndex = nPos - 1;
                        v.nFrame             = 4;
                        v.nAngle             = 240 + a::rnd().Get(60);
                        a::ball()->BallCaptured(v.nCapturedBallIndex, true);
                    }
                    else if (dist < minDist)
                    {
                        bFindBall = true;
                        minDist = dist;
                        nAngle = (int)(57.3 * asinf(c2 / dist));
                        if (c1 > 0)
                        {
                            nAngle = (90 + nAngle) % 360;
                        }
                        else
                        {
                            nAngle = (270 - nAngle) % 360;
                        }
                    }
                }

                if (bFindBall == true)
                {
                    for (int nIterations = 0; nIterations < 1 + g_fSpeedCorrection * 8; nIterations++)
                    {
                        if (v.nAngle < nAngle)
                        {
                            if (abs(v.nAngle - nAngle) >= 180)
                            {
                                v.nAngle--;
                            }
                            else
                            {
                                v.nAngle++;
                            }
                        }
                        else
                        {
                            if (abs(v.nAngle - nAngle) < 180)
                            {
                                v.nAngle--;
                            }
                            else
                            {
                                v.nAngle++;
                            }
                        }
                        v.nAngle += 360;
                        v.nAngle %= 360;
                        if (nAngle == v.nAngle)
                        {
                            break;
                        }
                    }
                    const float delta = g_fSpeedCorrection * 2.5f;
                    v.x += delta * g_fSin[v.nAngle];
                    v.y -= delta * g_fCos[v.nAngle];
                }
            }
            else
            {
                v.x += fSpeed * g_fSin[v.nAngle];
                v.y -= fSpeed * g_fCos[v.nAngle];
                ChangeMonsterAngle(i);
                if (v.nCaptureMode == 1)
                {
                    a::ball()->SetCapturedBallPos(v.nCapturedBallIndex, v.x, v.y);
                    if (abs((int)v.x - (RACKET_X - 140)) < 5)
                    {
                        a::ball()->BallCaptured(v.nCapturedBallIndex, false);
                        v.nCaptureMode = 2;
                        v.nAngle       = 90;
                        v.nFrame       = 0;
                    }
                }
                else
                {
                    if (v.x > SCREEN_WIDTH)
                    {
                        RemoveByPos(i--);
                        size--;
                    }
                }
            }
        }
        else if (v.nType == MONST_EYE)
        {
            if (v.x < RACKET_X - 60)
            {
                v.x += fSpeed;
            }
            if (v.y + v.h / 2 - 5 + a::rnd().Get(10) < nPaddleY + nPaddleHeight / 2)
            {
                v.y += fSpeed;
            }
            else
            {
                v.y -= fSpeed;
            }
        }
        else
        {
            v.x += (fSpeed * g_fSin[v.nAngle]);
            v.y -= (fSpeed * g_fCos[v.nAngle]);
            if (v.nType == MONST_STONE1 || v.nType == MONST_STONE2)
            {
                if (v.x < WALL_X1 || v.x > (m_bBackWall == true ? WALL_X2 - v.w : SCREEN_WIDTH) ||
                    v.y < WALL_Y1 || v.y + v.h > WALL_Y2)
                {
                    if (v.x < SCREEN_WIDTH)
                    {
                        a::expl()->AddExploision((int)v.x + (v.w - 45) / 2, (int)v.y + (v.h - 41) / 2, 0);
                    }
                    RemoveByPos(i--);
                    size--;
                }
            }
            else
            {
                ChangeMonsterAngle(i);
            }
        }
    }

    for (size_t i = 0, size = m_vecMonster2.size(); i < size;)
    {
        auto& v = m_vecMonster2[i];

        const float delta = g_fSpeedCorrection * v.fSpeed;
        v.x += delta * g_fSin[v.nAngle];
        v.y -= delta * g_fCos[v.nAngle];
        if (v.x > SCREEN_WIDTH)
        {
            v = m_vecMonster2[--size];
            m_vecMonster2.pop_back();
            continue;
        }
        i++;
    }
}

bool CMonster::IsAcross(int nX, int nY, int nWidth, int nHeight, bool bRemoveMonster)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_vecMonster.size(); i < size; i++)
    {
        const auto& v = m_vecMonster[i];

        const float r1 = (nWidth + nHeight) / 4.0f;
        const float r2 = (v.w - (v.nType == MONST_HAND ? 20 : 0) + v.h) / 4.0f;
        const float c1 = ((v.x + v.w * 0.5f) - (nX + nWidth * 0.5f));
        const float c2 = ((v.y + v.h * 0.5f) - (nY + nHeight * 0.5f));
        if (sqrtf(c1 * c1 + c2 * c2) <= r1 + r2)
        {
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            if (bRemoveMonster == true)
            {
                a::expl()->AddExploision(v.x + (v.w - 45) / 2, v.y + (v.h - 41) / 2, 0);
                RemoveByPos(i--);
                size--;
            }
        }
    }

    return bIsAcross;
}

bool CMonster::IsAcrossPaddle(int nX, int nY, int nWidth, int nHeight)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_vecMonster.size(); i < size; i++)
    {
        const auto& v = m_vecMonster[i];

        const float r1 = (nWidth + nHeight) / 4.0f;
        const float r2 = (v.w - (v.nType == MONST_HAND ? 20 : 0) + v.h) / 4.0f;
        const float c1 = (v.x + v.w * 0.5f) - (nX + nWidth * 0.5f);
        const float c2 = (v.y + v.h * 0.5f) - (nY + nHeight * 0.5f);
        if (sqrtf(c1 * c1 + c2 * c2) <= r1 + r2)
        {
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            if (v.nType == MONST_STONE2)
            {
                a::ark()->m_bCanMovePaddle = false;
                a::ark()->m_nCanMovePaddleCount = 3;
            }
            else if (v.nType == MONST_PATROL || v.nType == MONST_STONE1)
            {
                for (int a = 0; a < 6; a++)
                {
                    a::expl()->AddExploision(nX - (45 - 12) / 2 + (10 - a::rnd().Get(20)), nY - (41 - nHeight) / 2 + (nHeight / 2 - a::rnd().Get(nHeight)), 0);
                }
                a::ark()->AddGetReeadyInfo(v.nType == MONST_PATROL ? "Kamikaze impact paddle" : "Meteor impact paddle");
                a::ark()->RemoveOneLives();
            }
            a::expl()->AddExploision(v.x + (v.w - 45) / 2, v.y + (v.h - 41) / 2, 0);
            RemoveByPos(i--);
            size--;
        }
    }

    return bIsAcross;
}

bool CMonster::IsAcrossPaddle2(int nX, int nY, int nWidth, int nHeight)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_vecMonster2.size(); i < size;)
    {
        auto& v = m_vecMonster2[i];

        const float r1 = (nWidth + nHeight) / 4;
        const float r2 = (v.w + v.h) / 4;
        const float c1 = (v.x + v.w * 0.5f) - (nX + nWidth * 0.5f);
        const float c2 = (v.y + v.h * 0.5f) - (nY + nHeight * 0.5f);
        if (sqrtf(c1 * c1 + c2 * c2) <= r1 + r2)
        {
            bIsAcross = true;
            for (int a = 0; a < 6; a++)
            {
                a::expl()->AddExploision(nX - (45 - 12) / 2 + (10 - a::rnd().Get(20)), nY - (41 - nHeight) / 2 + (nHeight / 2 - a::rnd().Get(nHeight)), 0);
            }
            a::expl()->AddExploision(v.x + (v.w - 45) / 2, v.y + (v.h - 41) / 2, 0);
            v = m_vecMonster2[--size];
            m_vecMonster2.pop_back();
            continue;
        }
        i++;
    }

    return bIsAcross;
}

void CMonster::ChangeMonsterAngle(size_t idx)
{
    auto& v = m_vecMonster[idx];

    if (v.nType == MONST_HAND && v.nCaptureMode == 2)
    {
        return;
    }
    v.nAngle %= 360;
    if (v.x < WALL_X1)
    {
        if (v.nAngle >= 180 && v.nAngle < 270)
        {
            v.nAngle = 180 - (v.nAngle - 180);
        }
        else if (v.nAngle >= 270 && v.nAngle < 360)
        {
            v.nAngle = 360 - v.nAngle;
        }
    }
    else if (v.x + v.w > WALL_X2)
    {
        if (v.nAngle >= 0 && v.nAngle < 90)
        {
            v.nAngle = 360 - v.nAngle;
        }
        else if (v.nAngle >= 90 && v.nAngle < 180)
        {
            v.nAngle = 180 + 180 - v.nAngle;
        }
    }
    else if (v.y < WALL_Y1)
    {
        if (v.nAngle >= 0 && v.nAngle < 90)
        {
            v.nAngle = 180 - v.nAngle;
        }
        else if (v.nAngle >= 270 && v.nAngle < 360)
        {
            v.nAngle = 180 + (360 - v.nAngle);
        }
    }
    else if (v.y + v.h > WALL_Y2)
    {
        if (v.nAngle >= 90 && v.nAngle < 180)
        {
            v.nAngle = 180 - v.nAngle;
        }
        else if (v.nAngle >= 180 && v.nAngle < 270)
        {
            v.nAngle = 360 - (v.nAngle - 180);
        }
    }

    v.x = std::max<int>(v.x, WALL_X1);
    v.x = std::min<int>(v.x, WALL_X2 - v.w);
    v.y = std::max<int>(v.y, WALL_Y1);
    v.y = std::min<int>(v.y, WALL_Y2 - v.h);
}


bool CMonster::IsAcrossBall(SDL_Rect rc, bool bRemoveMonster)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_vecMonster.size(); i < size; i++)
    {
        const auto& v = m_vecMonster[i];

        if (v.nType != MONST_HAND)
        {
            const float nRadius = rc.w * 0.5f;
            const float c1 = rc.x + nRadius - (v.x + v.w * 0.5f);
            const float c2 = rc.y + nRadius - (v.y + v.h * 0.5f);
            if (sqrtf(c1 * c1 + c2 * c2) < nRadius + ((v.w + v.h) / 4))
            {
                a::ark()->AddScore(25);
                bIsAcross = true;
                if (bRemoveMonster == true)
                {
                    a::ark()->m_nGetReadyMonsters++;
                    a::expl()->AddExploision(v.x + (v.w - 45) / 2, v.y + (v.h - 41) / 2, 0);
                    RemoveByPos(i--);
                    size--;
                }
            }
        }
    }

    return bIsAcross;
}


bool CMonster::IsAcrossBall2(SDL_Rect rc)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_vecMonster2.size(); i < size;)
    {
        auto& v = m_vecMonster2[i];

        const float nRadius = rc.w * 0.5f;
        const float c1 = rc.x + nRadius - (v.x + v.w * 0.5f);
        const float c2 = rc.y + nRadius - (v.y + v.h * 0.5f);
        if (sqrtf(c1 * c1 + c2 * c2) < nRadius + ((v.w + v.h) / 4))
        {
            a::ark()->AddScore(100);
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            a::expl()->AddExploision((int)v.x + (v.w - 45) / 2, (int)v.y + (v.h - 41) / 2, 0);
            v = m_vecMonster2[--size];
            m_vecMonster2.pop_back();
            continue;
        }
        i++;
    }

    return bIsAcross;
}

void CMonster::RemoveByPos(size_t idx)
{
    auto& v = m_vecMonster[idx];

    if (v.nType == MONST_PATROL)
    {
        StopSound(m_nSndPatrol);
    }
    else if (v.nType == MONST_HAND)
    {
        a::ball()->BallCaptured(v.nCapturedBallIndex, false);
    }
    v = m_vecMonster.back();
    m_vecMonster.pop_back();
}
