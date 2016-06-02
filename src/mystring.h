#pragma once

#include <SDL.h>
#include <vector>

class CMyString final
{
public:
    CMyString();
    ~CMyString();

    bool LoadFont2(const char* pchFontName, int nSpace, int nKerning, const char* pchCharset);
    void SetRect(int left, int top, int width, int height);
    void Unload();
    enum { FONT_ALIGN_LEFT, FONT_ALIGN_RIGHT, FONT_ALIGN_CENTER, FONT_ALIGN_JUSTIFY };
    void DrawNumber(int nNum, int nX, int nY, int nAlign = FONT_ALIGN_LEFT);
    int GetStringWidth(const char *pchString);
    int GetStringWidth2(const char *pchString, int &nSpaces);
    void DrawStringSinus(int nX, int nY, const char *pchString, int nAplituda, int nPos, int nAlign = FONT_ALIGN_LEFT);
    void DrawString(int nX, int nY, const char *pchString, int nAlign = FONT_ALIGN_LEFT);
    void DrawString2(int nX, int nY, const char *pchString);
    bool LoadFont(const char *pchFontName, const char *pchFontShadowName, const char *pchFontIniName);
    bool LoadFont(const char *pchFontName, const char *pchFontIniName);
    void SetShadowOffset(int nDx = 1, int nDy = 1);
    void EnableShadow(bool bEnable = true);

private:
    int GetWordWidth(const char *pchString, int &nCharsCount);
    bool isTransparent(Sint32 x) const;
    int GetXpos(int nX, const char* pchString, int nAlign);
    Uint32 RegisterFrame(const SDL_Rect& rc);
    Uint32 GetFrameWidth(Uint32 index);
    bool LoadProps(const char *pchName);

private:
    float m_fJustifyWidth;
    int m_nKerning;
    int m_nShadowDx, m_nShadowDy;
    bool m_bIsShadowAvailable;
    bool m_bDrawShadow;
    SDL_Surface* m_psurFont;
    SDL_Surface* m_psurFontShadow;
    float m_afSin[360];
    int m_nLineHeight;
    std::vector<SDL_Rect> m_frames;
    SDL_Rect m_WindowRect;
    int m_anSymbDy[256];
    int m_anSymbIndex[256];
};

