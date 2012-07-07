/***************************************************************************
 *   Copyright (C) 2006 by Andrey A. Ugolnik                               *
 *   andrey@wegroup.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "arkanoidsb.h"
#include "energyhole.h"

CEnergyHole::CEnergyHole()
{
    RemoveAll();
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
    for(size_t i = 0; i < m_vecEnergyHole.size(); i++)
    {
        rc.y = !m_vecEnergyHole[i].is_over ? 0 : 80;
        Blit((Uint32)m_vecEnergyHole[i].x, (Uint32)m_vecEnergyHole[i].y, m_pEnergyHole, &rc);
    }
}

void CEnergyHole::AddEnergyHole()
{
    sEnergyHole hole;
    hole.x = WALL_X1 + g_Rnd.Get(WALL_X2 - WALL_X1 - 100);
    hole.y = WALL_Y1 + g_Rnd.Get(WALL_Y2 - WALL_Y1 - 100);
    hole.frame = 0;
    hole.time = 0;
    hole.move_time = 0;
    hole.angle = g_Rnd.Get(360);
    m_vecEnergyHole.push_back(hole);
}

void CEnergyHole::Move()
{
    Uint32 time = SDL_GetTicks();

    for(size_t i = 0; i < m_vecEnergyHole.size(); i++)
    {
        float speed = g_fSpeedCorrection * 0.2f;

        // change angle
        if(m_vecEnergyHole[i].move_time + 5000 < time)
        {
            m_vecEnergyHole[i].move_time = time;
            m_vecEnergyHole[i].angle += g_Rnd.Get(90);
            m_vecEnergyHole[i].angle %= 360;
        }
        m_vecEnergyHole[i].x += (speed * g_fSin[m_vecEnergyHole[i].angle]);
        m_vecEnergyHole[i].y -= (speed * g_fCos[m_vecEnergyHole[i].angle]);

        m_vecEnergyHole[i].x = min(RACKET_X - 80 - 60, m_vecEnergyHole[i].x);
        m_vecEnergyHole[i].x = max(WALL_X1, m_vecEnergyHole[i].x);
        m_vecEnergyHole[i].y = min(WALL_Y2 - 80, m_vecEnergyHole[i].y);
        m_vecEnergyHole[i].y = max(WALL_Y1, m_vecEnergyHole[i].y);
        m_vecEnergyHole[i].is_over = false;

        SDL_Rect rc;
        // is ball over energy hole? correct ball angle then
        int idx = 0;
        while(g_Ball.GetPositionAndDiameter(rc, idx))
        {
            int c1 = (m_vecEnergyHole[i].x + 80/2) - (rc.x + rc.w / 2);
            int c2 = (m_vecEnergyHole[i].y + 80/2) - (rc.y + rc.w / 2);
            float distance = sqrtf(c1 * c1 + c2 * c2);
            if(distance < 80/2)
            {
                g_TutorialDlg.AddDialog(rc.x + rc.w / 2, rc.y + rc.w / 2, 0, 5);

                m_vecEnergyHole[i].is_over = true;
                int angle = (int)(57.3 * asin(c2 / distance));
                if(c1 > 0)
                {
                    angle = g_Ball.GetAngle(idx - 1) - (90 + angle) % 360;
                }
                else
                {
                    angle = g_Ball.GetAngle(idx - 1) - (270 - angle) % 360;
                }
                if(abs(angle) < 180)
                {
                    g_Ball.ChangeAngle(idx - 1, angle < 0 ? true : false);
                }
                else
                {
                    g_Ball.ChangeAngle(idx - 1, angle < 0 ? false : true);
                }
            }
        }

        // is bullet over energy hole? correct bullet angle then
        g_Bullet.GetPositionAndSize(rc, idx, true);
        while(g_Bullet.GetPositionAndSize(rc, idx, false))
        {
            int c1 = int(m_vecEnergyHole[i].x + 80/2) - (rc.x + rc.w / 2);
            int c2 = int(m_vecEnergyHole[i].y + 80/2) - (rc.y + rc.h / 2);
            float distance = sqrtf(c1 * c1 + c2 * c2);
            if(distance < 80/2) {
                g_TutorialDlg.AddDialog(rc.x + rc.w / 2, rc.y + rc.h / 2, 0, 6);
                m_vecEnergyHole[i].is_over = true;
                if(c2 > 0)
                {
                    g_Bullet.ChangeAngle(idx - 1, false);
                }
                else
                {
                    g_Bullet.ChangeAngle(idx - 1, true);
                }
            }
        }
    }
}

