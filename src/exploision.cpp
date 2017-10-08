/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "exploision.h"
#include "defines.h"
#include "videosystem/videosystem.h"

#include <SDL.h>

#define NUM_OF_FRAMES 9

CExploision::CExploision()
{
    m_exlosions.reserve(30);
}

CExploision::~CExploision()
{
}

void CExploision::Draw(bool bBricks)
{
    static Uint32 dwTime = 0;
    static bool bUpdate = false;
    if (bBricks == true)
    {
        bUpdate = false;
        if (dwTime + 80 < SDL_GetTicks())
        {
            dwTime = SDL_GetTicks();
            bUpdate = true;
        }
    }
    SDL_Rect rc;
    rc.w = 45;
    rc.h = 41;
    for (size_t i = 0, size = m_exlosions.size(); i < size;)
    {
        auto& e = m_exlosions[i];

        if ((bBricks == true && e.type == 3) || (bBricks == false && e.type != 3))
        {
            rc.x = 40 + e.type * 45;
            rc.y = e.nFrame * 41;
            render(e.x, e.y, eImage::Exploision, &rc);
            if (bUpdate == true && ++e.nFrame == NUM_OF_FRAMES)
            {
                m_exlosions[i] = m_exlosions[--size];
                m_exlosions.pop_back();
                continue;
            }
        }

        i++;
    }
}

void CExploision::AddExploision(int x, int y, int type)
{
    eExpl e;
    e.type = type;
    e.x = x;
    e.y = y;
    e.nFrame = 0;
    m_exlosions.push_back(e);
    PlaySound(15);
}

void CExploision::RemoveAll()
{
    m_exlosions.clear();
}

size_t CExploision::GetCount() const
{
    return m_exlosions.size();
}
