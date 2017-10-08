/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "coolstring.h"
#include "accessor.h"
#include "coolfont.hpp"
#include "defines.h"
#include "mystring.h"
#include "videosystem/videosystem.h"

#include <SDL.h>
#include <cstdio>

CCoolString::CCoolString()
{
    m_strings.reserve(10);
}

CCoolString::~CCoolString()
{
    Clear();
}

void CCoolString::Add(const char* fmt, ...)
{
    va_list ap;
    char buffer[50];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    _COOL_STRING cs;
    cs.string = new char[strlen(buffer) + 1];
    cs.seconds = 2; // 1/2 seconds to show
    strcpy(cs.string, buffer);
    m_strings.push_back(cs);
}

void CCoolString::Draw()
{
    SDL_Rect rc;
    static Uint32 lastTime = 0;
    bool update = false;
    if (lastTime + 650 < SDL_GetTicks())
    {
        lastTime = SDL_GetTicks();
        update = true;
    }

    // draw frame
    SetRect(&rc, 0, 0, 160, 20);
    if (m_strings.size() > 0)
    {
        auto& s = m_strings[0];

        render(440, WALL_Y1, eImage::Transp, &rc);
        a::fntTut()->SetRect(440, WALL_Y1 + 2, 160, 20);
        a::fntTut()->DrawString(0, 0,  s.string, CMyString::eAlign::Center);
        if (update && --s.seconds == 0)
        {
            delete[] s.string;
            m_strings[0] = m_strings.back();
            m_strings.pop_back();
        }
    }
}

void CCoolString::Clear()
{
    for (auto& s : m_strings)
    {
        delete[] s.string;
    }
    m_strings.clear();
}

int CCoolString::GetCount()
{
    return m_strings.size();
}
