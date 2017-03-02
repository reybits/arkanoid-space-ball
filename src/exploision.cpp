/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "exploision.h"
#include "defines.h"

#include <SDL.h>

#define NUM_OF_FRAMES    9

CExploision::CExploision()
{
    m_vecExploisions.reserve(30);
}

CExploision::~CExploision()
{
}

void CExploision::Draw(bool bBricks)
{
    static Uint32   dwTime  = 0;
    static bool     bUpdate = false;
    if (bBricks == true)
    {
        bUpdate = false;
        if (dwTime + 80 < SDL_GetTicks())
        {
            dwTime  = SDL_GetTicks();
            bUpdate = true;
        }
    }
    SDL_Rect    rc;
    rc.w    = 45;
    rc.h    = 41;
    for (size_t i = 0, size = m_vecExploisions.size(); i < size;)
    {
        if ((bBricks == true && m_vecExploisions[i].nType == 3) || (bBricks == false && m_vecExploisions[i].nType != 3))
        {
            rc.x = 40 + m_vecExploisions[i].nType * 45;
            rc.y = m_vecExploisions[i].nFrame * 41;
            Blit(m_vecExploisions[i].x, m_vecExploisions[i].y, m_pExploision, &rc);
            if (bUpdate == true && ++m_vecExploisions[i].nFrame == NUM_OF_FRAMES)
            {
                m_vecExploisions[i] = m_vecExploisions[--size];
                m_vecExploisions.pop_back();
                continue;
            }
        }

        i++;
    }
}

void CExploision::AddExploision(int x, int y, int nType)
{
    _EXPLOISION expl;
    expl.nType      = nType;
    expl.x          = x;
    expl.y          = y;
    expl.nFrame     = 0;
    m_vecExploisions.push_back(expl);
    PlaySound(15);
}

void CExploision::RemoveAll()
{
    m_vecExploisions.clear();
}

int CExploision::GetCount()
{
    return  m_vecExploisions.size();
}
