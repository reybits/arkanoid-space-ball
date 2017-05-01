/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "monster.h"
#include "accessor.h"
#include "arkanoidsb.h"
#include "ball.h"
#include "defines.h"
#include "exploision.h"
#include "game.h"
#include "random.h"
#include "tutorialdlg.h"
#include "videosystem/videosystem.h"

#include <cmath>
#include <cstdlib>

CMonster::CMonster()
{
    m_nSndPatrol = -1;
    m_monsters.reserve(20);
    m_wreckage.reserve(10);
    RemoveAll();
}

CMonster::~CMonster()
{
}

void CMonster::RemoveAll()
{
    StopSound(m_nSndPatrol);
    m_monsters.clear();
    m_wreckage.clear();
}

void CMonster::DrawMonsters()
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
    for (auto& v : m_monsters)
    {
        if (updated == true && v.nCaptureMode == 0)
        {
            v.frame++;
            v.frame %= v.framesCount;
        }
        rc.x = v.frame * v.w;
        rc.w = v.w;
        rc.h = v.h;
        render(v.x, v.y, v.img, &rc);
    }
}

void CMonster::DrawWreckage()
{
    SDL_Rect rc2;
    SetRect(&rc2, WALL_X1, WALL_Y1, SCREEN_WIDTH - WALL_X1, WALL_Y2 - WALL_Y1);
    scissor(&rc2);

    static Uint32 curTime = 0;
    bool updated = false;
    if (curTime + 50 < SDL_GetTicks())
    {
        curTime = SDL_GetTicks();
        updated = true;
    }

    SDL_Rect rc;
    rc.y = 0;
    for (auto& v : m_wreckage)
    {
        if (updated == true)
        {
            v.frame++;
            v.frame %= v.framesCount;
        }
        rc.x = v.frame * v.w;
        rc.w = v.w;
        rc.h = v.h;
        render(v.x, v.y, v.img, &rc);
    }

    scissor();
}

void CMonster::AddMonster(int x, int y, eMonsters type)
{
    struct sDescription
    {
        int framesCount;
        eImage img;
        int w;
        int h;
    };
    const sDescription Descriptions[(unsigned)eMonsters::Count] = {
        { 10, eImage::MonsterPatrol, 64, 48 },
        { 5, eImage::MonsterCopter, 64, 48 },
        { 30, eImage::MonsterEye, 32, 32 },
        { 5, eImage::MonsterBlackhole, 64, 48 },
        { 10, eImage::MonsterFighter, 64, 48 },
        { 30, eImage::MonsterHand, 64, 48 },
        { 20, eImage::MonsterTurbine, 32, 32 },
        { 10, eImage::MonsterUfo, 64, 48 },
        { 30, eImage::MonsterStone1, 32, 24 },
        { 30, eImage::MonsterStone2, 32, 24 },

        { 10, eImage::MonsterWreckage1, 64, 48 },
        { 30, eImage::MonsterWreckage2, 64, 48 },
    };

    sMonster monster;

    const auto& m = Descriptions[(unsigned)type];

    monster.framesCount = m.framesCount;
    monster.img = m.img;
    monster.w = m.w;
    monster.h = m.h;

    if (type < eMonsters::WRECKAGE1)
    {
        monster.type = type;
        monster.x = x;
        monster.y = y;
        monster.frame = 0;
        monster.nCaptureMode = 0;
        //monster.nCapturedBallIndex    = -1;
        monster.angle = a::rnd().Get(360);

        if (type == eMonsters::PATROL)
        {
            if (m_nSndPatrol == -1 && a::ark()->m_nGameState == CGame::GS_GAME_ACTIVE)
            {
                m_nSndPatrol = PlaySound(2);
            }
        }

        m_monsters.push_back(monster);

        a::tutDlg()->AddDialog(monster.x + monster.w / 2, monster.y + monster.h / 2, 1, (int)type);
    }
    else
    {
        monster.type = (eMonsters)((unsigned)eMonsters::WRECKAGE1 + a::rnd().Get(2));
        monster.speed = 0.5 + a::rnd().Get(10) / 5.0;
        monster.x = -monster.w;
        monster.y = WALL_Y1 + a::rnd().Get(WALL_Y2 - WALL_Y1);
        monster.frame = a::rnd().Get(monster.framesCount);
        monster.angle = 70 + a::rnd().Get(40);
        m_wreckage.push_back(monster);
        //printf("added (total: %d)\n", m_monsters.size());
    }
}

void CMonster::Move(bool bBackWall, int nPaddleY, int nPaddleHeight)
{
    SDL_Rect rc;
    m_bBackWall = bBackWall;
    float speed = g_fSpeedCorrection * 0.7;

    for (size_t i = 0, size = m_monsters.size(); i < size; i++)
    {
        auto& v = m_monsters[i];

        if (v.type == eMonsters::PATROL)
        {
            v.x += (2 * g_fSpeedCorrection);
            if (v.x > SCREEN_WIDTH)
            {
                RemoveByPos(i--);
                size--;
            }
        }
        else if (v.type == eMonsters::COPTER)
        {
            int angle = 0;
            float minDist = std::numeric_limits<float>::max();
            bool ballLocated = false;
            auto balls = a::ball();
            for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
            {
                const auto& desc = balls->getDescription(idx);
                if (desc.bIsCaptured == false && desc.type != eBallType::BLUE)
                {
                    const float c1 = (rc.x + rc.w * 0.5f) - (v.x + 32);
                    const float c2 = (rc.y + rc.w * 0.5f) - (v.y + 24);
                    const float dist = sqrtf(c1 * c1 + c2 * c2);
                    if (dist < minDist)
                    {
                        ballLocated = true;
                        minDist = dist;
                        angle = (int)(57.3 * asinf(c2 / dist));
                        if (c1 > 0)
                        {
                            angle = (90 + angle) % 360;
                        }
                        else
                        {
                            angle = (270 - angle) % 360;
                        }
                    }
                }
            }

            if (ballLocated == true)
            {
                for (int nIterations = 0; nIterations < 2; nIterations++)
                {
                    if (v.angle < angle)
                    {
                        if (abs(v.angle - angle) >= 180)
                        {
                            v.angle--;
                        }
                        else
                        {
                            v.angle++;
                        }
                    }
                    else
                    {
                        if (abs(v.angle - angle) < 180)
                        {
                            v.angle--;
                        }
                        else
                        {
                            v.angle++;
                        }
                    }
                    v.angle += 360;
                    v.angle %= 360;
                    if (angle == v.angle)
                    {
                        break;
                    }
                }
                //v.angle %= 360;
                v.x += speed * g_fSin[v.angle];
                v.y -= speed * g_fCos[v.angle];
            }
        }
        else if (v.type == eMonsters::HAND)
        {
            if (v.nCaptureMode == 0)
            {
                int angle = 0;
                float minDist = std::numeric_limits<float>::max();
                bool ballLocated = false;
                auto balls = a::ball();
                for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
                {
                    const auto& desc = balls->getDescription(idx);
                    const float radius = desc.diameter * 0.5f;
                    const float c1 = (rc.x + radius) - (v.x + 32);
                    const float c2 = (rc.y + radius) - (v.y + 24);
                    const float dist = sqrtf(c1 * c1 + c2 * c2);
                    if (dist < 3)
                    {
                        v.nCaptureMode = 1;
                        v.nCapturedBallIndex = idx;
                        v.frame = 4;
                        v.angle = 240 + a::rnd().Get(60);
                        a::ball()->BallCaptured(v.nCapturedBallIndex, true);
                    }
                    else if (dist < minDist)
                    {
                        ballLocated = true;
                        minDist = dist;
                        angle = (int)(57.3 * asinf(c2 / dist));
                        if (c1 > 0)
                        {
                            angle = (90 + angle) % 360;
                        }
                        else
                        {
                            angle = (270 - angle) % 360;
                        }
                    }
                }

                if (ballLocated == true)
                {
                    for (int nIterations = 0; nIterations < 1 + g_fSpeedCorrection * 8; nIterations++)
                    {
                        if (v.angle < angle)
                        {
                            if (abs(v.angle - angle) >= 180)
                            {
                                v.angle--;
                            }
                            else
                            {
                                v.angle++;
                            }
                        }
                        else
                        {
                            if (abs(v.angle - angle) < 180)
                            {
                                v.angle--;
                            }
                            else
                            {
                                v.angle++;
                            }
                        }
                        v.angle += 360;
                        v.angle %= 360;
                        if (angle == v.angle)
                        {
                            break;
                        }
                    }
                    const float delta = g_fSpeedCorrection * 2.5f;
                    v.x += delta * g_fSin[v.angle];
                    v.y -= delta * g_fCos[v.angle];
                }
            }
            else
            {
                v.x += speed * g_fSin[v.angle];
                v.y -= speed * g_fCos[v.angle];
                ChangeMonsterAngle(i);
                if (v.nCaptureMode == 1)
                {
                    a::ball()->SetCapturedBallPos(v.nCapturedBallIndex, v.x, v.y);
                    if (abs((int)v.x - (RACKET_X - 140)) < 5)
                    {
                        a::ball()->BallCaptured(v.nCapturedBallIndex, false);
                        v.nCaptureMode = 2;
                        v.angle = 90;
                        v.frame = 0;
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
        else if (v.type == eMonsters::EYE)
        {
            if (v.x < RACKET_X - 60)
            {
                v.x += speed;
            }
            if (v.y + v.h / 2 - 5 + a::rnd().Get(10) < nPaddleY + nPaddleHeight / 2)
            {
                v.y += speed;
            }
            else
            {
                v.y -= speed;
            }
        }
        else
        {
            v.x += (speed * g_fSin[v.angle]);
            v.y -= (speed * g_fCos[v.angle]);
            if (v.type == eMonsters::STONE1 || v.type == eMonsters::STONE2)
            {
                if (v.x < WALL_X1 || v.x > (m_bBackWall == true ? WALL_X2 - v.w : SCREEN_WIDTH) || v.y < WALL_Y1 || v.y + v.h > WALL_Y2)
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

    for (size_t i = 0, size = m_wreckage.size(); i < size;)
    {
        auto& v = m_wreckage[i];

        const float delta = g_fSpeedCorrection * v.speed;
        v.x += delta * g_fSin[v.angle];
        v.y -= delta * g_fCos[v.angle];
        if (v.x > SCREEN_WIDTH)
        {
            v = m_wreckage[--size];
            m_wreckage.pop_back();
            continue;
        }
        i++;
    }
}

bool CMonster::IsAcross(int nX, int nY, int nWidth, int nHeight, bool remove)
{
    bool bIsAcross = false;

    for (size_t i = 0, size = m_monsters.size(); i < size; i++)
    {
        const auto& v = m_monsters[i];

        const float r1 = (nWidth + nHeight) / 4.0f;
        const float r2 = (v.w - (v.type == eMonsters::HAND ? 20 : 0) + v.h) / 4.0f;
        const float c1 = ((v.x + v.w * 0.5f) - (nX + nWidth * 0.5f));
        const float c2 = ((v.y + v.h * 0.5f) - (nY + nHeight * 0.5f));
        if (sqrtf(c1 * c1 + c2 * c2) <= r1 + r2)
        {
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            if (remove == true)
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

    for (size_t i = 0, size = m_monsters.size(); i < size; i++)
    {
        const auto& v = m_monsters[i];

        const float r1 = (nWidth + nHeight) / 4.0f;
        const float r2 = (v.w - (v.type == eMonsters::HAND ? 20 : 0) + v.h) / 4.0f;
        const float c1 = (v.x + v.w * 0.5f) - (nX + nWidth * 0.5f);
        const float c2 = (v.y + v.h * 0.5f) - (nY + nHeight * 0.5f);
        if (sqrtf(c1 * c1 + c2 * c2) <= r1 + r2)
        {
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            if (v.type == eMonsters::STONE2)
            {
                a::ark()->m_bCanMovePaddle = false;
                a::ark()->m_nCanMovePaddleCount = 3;
            }
            else if (v.type == eMonsters::PATROL || v.type == eMonsters::STONE1)
            {
                for (int a = 0; a < 6; a++)
                {
                    a::expl()->AddExploision(nX - (45 - 12) / 2 + (10 - a::rnd().Get(20)), nY - (41 - nHeight) / 2 + (nHeight / 2 - a::rnd().Get(nHeight)), 0);
                }
                a::ark()->AddGetReeadyInfo(v.type == eMonsters::PATROL ? "Kamikaze impact paddle" : "Meteor impact paddle");
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

    for (size_t i = 0, size = m_wreckage.size(); i < size;)
    {
        auto& v = m_wreckage[i];

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
            m_wreckage[i] = m_wreckage[--size];
            m_wreckage.pop_back();
            continue;
        }
        i++;
    }

    return bIsAcross;
}

void CMonster::ChangeMonsterAngle(size_t idx)
{
    auto& v = m_monsters[idx];

    if (v.type == eMonsters::HAND && v.nCaptureMode == 2)
    {
        return;
    }
    v.angle %= 360;
    if (v.x < WALL_X1)
    {
        if (v.angle >= 180 && v.angle < 270)
        {
            v.angle = 180 - (v.angle - 180);
        }
        else if (v.angle >= 270 && v.angle < 360)
        {
            v.angle = 360 - v.angle;
        }
    }
    else if (v.x + v.w > WALL_X2)
    {
        if (v.angle >= 0 && v.angle < 90)
        {
            v.angle = 360 - v.angle;
        }
        else if (v.angle >= 90 && v.angle < 180)
        {
            v.angle = 180 + 180 - v.angle;
        }
    }
    else if (v.y < WALL_Y1)
    {
        if (v.angle >= 0 && v.angle < 90)
        {
            v.angle = 180 - v.angle;
        }
        else if (v.angle >= 270 && v.angle < 360)
        {
            v.angle = 180 + (360 - v.angle);
        }
    }
    else if (v.y + v.h > WALL_Y2)
    {
        if (v.angle >= 90 && v.angle < 180)
        {
            v.angle = 180 - v.angle;
        }
        else if (v.angle >= 180 && v.angle < 270)
        {
            v.angle = 360 - (v.angle - 180);
        }
    }

    v.x = std::max<int>(v.x, WALL_X1);
    v.x = std::min<int>(v.x, WALL_X2 - v.w);
    v.y = std::max<int>(v.y, WALL_Y1);
    v.y = std::min<int>(v.y, WALL_Y2 - v.h);
}

bool CMonster::IsAcrossBall(const sBallDescription& desc, bool remove)
{
    bool bIsAcross = false;

    const float radius = desc.diameter * 0.5f;
    for (size_t i = 0, size = m_monsters.size(); i < size; i++)
    {
        const auto& v = m_monsters[i];

        if (v.type != eMonsters::HAND)
        {
            const float c1 = desc.x + radius - (v.x + v.w * 0.5f);
            const float c2 = desc.y + radius - (v.y + v.h * 0.5f);
            if (sqrtf(c1 * c1 + c2 * c2) < radius + ((v.w + v.h) / 4))
            {
                a::ark()->AddScore(25);
                bIsAcross = true;
                if (remove == true)
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

bool CMonster::IsAcrossBall(const sBallDescription& desc)
{
    bool bIsAcross = false;

    const float radius = desc.diameter * 0.5f;
    for (size_t i = 0, size = m_wreckage.size(); i < size;)
    {
        auto& v = m_wreckage[i];

        const float c1 = desc.x + radius - (v.x + v.w * 0.5f);
        const float c2 = desc.y + radius - (v.y + v.h * 0.5f);
        if (sqrtf(c1 * c1 + c2 * c2) < radius + ((v.w + v.h) / 4))
        {
            a::ark()->AddScore(100);
            bIsAcross = true;
            a::ark()->m_nGetReadyMonsters++;
            a::expl()->AddExploision(v.x + (v.w - 45) / 2, v.y + (v.h - 41) / 2, 0);
            m_wreckage[i] = m_wreckage[--size];
            m_wreckage.pop_back();
            continue;
        }
        i++;
    }

    return bIsAcross;
}

void CMonster::RemoveByPos(size_t idx)
{
    auto& v = m_monsters[idx];

    if (v.type == eMonsters::PATROL)
    {
        StopSound(m_nSndPatrol);
    }
    else if (v.type == eMonsters::HAND)
    {
        a::ball()->BallCaptured(v.nCapturedBallIndex, false);
    }
    v = m_monsters.back();
    m_monsters.pop_back();
}
