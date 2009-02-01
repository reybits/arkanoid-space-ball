// MyString.cpp: implementation of the CMyString class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "mystring.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyString::CMyString() {
	m_psurFont				= 0;
	m_psurFontShadow		= 0;
	m_bIsShadowAvailable	= false;
	m_nShadowDx				= 1;
	m_nShadowDy				= 1;
	m_nKerning				= 0;
	m_nLineHeight			= 0;
	m_prcFrames				= 0;
	m_dwFrames				= 0;
	m_WindowRect.x			= 0;
	m_WindowRect.y			= 0;
	m_WindowRect.w			= SCREEN_WIDTH;
	m_WindowRect.h			= SCREEN_HEIGHT;
	const double	pi	= 3.1415926535;
	for(int i = 0; i < 360; i++) {
		m_afSin[i]	= (double)sin((pi / 180) * i);
	}
}

CMyString::~CMyString()	{
	Unload();
	delete[]	m_prcFrames;
}

bool CMyString::LoadFont(const char *pchFontName, const char *pchFontIniName) {
	m_psurFont	= LoadImage(pchFontName);
	if(m_psurFont != 0 && LoadProps(pchFontIniName) != false) {
		return	true;
	}

	return	false;
}

bool CMyString::LoadFont(const char *pchFontName, const char *pchFontShadowName, const char *pchFontIniName) {
	m_psurFont			= LoadImage(pchFontName);
	m_psurFontShadow	= LoadImage(pchFontShadowName);
	if(m_psurFontShadow != NULL) {
			m_bIsShadowAvailable	= true;
			m_bDrawShadow			= true;
	}
	if(m_psurFont != 0 && LoadProps(pchFontIniName) != false) {
		return	true;
	}

	return	false;
}
void CMyString::SetShadowOffset(int nDx, int nDy) {
	m_nShadowDx	= nDx;
	m_nShadowDy	= nDy;
}

void CMyString::EnableShadow(bool bEnable) {
	m_bDrawShadow	= bEnable;
}

Uint32 CMyString::GetFrameWidth(Uint32 dwIndex) {
    if(dwIndex >= m_dwFrames)
		return	0;

	return	m_prcFrames[dwIndex].w;
}

void CMyString::DrawString(int nX, int nY, const char *pchString, int nAlign) {
	if(pchString == NULL)
		return;

	int	nYPos	= m_WindowRect.y + nY;
	double	nXPos	= GetXpos(nX, pchString, nAlign);

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
			SDL_BlitSurface(m_psurFontShadow, &m_prcFrames[dwFrame], g_psurfScreen, &rcShadow);
		}
		SDL_BlitSurface(m_psurFont, &m_prcFrames[dwFrame], g_psurfScreen, &rc);
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
		m_fJustifyWidth	= (m_WindowRect.w - nWidth) / (double)nSpaces;
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
		m_fJustifyWidth	= (m_WindowRect.w - nWidthLine) / (double)nSpaces;
		double	nXPos	= m_WindowRect.x + nX;
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
				SDL_BlitSurface(m_psurFontShadow, &m_prcFrames[dwFrame], g_psurfScreen, &rcShadow);
			}
			SDL_BlitSurface(m_psurFont, &m_prcFrames[dwFrame], g_psurfScreen, &rc);
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
			SDL_BlitSurface(m_psurFontShadow, &m_prcFrames[dwFrame], g_psurfScreen, &rcShadow);
		}
		SDL_BlitSurface(m_psurFont, &m_prcFrames[dwFrame], g_psurfScreen, &rc);
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

int CMyString::GetStringWidth2(const char *pchString, int &nSpaces) {
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

	return	nStrWidth + m_nKerning * nSymbolsCount;
}

Uint32 CMyString::RegisterFrame(SDL_Rect &rc) {
	SDL_Rect	*pNewFrames	= new SDL_Rect[m_dwFrames + 1];
	if(m_dwFrames && m_prcFrames) {
		memcpy(pNewFrames, m_prcFrames, sizeof(SDL_Rect) * m_dwFrames);
		delete[]	m_prcFrames;
	}

	m_prcFrames	= pNewFrames;
	m_prcFrames[m_dwFrames++]	= rc;

	return	m_dwFrames;	// return total frames
}

void CMyString::Unload() {
	if(m_psurFont != 0)	SDL_FreeSurface(m_psurFont);
	m_psurFont	= 0;
}

void CMyString::SetRect(int left, int top, int width, int height) {
	m_WindowRect.x	= left;
	m_WindowRect.y	= top;
	m_WindowRect.w	= width;
	m_WindowRect.h	= height;
}


/*!
    \fn CMyString::LoadProps(const char *pchName)
 */
bool CMyString::LoadProps(const char *pchName) {
	unsigned int	nDataLen;
	char				*pchIniData	= (char*)g_Resource.GetDataAllocMem(pchName, nDataLen);
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
		Uint8	bySymbName;

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


/*!
    \fn CMyString::LoadFont(const char *pchFontName)
 */
bool CMyString::LoadFont2(const char *pchFontName, int nSpace, int nKerning, const char *pchCharset) {
	const char	*pchCharsetDef	= { "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };
	if(pchCharset == 0) {
		pchCharset	= pchCharsetDef;
	}
	m_nKerning	= nKerning;
	m_psurFont	= LoadImage(pchFontName);
	if(m_psurFont != 0) {
		memset(m_anSymbIndex, 0, sizeof(m_anSymbIndex));
		memset(m_anSymbDy, 0, sizeof(m_anSymbDy));
		SDL_Rect	rc;
		rc.y	= 1;
		m_nLineHeight	= m_psurFont->h - 1;
		rc.h	= m_nLineHeight;
		int	nXpos	= 0;
		while(nXpos < m_psurFont->w) {
			if(_DoStartNewChar(nXpos) == false) {
				rc.x	= nXpos;
				while(nXpos < m_psurFont->w - 1 && _DoStartNewChar(nXpos) == false)	nXpos++;
				rc.w	= nXpos - rc.x;
				Uint8	bySymbName	= *pchCharset++;
				m_anSymbIndex[bySymbName]	= (int)RegisterFrame(rc) - 1;
			}
			nXpos++;
		}
		rc.x	= rc.y	= rc.h	= 0;	rc.w	= nSpace;
		m_anSymbIndex[(int)' ']	= (int)RegisterFrame(rc) - 1;
		return	true;
	}

	return	false;
}

bool CMyString::_DoStartNewChar(Sint32 x) {
	return _GetPixel(m_psurFont, x, 0) == SDL_MapRGB(m_psurFont->format, 255, 0, 255);
}

Uint32 CMyString::_GetPixel(SDL_Surface *pSurface, Sint32 X, Sint32 Y) {
	if(!pSurface) {
		fprintf(stderr, "SoFontGetPixel: No surface!\n");
		return 0;
	}
	if((X < 0) || (X >= pSurface->w)) {
		fprintf(stderr, "SoFontGetPixel: X (%d) out of range!\n", X);
		return 0;
	}

	Uint32	dwBpp		= pSurface->format->BytesPerPixel;
	Uint8		*byBits	= ((Uint8 *) pSurface->pixels) + Y * pSurface->pitch + X * dwBpp;

	switch(dwBpp) {
	case 1:
		return *((Uint8 *) pSurface->pixels + Y * pSurface->pitch + X);
		break;
	case 2:
		return *((Uint16 *) pSurface->pixels + Y * pSurface->pitch / 2 + X);
		break;
	case 3:
		// Format/endian independent
		Uint8 r, g, b;
		r	= *((byBits) + pSurface->format->Rshift / 8);
		g	= *((byBits) + pSurface->format->Gshift / 8);
		b	= *((byBits) + pSurface->format->Bshift / 8);
		return SDL_MapRGB(pSurface->format, r, g, b);
		break;
	case 4:
		return *((Uint32 *) pSurface->pixels + Y * pSurface->pitch / 4 + X);
		break;
	}
	fprintf(stderr, "SoFontGetPixel: Unsupported pixel format!\n");
	return 0;       // David (to get rid of warning)
}
