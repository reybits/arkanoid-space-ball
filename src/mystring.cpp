/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "mystring.h"
#include "accessor.h"
#include "defines.h"
#include "videosystem/videosystem.h"

#include <cassert>
#include <cmath>

namespace
{

    bool InitializeSin = true;
    float Sin[360];

}

CMyString::CMyString()
    : m_fnt(nullptr)
    , m_shadow(nullptr)
    , m_lineHeight(0)
    , m_drawShadow(false)
    , m_fJustifyWidth(0.0f)
    , m_kerning(0)
    , m_nShadowDx(1)
    , m_nShadowDy(1)
{
    if (InitializeSin)
    {
        InitializeSin = false;
        for (int i = 0; i < 360; i++)
        {
            Sin[i] = sinf((M_PI / 180.0f) * i);
        }
    }

    m_winRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
}

CMyString::~CMyString()
{
    Unload();
}

bool CMyString::loadFont(const char* fontName, const char* fontIniName, const char* fontShadowName)
{
    if (fontShadowName != nullptr)
    {
        m_shadow = a::res()->loadImage(fontShadowName);
        if (m_shadow != nullptr)
        {
            m_drawShadow = true;
        }
    }

    m_fnt = a::res()->loadImage(fontName);
    if (m_fnt != nullptr)
    {
        return LoadProps(fontIniName);
    }

    return false;
}

void CMyString::SetShadowOffset(int nDx, int nDy)
{
    m_nShadowDx = nDx;
    m_nShadowDy = nDy;
}

void CMyString::EnableShadow(bool bEnable)
{
    m_drawShadow = bEnable;
}

Uint32 CMyString::GetFrameWidth(Uint32 index)
{
    if (index < m_frames.size())
    {
        return m_frames[index].w;
    }
    return 0;
}

void CMyString::DrawString(int nX, int nY, const char* str, eAlign align)
{
    if (str == nullptr)
    {
        return;
    }

    int nYPos = m_winRect.y + nY;
    float nXPos = GetXpos(nX, str, align);

    while (*str)
    {
        const uint8_t bySymb = *str++;
        if (bySymb == '\n')
        {
            nXPos = GetXpos(nX, str, align);
            nYPos += m_lineHeight;
            continue;
        }
        if (align == eAlign::Justify && bySymb == ' ')
        {
            nXPos += m_fJustifyWidth;
            continue;
        }

        const auto idx = m_symbIndex[bySymb];
        auto frame = &m_frames[idx];

        SDL_Rect rc;
        rc.x = nXPos;
        rc.y = nYPos + m_anSymbDy[bySymb];
        if (m_drawShadow == true && m_shadow != nullptr)
        {
            render(rc.x + m_nShadowDx, rc.y + m_nShadowDy, m_shadow, frame);
            // SDL_Rect rcShadow;
            // rcShadow.x = rc.x + m_nShadowDx;
            // rcShadow.y = rc.y + m_nShadowDy;
            // SDL_BlitSurface(m_shadow, frame, g_psurfScreen, &rcShadow);
        }
        render(rc.x, rc.y, m_fnt, frame);
        // SDL_BlitSurface(m_fnt, frame, g_psurfScreen, &rc);

        const int width = GetFrameWidth(idx);
        nXPos += width + m_kerning;
    }
}

int CMyString::GetXpos(int nX, const char* str, eAlign align)
{
    if (align == eAlign::Right)
    {
        return m_winRect.x + m_winRect.w - nX - GetStringWidth(str);
    }
    else if (align == eAlign::Center)
    {
        return m_winRect.x + (m_winRect.w - GetStringWidth(str)) / 2;
    }
    else if (align == eAlign::Left)
    {
        return m_winRect.x + nX;
    }
    else
    {
        int nSpaces;
        int nWidth = GetStringWidth(str, nSpaces);
        m_fJustifyWidth = (m_winRect.w - nWidth) / (float)nSpaces;
        return m_winRect.x + nX;
    }
}

int CMyString::GetWordWidth(const char* str, int& nCharsCount)
{
    int nStrWidth = 0;
    nCharsCount = 0;
    while (*str && *str != ' ' &&  *str != '\n')
    {
        nCharsCount++;
        nStrWidth += GetFrameWidth(m_symbIndex[(int)*str++]);
    }

    return nStrWidth + m_kerning * nCharsCount;
}

void CMyString::DrawString2(int nX, int nY, const char* str)
{
    if (str == nullptr)
    {
        return;
    }

    int nYPos = m_winRect.y + nY;

    int nSpaceWidth = GetFrameWidth(m_symbIndex[(int)' ']);

    while (*str)
    {
        const char* pchTmp = str;
        int nCharsWord;
        int nCharsLine = 0;
        int nSpaces = -1;
        int nWidthWord;
        int nWidthLine = 0;
        while (*pchTmp)
        {
            nWidthWord = GetWordWidth(pchTmp, nCharsWord);
            if (nWidthLine + nWidthWord + nSpaceWidth * nSpaces < m_winRect.w)
            {
                nWidthLine += nWidthWord;
                pchTmp     += nCharsWord;
                nCharsLine += nCharsWord;
                nSpaces++;
                while (*pchTmp == ' ')
                {
                    pchTmp++; // skip spaces
                }
            }
            else
            {
                break;
            }
        }
        m_fJustifyWidth = (m_winRect.w - nWidthLine) / (float)nSpaces;
        float nXPos = m_winRect.x + nX;
        while (nCharsLine)
        {
            Uint8 bySymb = *str++;
            if (bySymb == ' ')
            {
                if (*pchTmp)
                {
                    nXPos += m_fJustifyWidth;
                }
                else
                {
                    nXPos += nSpaceWidth;
                }
                continue;
            }
            nCharsLine--;

            const auto idx = m_symbIndex[bySymb];
            auto frame = &m_frames[idx];

            SDL_Rect rc;
            rc.x = nXPos;
            rc.y = nYPos + m_anSymbDy[bySymb];
            if (m_drawShadow == true && m_shadow != nullptr)
            {
                render(rc.x + m_nShadowDx, rc.y + m_nShadowDy, m_shadow, frame);
                // SDL_Rect rcShadow;
                // rcShadow.x = rc.x + m_nShadowDx;
                // rcShadow.y = rc.y + m_nShadowDy;
                // SDL_BlitSurface(m_shadow, frame, g_psurfScreen, &rcShadow);
            }
            render(rc.x, rc.y, m_fnt, frame);
            // SDL_BlitSurface(m_fnt, frame, g_psurfScreen, &rc);

            const int width = GetFrameWidth(idx);
            nXPos += width + m_kerning;
        }
        nYPos += m_lineHeight;
        if (*str == '\n')
        {
            str++;
        }
        while (*str == ' ')
        {
            str++; // skip spaces
        }
    }
}

void CMyString::DrawStringSinus(int nX, int nY, const char* str, int nAplituda, int nPos, eAlign align)
{
    if (str == nullptr)
    {
        return;
    }

    int nYPos = m_winRect.y + nY;
    int nXPos = GetXpos(nX, str, align);

    while (*str)
    {
        nPos %= 360;
        Uint8 bySymb = *str++;

        if (bySymb == '\n')
        {
            nXPos = GetXpos(nX, str, align);
            nYPos += m_lineHeight;
            continue;
        }

        const auto idx = m_symbIndex[bySymb];
        auto frame = &m_frames[idx];

        SDL_Rect rc;
        rc.x = nXPos;
        rc.y = nYPos + (int)(Sin[nPos] * nAplituda) + m_anSymbDy[bySymb];
        if (m_drawShadow == true && m_shadow != nullptr)
        {
            render(rc.x + m_nShadowDx, rc.y + m_nShadowDy, m_shadow, frame);
            // SDL_Rect rcShadow;
            // rcShadow.x = rc.x + m_nShadowDx;
            // rcShadow.y = rc.y + m_nShadowDy;
            // SDL_BlitSurface(m_shadow, frame, g_psurfScreen, &rcShadow);
        }
        render(rc.x, rc.y, m_fnt, frame);
        // SDL_BlitSurface(m_fnt, frame, g_psurfScreen, &rc);

        const int width = GetFrameWidth(idx);
        nPos += width;
        nXPos += width;
    }
}

void CMyString::DrawNumber(int nNum, int nX, int nY, eAlign align)
{
    char achBuf[20];
    sprintf(achBuf, "%d", nNum);
    DrawString(nX, nY, achBuf, align);
}

int CMyString::GetStringWidth(const char* str)
{
    int symbolsCount = 0;
    int width = 0;

    for (uint8_t symb = *str; *str && symb != '\n'; symb = *str++)
    {
        symbolsCount++;
        width += GetFrameWidth(m_symbIndex[symb]);
    }

    return width + m_kerning * symbolsCount;
}

int CMyString::GetStringWidth(const char* str, int& spacesCount)
{
    int symbolsCount = 0;
    int width = 0;

    spacesCount = 0;

    for (uint8_t symb = *str; *str && symb != '\n'; symb = *str++)
    {
        if (symb == ' ')
        {
            spacesCount++;
        }
        else
        {
            symbolsCount++;
            width += GetFrameWidth(m_symbIndex[symb]);
        }
    }

    return width + m_kerning * symbolsCount;
}

Uint32 CMyString::RegisterFrame(const SDL_Rect& rc)
{
    m_frames.push_back(rc);
    return m_frames.size();
}

void CMyString::Unload()
{
    if (m_fnt != nullptr)
    {
        SDL_FreeSurface(m_fnt);
        m_fnt = nullptr;
    }
}

void CMyString::SetRect(int left, int top, int width, int height)
{
    m_winRect.x = left;
    m_winRect.y = top;
    m_winRect.w = width;
    m_winRect.h = height;
}

bool CMyString::LoadProps(const char* pchName)
{
    unsigned int nDataLen;
    char* data = (char*)a::res()->GetDataAllocMem(pchName, nDataLen);
    if (data != nullptr)
    {
        memset(m_symbIndex, 0, sizeof(m_symbIndex));
        memset(m_anSymbDy, 0, sizeof(m_anSymbDy));

        int x, y;
        uint32_t w, h;

        char* pchLine = strtok(data, "\r\n");

        while (pchLine != nullptr)
        {
            uint8_t bySymbName;
            int index, dummy, dy;
            if (sscanf(pchLine, "Char=\"%c\"; %d,%d,%u,%u; %d,%d,%d", &bySymbName, &x, &y, &w, &h, &dummy, &dummy, &dy) == 8)
            {
                const SDL_Rect rc{ (int16_t)x, (int16_t)y, (uint16_t)w, (uint16_t)h };
                m_symbIndex[bySymbName] = RegisterFrame(rc) - 1;
                m_anSymbDy[bySymbName] = dy;
            }
            else if (sscanf(pchLine, "Char=%x; %d,%d,%u,%u; %d,%d,%d", &index, &x, &y, &w, &h, &dummy, &dummy, &dy) == 8)
            {
                if (index > 256)
                {
                    index -= 848;
                }
                bySymbName = index;
                const SDL_Rect rc{ (int16_t)x, (int16_t)y, (uint16_t)w, (uint16_t)h };
                m_symbIndex[bySymbName] = RegisterFrame(rc) - 1;
                m_anSymbDy[bySymbName] = dy;
            }
            else if (sscanf(pchLine, "Height=%d", &dummy) == 1)
            {
                m_lineHeight = dummy;
            }

            pchLine = strtok(nullptr, "\r\n");
        }

        a::res()->FreeMem((unsigned char*)data);
        printf(" done.\n");
        return true;
    }

    return false;
}

bool CMyString::loadFont(const char* fontName, int space, int kerning, const char* charset)
{
    if (charset == nullptr)
    {
        charset = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    }

    m_kerning = kerning;
    m_fnt = a::res()->loadImage(fontName);
    if (m_fnt != nullptr)
    {
        memset(m_symbIndex, 0, sizeof(m_symbIndex));
        memset(m_anSymbDy, 0, sizeof(m_anSymbDy));
        m_lineHeight = m_fnt->h - 1;

        SDL_Rect rc{ 0, 1, 0, (Uint16)m_lineHeight };
        for (int x = 0; x < m_fnt->w; x++)
        {
            if (isTransparent(x))
            {
                rc.x = x;
                while (x < m_fnt->w && isTransparent(x))
                {
                    x++;
                }
                rc.w = x - rc.x;
                Uint8 bySymbName = *charset++;
                m_symbIndex[bySymbName] = RegisterFrame(rc) - 1;
                // printf("sym '%c': %d , %d - %d x %d\n", (char)bySymbName, (int)rc.x, (int)rc.y, (int)rc.w, (int)rc.h);
            }
        }

        m_symbIndex[(int)' '] = RegisterFrame({ 0, 0, (Uint16)space, 0 }) - 1;

        return true;
    }

    return false;
}

bool CMyString::isTransparent(int x) const
{
    assert(m_fnt != nullptr);
    assert(m_fnt->format->BytesPerPixel == 4);
    assert(x >= 0 && x < m_fnt->w);

    auto bmp = (const uint32_t*)m_fnt->pixels;
    const Uint32 pixel = bmp[x];

    const Uint8 alpha = (pixel & m_fnt->format->Amask) >> m_fnt->format->Ashift;
    return alpha == 0;
}
