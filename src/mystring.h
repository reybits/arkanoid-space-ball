// MyString.h: interface for the CMyString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSTRING_H__335D7BEA_C9A6_4C30_98D8_164B1B1B2114__INCLUDED_)
#define AFX_MYSTRING_H__335D7BEA_C9A6_4C30_98D8_164B1B1B2114__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyString {
public:
	bool LoadFont2(const char *pchFontName, int nSpace, int nKerning = 0, const char *pchCharset = 0);
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
	CMyString();
	virtual ~CMyString();

protected:
	int GetWordWidth(const char *pchString, int &nCharsCount);
	double m_fJustifyWidth;
	int m_nKerning;
	Uint32 _GetPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y);
	bool _DoStartNewChar(Sint32 x);
	int m_nShadowDx, m_nShadowDy;
	bool m_bIsShadowAvailable;
	bool m_bDrawShadow;
	int GetXpos(int nX, const char *pchString, int nAlign);
	Uint32 RegisterFrame(SDL_Rect &rc);
	Uint32 GetFrameWidth(Uint32 dwIndex);
	bool LoadProps(const char *pchName);
	SDL_Surface *m_psurFont;
	SDL_Surface *m_psurFontShadow;
	double m_afSin[360];
	int m_nLineHeight;
	SDL_Rect *m_prcFrames;
	SDL_Rect m_WindowRect;
	Uint32 m_dwFrames;
	int m_anSymbDy[256];
	int m_anSymbIndex[256];
};

#endif // !defined(AFX_MYSTRING_H__335D7BEA_C9A6_4C30_98D8_164B1B1B2114__INCLUDED_)
