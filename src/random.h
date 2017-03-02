/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <SDL.h>

class CRandom final
{
public:
    CRandom()
        : m_nSeed(SDL_GetTicks())
    {
    }

    CRandom(int nSeed)
    {
        m_nSeed = nSeed;
    }

    int Get(int nMax)
    {
        m_nSeed = m_nSeed * 0x343fD + 0x269ec3;
        return ((m_nSeed >> 16) & 0x7fff) % nMax;
    }

protected:
    int m_nSeed;
};
