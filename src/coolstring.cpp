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
	char	achBuf[50];

	va_start(ap, pchFormat);
	vsprintf(achBuf, pchFormat, ap);
	va_end(ap);

	_COOL_STRING	cs;
	cs.pchString	= new char[strlen(achBuf) + 1];
	cs.nSeconds		= 2;	// 1/2 seconds to show
	strcpy(cs.pchString, achBuf);
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

	// draw frame
	SetRect(&rc, 0, 0, 160, 20);
	if(m_vecCoolString.size() > 0) {
		Blit(440, WALL_Y1, g_pTransp, &rc);
		g_FontTutorial.SetRect(440, WALL_Y1 + 2, 160, 20);
		g_FontTutorial.DrawString(0, 0, m_vecCoolString[0].pchString, 2);
		if(bStep == true && --m_vecCoolString[0].nSeconds == 0) {
			delete[]	m_vecCoolString[0].pchString;
			if(m_vecCoolString.size() > 1) {
				swap(m_vecCoolString[0], m_vecCoolString.back());
			}
			m_vecCoolString.resize(m_vecCoolString.size() - 1);
		}
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
