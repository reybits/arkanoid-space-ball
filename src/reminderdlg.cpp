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
#include "reminderdlg.h"

CReminderDlg::CReminderDlg() {
}


CReminderDlg::~CReminderDlg() {
}


/*!
    \fn CReminderDlg::Draw(int nType)
 */
int CReminderDlg::Draw(int nType) {
	char	achBuffer[260];
	SDL_Rect	rc;
	const int	nDx	= (SCREEN_WIDTH - 496) / 2;
	const int	nDy	= (SCREEN_HEIGHT - 261) / 2;

	EnableCursor(true);
	FadeScreen();

	SetRect(&rc, 0, 0, 496, 261);
	Blit(nDx, nDy, m_pUnregistered, &rc);

	switch(nType) {
	case 0:
		sprintf(achBuffer, "REMAINING TIME %d MIN!", (int)(g_nUnregisterdCount / 60));
		break;
	case 1:
		strcpy(achBuffer, "TRIAL TIME IS OUT!");
		break;
	case 2:
		strcpy(achBuffer, "REGISTER TO RESTORE GAME!");
		break;
	}
	g_Font2.DrawString(0, nDx + 65, achBuffer, 2);

	static int nFrame	= 0;
	static int nFrame2	= 0;
	static Uint32	dwTime	= 0;
	if(dwTime + 30 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		nFrame++;	nFrame	%= 10;
		nFrame2++;	nFrame2	%= 5;
	}
	SetRect(&rc, nFrame * 64, 0, 64, 48);
	Blit(nDx + 400, nDy + 180, m_pMonstPatrol, &rc);
	SetRect(&rc, nFrame2 * 64, 0, 64, 48);
	Blit(nDx + 400, nDy + 110, m_pMonstBlackHole, &rc);

	int	nWhichKey	= 0;
	// button 1
	if(g_nCursorX >= nDx + 124 && g_nCursorX <= nDx + 124 + 124 && g_nCursorY >= nDy + 212 && g_nCursorY <= nDy + 212 + 29) {
		SetRect(&rc, 0, 261, 124, 29);	// buy2
		nWhichKey	= 1;
	}
	else {
		SetRect(&rc, 0, 290, 124, 29);	// buy1
	}
	Blit(nDx + 124, nDy + 212, m_pUnregistered, &rc);

	// button 2
	if(g_nCursorX >= nDx + 248 && g_nCursorX <= nDx + 248 + 124 && g_nCursorY >= nDy + 212 && g_nCursorY <= nDy + 212 + 29) {
		if(nType != 1) {
			SetRect(&rc, 124, 261, 124, 29);	// continue2
		}
		else {
			SetRect(&rc, 248, 261, 124, 29);	// exit2
		}
		nWhichKey	= 2;
	}
	else {
		if(nType != 1) {
			SetRect(&rc, 124, 290, 124, 29);	// continue1
		}
		else {
			SetRect(&rc, 248, 290, 124, 29);	// exit1
		}
	}
	Blit(nDx + 248, nDy + 212, m_pUnregistered, &rc);

	if(g_bMouseLB == true && nWhichKey != 0) {
		g_bMouseLB	= false;
		switch(nWhichKey) {
		case 1:
			return 1;
		case 2:
			//EnableCursor(false);
			return 2;
		}
	}

	return -1;
}
