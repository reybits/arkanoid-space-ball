/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "energyhole.h"
#include "accessor.h"
#include "ball.h"
#include "bullet.h"
#include "defines.h"
#include "random.h"
#include "tutorialdlg.h"

#include <cmath>

CEnergyHole::CEnergyHole()
{
}

CEnergyHole::~CEnergyHole()
{
}

void CEnergyHole::RemoveAll()
{
    m_vecEnergyHole.clear();
}

void CEnergyHole::Draw()
{
    SDL_Rect rc;
    rc.x = 0;
    rc.w = rc.h = 80;
    for (const auto& v : m_vecEnergyHole)
    {
        rc.y = !v.is_over ? 0 : 80;
        Blit(v.x, v.y, m_pEnergyHole, &rc);
    }
}

void CEnergyHole::AddEnergyHole()
{
    sEnergyHole hole;
    hole.x = WALL_X1 + a::rnd().Get(WALL_X2 - WALL_X1 - 100);
    hole.y = WALL_Y1 + a::rnd().Get(WALL_Y2 - WALL_Y1 - 100);
    hole.frame = 0;
    hole.time = 0;
    hole.move_time = 0;
    hole.angle = a::rnd().Get(360);
    m_vecEnergyHole.push_back(hole);
}

void CEnergyHole::Move()
{
    auto curTime = SDL_GetTicks();
    const float speed = g_fSpeedCorrection * 0.2f;

    for (auto& v : m_vecEnergyHole)
    {
        // change angle
        if (v.move_time + 5000 < curTime)
        {
            v.move_time = curTime;
            v.angle += a::rnd().Get(90);
            v.angle %= 360;
        }
        v.x += speed * g_fSin[v.angle];
        v.y -= speed * g_fCos[v.angle];

        v.x = std::min<int>(RACKET_X - 80 - 60, v.x);
        v.x = std::max<int>(WALL_X1, v.x);
        v.y = std::min<int>(WALL_Y2 - 80, v.y);
        v.y = std::max<int>(WALL_Y1, v.y);
        v.is_over = false;

        SDL_Rect rc;
        // is ball over energy hole? correct ball angle then
        int idx = 0;
        while (a::ball()->GetPositionAndDiameter(rc, idx))
        {
            const float c1 = (v.x + 80 * 0.5f) - (rc.x + rc.w * 0.5f);
            const float c2 = (v.y + 80 * 0.5f) - (rc.y + rc.w * 0.5f);
            float distance = sqrtf(c1 * c1 + c2 * c2);
            if (distance < 80 * 0.5f)
            {
                a::tutDlg()->AddDialog(rc.x + rc.w / 2, rc.y + rc.w / 2, 0, 5);

                v.is_over = true;
                int angle = (int)(57.3 * asinf(c2 / distance));
                if (c1 > 0)
                {
                    angle = a::ball()->GetAngle(idx - 1) - (90 + angle) % 360;
                }
                else
                {
                    angle = a::ball()->GetAngle(idx - 1) - (270 - angle) % 360;
                }
                if (abs(angle) < 180)
                {
                    a::ball()->ChangeAngle(idx - 1, angle < 0 ? true : false);
                }
                else
                {
                    a::ball()->ChangeAngle(idx - 1, angle < 0 ? false : true);
                }
            }
        }

        // is bullet over energy hole? correct bullet angle then
        a::bullet()->GetPositionAndSize(rc, idx, true);
        while (a::bullet()->GetPositionAndSize(rc, idx, false))
        {
            const float c1 = (v.x + 80 * 0.5f) - (rc.x + rc.w * 0.5f);
            const float c2 = (v.y + 80 * 0.5f) - (rc.y + rc.h * 0.5f);
            const float distance = sqrtf(c1 * c1 + c2 * c2);
            if (distance < 80 * 0.5f)
            {
                a::tutDlg()->AddDialog(rc.x + rc.w / 2, rc.y + rc.h / 2, 0, 6);
                v.is_over = true;
                if (c2 > 0)
                {
                    a::bullet()->ChangeAngle(idx - 1, false);
                }
                else
                {
                    a::bullet()->ChangeAngle(idx - 1, true);
                }
            }
        }
    }
}
