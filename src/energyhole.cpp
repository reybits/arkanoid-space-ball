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
#include "videosystem/videosystem.h"

#include <cmath>
#include <cstdlib>

CEnergyHole::CEnergyHole()
{
}

CEnergyHole::~CEnergyHole()
{
}

void CEnergyHole::RemoveAll()
{
    m_holes.clear();
}

void CEnergyHole::Draw()
{
    SDL_Rect rc;
    rc.x = 0;
    rc.w = rc.h = 80;
    for (const auto& v : m_holes)
    {
        rc.y = !v.is_over ? 0 : 80;
        render(v.x, v.y, eImage::EnergyHole, &rc);
    }
}

void CEnergyHole::AddEnergyHole()
{
    sEnergyHole hole;
    hole.x = WALL_X1 + a::rnd().Get(WALL_X2 - WALL_X1 - 100);
    hole.y = WALL_Y1 + a::rnd().Get(WALL_Y2 - WALL_Y1 - 100);
    hole.frame = 0;
    hole.frameTime = 0;
    hole.move_time = 0;
    hole.angle = a::rnd().Get(360);
    m_holes.push_back(hole);
}

void CEnergyHole::Move()
{
    auto curTime = SDL_GetTicks();
    const float speed = g_fSpeedCorrection * 0.2f;

    for (auto& v : m_holes)
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

        // is ball over energy hole? correct ball angle then
        auto balls = a::ball();
        for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
        {
            const auto& desc = balls->getDescription(idx);
            if (desc.bIsCaptured == false)
            {
                const float radius = desc.diameter * 0.5f;
                const float c1 = (v.x + 80 * 0.5f) - (desc.x + radius);
                const float c2 = (v.y + 80 * 0.5f) - (desc.y + radius);
                const float distance = sqrtf(c1 * c1 + c2 * c2);
                if (distance < 80 * 0.5f)
                {
                    a::tutDlg()->AddDialog(desc.x + radius, desc.y + radius, 0, 5);

                    v.is_over = true;
                    int angle = (int)(57.3f * asinf(c2 / distance));
                    if (c1 > 0)
                    {
                        angle = balls->getAngle(idx) - (90 + angle) % 360;
                    }
                    else
                    {
                        angle = balls->getAngle(idx) - (270 - angle) % 360;
                    }
                    if (abs(angle) < 180)
                    {
                        balls->ChangeAngle(idx, angle < 0 ? true : false);
                    }
                    else
                    {
                        balls->ChangeAngle(idx, angle < 0 ? false : true);
                    }
                }
            }
        }

        // is bullet over energy hole? correct bullet angle then
        auto bullets = a::bullet();
        for (size_t idx = 0, size = bullets->getCount(); idx < size; idx++)
        {
            const auto& desc = bullets->getDescription(idx);
            const float radius = desc.h * 0.5f;
            const float c1 = (v.x + 80 * 0.5f) - (desc.x + radius);
            const float c2 = (v.y + 80 * 0.5f) - (desc.y + radius);
            const float distance = sqrtf(c1 * c1 + c2 * c2);
            if (distance < 80 * 0.5f)
            {
                a::tutDlg()->AddDialog(desc.x + radius, desc.y + radius, 0, 6);
                v.is_over = true;
                if (c2 > 0)
                {
                    bullets->ChangeAngle(idx, false);
                }
                else
                {
                    bullets->ChangeAngle(idx, true);
                }
            }
        }
    }
}
