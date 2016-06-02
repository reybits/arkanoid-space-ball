#include "mystring.h"
#include "arkanoidsb.h"

#include <cassert>

CMyString::CMyString()
{
    m_psurFont				= 0;
    m_psurFontShadow		= 0;
    m_bIsShadowAvailable	= false;
    m_nShadowDx				= 1;
    m_nShadowDy				= 1;
    m_nKerning				= 0;
    m_nLineHeight			= 0;
    m_WindowRect.x			= 0;
    m_WindowRect.y			= 0;
    m_WindowRect.w			= SCREEN_WIDTH;
    m_WindowRect.h			= SCREEN_HEIGHT;
    for(int i = 0; i < 360; i++)
    {
        m_afSin[i] = sinf((M_PI / 180.0f) * i);
    }
}

CMyString::~CMyString()
{
    Unload();
}

bool CMyString::LoadFont(const char* pchFontName, const char* pchFontIniName)
{
    m_psurFont = LoadImage(pchFontName);
    if(m_psurFont != nullptr)
    {
        return LoadProps(pchFontIniName);
    }

    return false;
}

bool CMyString::LoadFont(const char* pchFontName, const char* pchFontShadowName, const char* pchFontIniName)
{
    m_psurFontShadow = LoadImage(pchFontShadowName);
    if(m_psurFontShadow != nullptr)
    {
        m_bIsShadowAvailable = true;
        m_bDrawShadow = true;
    }

    return LoadFont(pchFontName, pchFontIniName);
}

void CMyString::SetShadowOffset(int nDx, int nDy)
{
    m_nShadowDx = nDx;
    m_nShadowDy = nDy;
}

void CMyString::EnableShadow(bool bEnable)
{
    m_bDrawShadow = bEnable;
}

Uint32 CMyString::GetFrameWidth(Uint32 index)
{
    if(index < m_frames.size())
    {
        return m_frames[index].w;
    }
    return 0;
}

void CMyString::DrawString(int nX, int nY, const char *pchString, int nAlign) {
	if(pchString == NULL)
		return;

	int	nYPos	= m_WindowRect.y + nY;
	float	nXPos	= GetXpos(nX, pchString, nAlign);

	Uint32	dwFrame		= 0;
	int		nFrameWidth	= 0;
	while(*pchString) {
		Uint8	bySymb	= *pchString++;
		if(bySymb == '\n') {
			nXPos	= GetXpos(nX, pchString, nAlign);
			nYPos	+= m_nLineHeight;
			continue;
		}
		if(nAlign == FONT_ALIGN_JUSTIFY && bySymb == ' ') {
			nXPos	+= m_fJustifyWidth;
			continue;
		}
		dwFrame		= m_anSymbIndex[bySymb];
		nFrameWidth	= GetFrameWidth(dwFrame);
		SDL_Rect	rc, rcShadow;
		rc.x	= (Sint16)nXPos;
		rc.y	= nYPos + m_anSymbDy[bySymb];
		if(m_bDrawShadow == true && m_bIsShadowAvailable == true) {
			rcShadow.x	= rc.x + m_nShadowDx;
			rcShadow.y	= rc.y + m_nShadowDy;
			SDL_BlitSurface(m_psurFontShadow, &m_frames[dwFrame], g_psurfScreen, &rcShadow);
		}
		SDL_BlitSurface(m_psurFont, &m_frames[dwFrame], g_psurfScreen, &rc);
		nXPos	+= nFrameWidth + m_nKerning;
	}
}

int CMyString::GetXpos(int nX, const char *pchString, int nAlign) {
	if(nAlign == FONT_ALIGN_RIGHT)
		return m_WindowRect.x + m_WindowRect.w - nX - GetStringWidth(pchString);
	else if(nAlign == FONT_ALIGN_CENTER)
		return m_WindowRect.x + (m_WindowRect.w - GetStringWidth(pchString)) / 2;
	else if(nAlign == FONT_ALIGN_LEFT)
		return m_WindowRect.x + nX;
	else {
		int	nSpaces;
		int	nWidth	= GetStringWidth2(pchString, nSpaces);
		m_fJustifyWidth	= (m_WindowRect.w - nWidth) / (float)nSpaces;
		return m_WindowRect.x + nX;
	}
}

int CMyString::GetWordWidth(const char *pchString, int &nCharsCount) {
	int	nStrWidth	= 0;
	nCharsCount		= 0;
	while(*pchString && *pchString != ' ' &&  *pchString != '\n') {
		nCharsCount++;
		nStrWidth	+= GetFrameWidth(m_anSymbIndex[(int)*pchString++]);
	}

	return	nStrWidth + m_nKerning * nCharsCount;
}

void CMyString::DrawString2(int nX, int nY, const char *pchString) {
	if(pchString == NULL)
		return;

	int nYPos	= m_WindowRect.y + nY;

	Uint32	dwFrame	= 0;
	int nFrameWidth	= 0;
	int nSpaceWidth	= GetFrameWidth(m_anSymbIndex[(int)' ']);

	while(*pchString) {
		char	*pchTmp	= (char*)pchString;
		int	nCharsWord, nCharsLine	= 0, nSpaces	= -1, nWidthWord, nWidthLine	= 0;
		while(*pchTmp) {
			nWidthWord	= GetWordWidth(pchTmp, nCharsWord);
			if(nWidthLine + nWidthWord + nSpaceWidth * nSpaces < m_WindowRect.w) {
				nWidthLine	+= nWidthWord;
				pchTmp		+= nCharsWord;
				nCharsLine	+= nCharsWord;
				nSpaces++;
				while(*pchTmp == ' ') pchTmp++;	// skip spaces
			}
			else break;
		}
		m_fJustifyWidth	= (m_WindowRect.w - nWidthLine) / (float)nSpaces;
		float	nXPos	= m_WindowRect.x + nX;
		while(nCharsLine) {
			Uint8	bySymb	= *pchString++;
			if(bySymb == ' ') {
				if(*pchTmp) nXPos	+= m_fJustifyWidth;
				else nXPos	+= nSpaceWidth;
				continue;
			}
			nCharsLine--;
			dwFrame		= m_anSymbIndex[bySymb];
			nFrameWidth	= GetFrameWidth(dwFrame);
			SDL_Rect	rc, rcShadow;
			rc.x	= (int)nXPos;
			rc.y	= nYPos + m_anSymbDy[bySymb];
			if(m_bDrawShadow == true && m_bIsShadowAvailable == true) {
				rcShadow.x	= rc.x + m_nShadowDx;
				rcShadow.y	= rc.y + m_nShadowDy;
				SDL_BlitSurface(m_psurFontShadow, &m_frames[dwFrame], g_psurfScreen, &rcShadow);
			}
			SDL_BlitSurface(m_psurFont, &m_frames[dwFrame], g_psurfScreen, &rc);
			nXPos	+= nFrameWidth + m_nKerning;
		}
		nYPos	+= m_nLineHeight;
		if(*pchString == '\n') pchString++;
		while(*pchString == ' ') pchString++;	// skip spaces
	}
}

void CMyString::DrawStringSinus(int nX, int nY, const char *pchString, int nAplituda, int nPos, int nAlign) {
	if(pchString == NULL)
		return;

	int	nYPos	= m_WindowRect.y + nY;
	int	nXPos	= GetXpos(nX, pchString, nAlign);

	Uint32	dwFrame		= 0;
	int		nFrameWidth	= 0;
	while(*pchString) {
		nPos	%= 360;
		Uint8	bySymb	= *pchString++;

		if(bySymb == '\n') {
			nXPos	= GetXpos(nX, pchString, nAlign);
			nYPos	+= m_nLineHeight;
			continue;
		}

		dwFrame		= m_anSymbIndex[bySymb];
		nFrameWidth	= GetFrameWidth(dwFrame);
		SDL_Rect	rc, rcShadow;
		rc.x	= nXPos;
		rc.y	= nYPos + (int)(m_afSin[nPos] * nAplituda) + m_anSymbDy[bySymb];
		if(m_bDrawShadow == true && m_bIsShadowAvailable == true) {
			rcShadow.x	= rc.x + m_nShadowDx;
			rcShadow.y	= rc.y + m_nShadowDy;
			SDL_BlitSurface(m_psurFontShadow, &m_frames[dwFrame], g_psurfScreen, &rcShadow);
		}
		SDL_BlitSurface(m_psurFont, &m_frames[dwFrame], g_psurfScreen, &rc);
		nPos	+= nFrameWidth;
		nXPos	+= nFrameWidth;
	}
}

void CMyString::DrawNumber(int nNum, int nX, int nY, int nAlign) {
	char	achBuf[20];
	sprintf(achBuf, "%d", nNum);
	DrawString(nX, nY, achBuf, nAlign);
}

int CMyString::GetStringWidth(const char *pchString) {
	int	nSymbolsCount	= 0;
	int	nStrWidth		= 0;
	Uint8	bySymb			= 0;

	while(*pchString && bySymb != '\n') {
		nSymbolsCount++;
		bySymb	= *pchString++;
		nStrWidth	+= GetFrameWidth(m_anSymbIndex[bySymb]);
	}

	return	nStrWidth + m_nKerning * nSymbolsCount;
}

int CMyString::GetStringWidth2(const char *pchString, int &nSpaces)
{
    int	nSymbolsCount	= 0;
    int	nStrWidth		= 0;
    Uint8	bySymb			= 0;

    nSpaces	= 0;
    while(*pchString && bySymb != '\n') {
        bySymb	= *pchString++;
        if(bySymb == ' ')	nSpaces++;
        else {
            nSymbolsCount++;
            nStrWidth	+= GetFrameWidth(m_anSymbIndex[bySymb]);
        }
    }

    return nStrWidth + m_nKerning * nSymbolsCount;
}

Uint32 CMyString::RegisterFrame(const SDL_Rect& rc)
{
    m_frames.push_back(rc);
    return m_frames.size();
}

void CMyString::Unload()
{
    if(m_psurFont != nullptr)
    {
        SDL_FreeSurface(m_psurFont);
        m_psurFont = nullptr;
    }
}

void CMyString::SetRect(int left, int top, int width, int height)
{
    m_WindowRect.x = left;
    m_WindowRect.y = top;
    m_WindowRect.w = width;
    m_WindowRect.h = height;
}

bool CMyString::LoadProps(const char* pchName)
{
	unsigned int nDataLen;
	char* pchIniData = (char*)g_Resource.GetDataAllocMem(pchName, nDataLen);
/*	if(!pchIniData) {
		printf("  trying to load from disk...\n");
		FILE	*pFile	= fopen(MakePath(pchName), "r");
		if(pFile != 0) {
			fseek(pFile, 0, SEEK_END);
			nDataLen	= ftell(pFile);
			fseek(pFile, 0, SEEK_SET);
			char	*pchIniData	= new char[nDataLen + 1];
			fread(pchIniData, nDataLen, 1, pFile);
			fclose(pFile);
		}
		else {
			printf("  %s\n", strerror(errno));
		}
	}*/
	if(pchIniData != 0) {
		memset(m_anSymbIndex, 0, sizeof(m_anSymbIndex));
		memset(m_anSymbDy, 0, sizeof(m_anSymbDy));

		SDL_Rect	rc;
		int	x, y, w, h;
		Uint8 bySymbName;

		char	*pchLine	= strtok(pchIniData, "\r\n");

		while(pchLine != NULL) {
			int	index, dummy, dy;
			if(sscanf(pchLine, "Char=\"%c\"; %d,%d,%d,%d; %d,%d,%d", &bySymbName, &x, &y, &w, &h, &dummy, &dummy, &dy) == 8) {
				rc.x	= x;	rc.y	= y;	rc.w	= w; rc.h	= h;
				m_anSymbIndex[bySymbName]	= (int)RegisterFrame(rc) - 1;
				m_anSymbDy[bySymbName]		= dy;
			}
			else if(sscanf(pchLine, "Char=%x; %d,%d,%d,%d; %d,%d,%d", &index, &x, &y, &w, &h, &dummy, &dummy, &dy) == 8) {
				if(index > 256)	index	-= 848;
				bySymbName	= index;
				rc.x	= x;	rc.y	= y;	rc.w	= w; rc.h	= h;
				m_anSymbIndex[bySymbName]	= (int)RegisterFrame(rc) - 1;
				m_anSymbDy[bySymbName]		= dy;
			}
			else if(sscanf(pchLine, "Height=%d", &dummy) == 1) {
				m_nLineHeight	= dummy;
			}

			pchLine	= strtok(NULL, "\r\n");
		}

		g_Resource.FreeMem((unsigned char*)pchIniData);
		printf(" done.\n");
		return	true;
	}

	return	false;
}

bool CMyString::LoadFont2(const char* pchFontName, int nSpace, int nKerning, const char* pchCharset)
{
    if(pchCharset == nullptr)
    {
        pchCharset = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    }

    m_nKerning = nKerning;
    m_psurFont = LoadImage(pchFontName);
    if(m_psurFont != nullptr)
    {
        memset(m_anSymbIndex, 0, sizeof(m_anSymbIndex));
        memset(m_anSymbDy, 0, sizeof(m_anSymbDy));
        m_nLineHeight = m_psurFont->h - 1;

        SDL_Rect rc{ 0, 1, 0, (Uint16)m_nLineHeight };
        int nXpos = 0;
        while(nXpos < m_psurFont->w)
        {
            if(isTransparent(nXpos))
            {
                rc.x = nXpos;
                while(nXpos < m_psurFont->w - 1 && isTransparent(nXpos))
                {
                    nXpos++;
                }
                rc.w = nXpos - rc.x;
                Uint8 bySymbName = *pchCharset++;
                m_anSymbIndex[bySymbName] = (int)RegisterFrame(rc) - 1;
            }
            nXpos++;
        }

        m_anSymbIndex[(int)' '] = (int)RegisterFrame({ 0, 0, (Uint16)nSpace, 0 }) - 1;

        return true;
    }

    return false;
}

bool CMyString::isTransparent(Sint32 x) const
{
    return isTransparent(m_psurFont, x, 0);
}

bool CMyString::isTransparent(SDL_Surface* pSurface, Sint32 X, Sint32 Y) const
{
    assert(pSurface != nullptr);
    assert(X >= 0 && X < pSurface->w);
    assert(pSurface->format->BytesPerPixel == 4);

    const Uint32* bmp = (Uint32*)pSurface->pixels;
    const Uint32 pixel = bmp[Y * pSurface->pitch / 4 + X];

    const Uint8 alpha = (pixel & pSurface->format->Amask) >> pSurface->format->Ashift;
    return alpha == 0;
}

