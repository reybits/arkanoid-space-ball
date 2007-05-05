/***************************************************************************
 *   Copyright (C) 2006 by Andrey A. Ugolnik                               *
 *   andrey@wegroup.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "arkanoidsb.h"
#include "coolstring.h"
#include "coolfont.hpp"

CCoolString::CCoolString() {
	m_vecCoolString.reserve(10);
}


CCoolString::~CCoolString() {
	Clear();
}

/*!
    \fn CCoolString::Add(const char *pchFormat, ...)
 */
void CCoolString::Add(const char *pchFormat, ...) {
	va_list ap;
	char	achBuf[30];//, byChar;

	va_start(ap, pchFormat);
	vsprintf(achBuf, pchFormat, ap);
	va_end(ap);

	_COOL_STRING	cs;
	cs.pchString	= new char[strlen(achBuf) + 1];
	cs.nSeconds		= 2;	// 1/2 seconds to show
	strcpy(cs.pchString, achBuf);
/*	cs.fDelta	= 4;
	cs.nLen		= strlen(achBuf);
	cs.fX		= (SCREEN_WIDTH - cs.nLen * cs.fDelta * 8) / 2;
	cs.fY		= (SCREEN_HEIGHT - cs.fDelta * 8) / 2;
	cs.nLen		= min(cs.nLen, COOLSTRING_MAX_LEN);
	cs.nPos		= 0;
	memset(cs.byPoint, 0, sizeof(cs.byPoint));

	for(int i = 0; i < cs.nLen; i++) {
		if(achBuf[i] >= '0' && achBuf[i] <= '9')
			byChar  = achBuf[i] - '0';
		else if(achBuf[i] == ':')
			byChar  = 10;
		else if(achBuf[i] == '!')
			byChar  = 11;
		else if(achBuf[i] >= 'A' && achBuf[i] <= 'Z')
			byChar  = 12 + achBuf[i] - 'A';
		else
			continue;
		for(int x = 0; x < 8; x++) {
			for(int y = 0; y < 8; y++) {
				switch(coolfont_raw[x + y * 8 + byChar * 64]) {
				case 0x00:
					cs.byPoint[x][y + 8 * i]	= 1;
					break;
				case 0x01:
					cs.byPoint[x][y + 8 * i]	= 2;
					break;
				}
			}
		}
	}*/
	m_vecCoolString.push_back(cs);
}


/*!
    \fn CCoolString::Draw()
 */
void CCoolString::Draw() {
	SDL_Rect	rc;
	static Uint32	dwTime	= 0;
	bool	bStep	= false;
	if(dwTime + 650 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		bStep	= true;
	}
/*	rc.w	= 4;
	rc.h	= 4;
	for(int i = 0; i < m_vecCoolString.size(); i++) {
		if(m_vecCoolString[i].nPos < 40 - 4) {
			if(bStep == true)	m_vecCoolString[i].nPos++;
		//}
		rc.y	= m_vecCoolString[i].nPos;
		//if(m_vecCoolString[i].fDelta < 10) {
			for(int x = 0; x < m_vecCoolString[i].nLen * 8; x++) {
				int	nX	= (int)(m_vecCoolString[i].fX + x * m_vecCoolString[i].fDelta);
				for(int y = 0; y < 8; y++) {
					int	nY	= (int)(m_vecCoolString[i].fY + y * m_vecCoolString[i].fDelta);
					switch(m_vecCoolString[i].byPoint[7 - y][x]) {
					case 1:
						rc.x	= 0;
						Blit(nX, nY, g_pCoolFont, &rc);
						break;
					case 2:
						rc.x	= 4;
						Blit(nX, nY, g_pCoolFont, &rc);
						break;
					}
				}
			}
			m_vecCoolString[i].fDelta	+= g_fSpeedCorrection * 0.01f;
			m_vecCoolString[i].fX		= (SCREEN_WIDTH - m_vecCoolString[i].nLen * m_vecCoolString[i].fDelta * 8) / 2;
			m_vecCoolString[i].fY		-= g_fSpeedCorrection;//(SCREEN_HEIGHT - m_vecCoolString[i].fDelta * 6) / 2;
		}
		else {
			swap(m_vecCoolString[i], m_vecCoolString.back());
			m_vecCoolString.resize(m_vecCoolString.size() - 1);
		}
	}*/

	// draw frame
	SetRect(&rc, 0, 0, 160, 20);
	if(m_vecCoolString.size() > 0) {
		Blit(440, WALL_Y1, g_pTransp, &rc);
		g_FontTutorial.SetRect(440, WALL_Y1 + 2, 160, 20);
		//for(int i = 0; i < m_vecCoolString.size(); i++) {
		g_FontTutorial.DrawString(0, 0, m_vecCoolString[0].pchString, 2);
			if(bStep == true && --m_vecCoolString[0].nSeconds <= 0) {
				delete[]	m_vecCoolString[0].pchString;
				swap(m_vecCoolString[0], m_vecCoolString.back());
				m_vecCoolString.resize(m_vecCoolString.size() - 1);
			}
		//	break;
		//}
		//g_Font.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}


/*!
    \fn CCoolString::Clear()
 */
void CCoolString::Clear() {
	for(int i = 0; i < m_vecCoolString.size(); i++) {
		delete[]	m_vecCoolString[i].pchString;
	}
	m_vecCoolString.clear();
}


/*!
    \fn CCoolString::GetCount()
 */
int CCoolString::GetCount() {
	return	m_vecCoolString.size();
}
