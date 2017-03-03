/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <SDL.h>
#include <vector>

class CMyString final
{
public:
    CMyString();
    ~CMyString();

    bool loadFont(const char* fontName, const char* fontIniName, const char* fontShadowName = nullptr);
    bool loadFont(const char* fontName, int space, int kerning, const char* charset);
    void Unload();

    void SetRect(int left, int top, int width, int height);

    enum class eAlign { Left, Right, Center, Justify };
    void DrawNumber(int nNum, int nX, int nY, eAlign align = eAlign::Left);

    void DrawStringSinus(int nX, int nY, const char* str, int nAplituda, int nPos, eAlign align = eAlign::Left);
    void DrawString(int nX, int nY, const char* str, eAlign align = eAlign::Left);
    void DrawString2(int nX, int nY, const char* str);

    int GetStringWidth(const char* str);
    int GetStringWidth(const char* str, int& spacesCount);

    void SetShadowOffset(int nDx = 1, int nDy = 1);
    void EnableShadow(bool bEnable = true);

private:
    int GetWordWidth(const char* str, int& nCharsCount);
    bool isTransparent(int x) const;
    int GetXpos(int nX, const char* str, eAlign align);
    Uint32 RegisterFrame(const SDL_Rect& rc);
    Uint32 GetFrameWidth(Uint32 index);
    bool LoadProps(const char* pchName);

private:
    SDL_Surface* m_fnt;
    SDL_Surface* m_shadow;

    int m_lineHeight;

    bool m_drawShadow;

    float m_fJustifyWidth;
    int m_kerning;
    int m_nShadowDx;
    int m_nShadowDy;

    int m_anSymbDy[256];
    uint32_t m_symbIndex[256];
    std::vector<SDL_Rect> m_frames;

    SDL_Rect m_winRect;
};
