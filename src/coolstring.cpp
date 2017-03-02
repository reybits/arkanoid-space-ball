/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "coolstring.h"
#include "accessor.h"
#include "defines.h"
#include "mystring.h"

#include "coolfont.hpp"

#include <SDL.h>

CCoolString::CCoolString()
{
    m_vecCoolString.reserve(10);
}

CCoolString::~CCoolString()
{
    Clear();
}

void CCoolString::Add(const char* pchFormat, ...)
{
    va_list ap;
    char    achBuf[50];

    va_start(ap, pchFormat);
    vsprintf(achBuf, pchFormat, ap);
    va_end(ap);

    _COOL_STRING    cs;
    cs.pchString    = new char[strlen(achBuf) + 1];
    cs.nSeconds     = 2;    // 1/2 seconds to show
    strcpy(cs.pchString, achBuf);
    m_vecCoolString.push_back(cs);
}

void CCoolString::Draw()
{
    SDL_Rect    rc;
    static Uint32   dwTime  = 0;
    bool    bStep   = false;
    if (dwTime + 650 < SDL_GetTicks())
    {
        dwTime  = SDL_GetTicks();
        bStep   = true;
    }

    // draw frame
    SetRect(&rc, 0, 0, 160, 20);
    if (m_vecCoolString.size() > 0)
    {
        Blit(440, WALL_Y1, g_pTransp, &rc);
        a::fntTut()->SetRect(440, WALL_Y1 + 2, 160, 20);
        a::fntTut()->DrawString(0, 0, m_vecCoolString[0].pchString, 2);
        if (bStep == true && --m_vecCoolString[0].nSeconds == 0)
        {
            delete[] m_vecCoolString[0].pchString;
            m_vecCoolString[0] = m_vecCoolString.back();
            m_vecCoolString.pop_back();
        }
    }
}

void CCoolString::Clear()
{
    for (size_t i = 0; i < m_vecCoolString.size(); i++)
    {
        delete[] m_vecCoolString[i].pchString;
    }
    m_vecCoolString.clear();
}

int CCoolString::GetCount()
{
    return  m_vecCoolString.size();
}
