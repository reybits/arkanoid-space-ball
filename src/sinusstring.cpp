/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "sinusstring.h"
#include "accessor.h"
#include "defines.h"
#include "videosystem/videosystem.h"

CSinusString::CSinusString()
    : m_pbySymbs(nullptr)
    , m_nSinPos(0)
    , m_nAmplifier(1)
    , m_nY(0)
    , m_nX(0)
    , m_nStartPos(0)
    , m_nStrLen(0)
    , m_pchString(nullptr)
{
}

CSinusString::~CSinusString()
{
    delete[] m_pbySymbs;
    delete[] m_pchString;
}

void CSinusString::Draw()
{
    static bool bLoaded = false;
    if (bLoaded == false)
    {
        bLoaded = true;
        Uint32 nDataLen;
        Uint8* pbyData = a::res()->GetDataAllocMem("sinusstring.bin", nDataLen);
        if (pbyData)
        {
            m_pbySymbs = new Uint8[nDataLen];
            memcpy(m_pbySymbs, pbyData, nDataLen);
            a::res()->FreeMem(pbyData);
            printf("  done.\n");
        }
    }
#define DELTA 3
    int nPrevPos = m_nSinPos;
    SDL_Rect rc;
    SetRect(&rc, 0, 0, 6, 6);

    bool bOutScreen = false;
    for (int s = m_nStartPos; s < m_nStrLen; s++)
    {
        int nX = (int)m_nX + 8 * s * DELTA;
        if (nX + 8 * DELTA >= 0)
        {
            int nOffset = 8 * m_pchString[s];
            int nSinPos = m_nSinPos;
            for (int y = 0; y < 8; y++)
            {
                //int   nDy = (int)(((float)m_nAmplifier / y) * g_pMainFrame->m_afSin[nSinPos]);
                Uint8 byBits = m_pbySymbs[nOffset + y];
                for (int x = 0; x < 8; x++)
                {
                    int nDy = (int)(m_nAmplifier * g_fSin[nSinPos]);
                    if (nX + x * DELTA > SCREEN_WIDTH)
                    {
                        bOutScreen = true;
                        break;
                    }
                    if (byBits & 0x80)
                    {
                        rc.y = 6;
                        render(nX + x * DELTA + 2, (int)(m_nY + y * DELTA + 2 + nDy), eImage::SinusString, &rc);
                        rc.y = 0;
                        render(nX + x * DELTA, (int)(m_nY + y * DELTA + nDy), eImage::SinusString, &rc);
                    }
                    byBits <<= 1;
                    nSinPos += 3;
                    nSinPos %= 360;
                }
                nSinPos = m_nSinPos;
            }
        }
        else
        {
            m_nStartPos++;
            nPrevPos += 8 * 3;
            nPrevPos %= 360;
        }
        m_nSinPos += 8 * 3;
        m_nSinPos %= 360;
        if (bOutScreen == true)
        {
            break;
        }
    }

    m_nX -= g_fSpeedCorrection;
    if (m_nStartPos == m_nStrLen - 1)
    {
        m_nX = SCREEN_WIDTH;
        m_nStartPos = 0;
    }

    m_nSinPos = nPrevPos;
    static Uint32 dwTime = 0;
    if (dwTime + 10 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        m_nSinPos += 5;
        m_nSinPos %= 360;
    }
}

void CSinusString::SetString(const char* pchString, int nX, int nY)
{
    m_nStrLen = strlen(pchString);
    delete[] m_pchString;
    m_pchString = new char[m_nStrLen + 1];
    strcpy(m_pchString, pchString);
    m_nX = (float)nX;
    m_nY = nY;
    m_nStartPos = 0;
    m_nAmplifier = 6;
    //printf("Sinus string set to '%s'\n", m_pchString);
}
