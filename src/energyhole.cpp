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
#include "energyhole.h"

CEnergyHole::CEnergyHole() {
	RemoveAll();
}


CEnergyHole::~CEnergyHole() {
}


/*!
    \fn CEnergyHole::RemoveAll()
 */
void CEnergyHole::RemoveAll() {
	m_vecEnergyHole.clear();
}

/*!
    \fn CEnergyHole::Draw()
 */
void CEnergyHole::Draw() {
	SDL_Rect	rc;
	rc.x	= 0;
	rc.w	=	rc.h	= 80;
	for(size_t i = 0; i < m_vecEnergyHole.size(); i++) {
		if(m_vecEnergyHole[i].nIsOver == false) {
			rc.y	= 0;
			Blit((Uint32)m_vecEnergyHole[i].x, (Uint32)m_vecEnergyHole[i].y, m_pEnergyHole, &rc);
		}
		else {
			rc.y	= 80;
			Blit((Uint32)m_vecEnergyHole[i].x, (Uint32)m_vecEnergyHole[i].y, m_pEnergyHole, &rc);
		}
	}
}

/*!
    \fn CEnergyHole::AddEnergyHole()
 */
void CEnergyHole::AddEnergyHole() {
	_ENERGYHOLE	hole;
	hole.x				= WALL_X1 + (double)g_Rnd.Get(WALL_X2 - WALL_X1 - 100);
	hole.y				= WALL_Y1 + (double)g_Rnd.Get(WALL_Y2 - WALL_Y1 - 100);
	hole.nFrame			= 0;
	hole.dwTime			= 0;
	hole.dwMoveTime	= 0;
	hole.nAngle			= g_Rnd.Get(360);
	m_vecEnergyHole.push_back(hole);
}

/*!
    \fn CEnergyHole::Move()
 */
void CEnergyHole::Move() {
	Uint32	dwTime	= SDL_GetTicks();

	for(size_t i = 0; i < m_vecEnergyHole.size(); i++) {
		double	fSpeed	= g_fSpeedCorrection * 0.2;

		if(m_vecEnergyHole[i].dwMoveTime + 5000 < dwTime) {	// change angle
			m_vecEnergyHole[i].dwMoveTime	= dwTime;
			m_vecEnergyHole[i].nAngle		+= g_Rnd.Get(90);
			m_vecEnergyHole[i].nAngle		%= 360;
		}
		m_vecEnergyHole[i].x	+= (fSpeed * g_fSin[m_vecEnergyHole[i].nAngle]);
		m_vecEnergyHole[i].y	-= (fSpeed * g_fCos[m_vecEnergyHole[i].nAngle]);

		m_vecEnergyHole[i].x	= min(RACKET_X - 80 - 60, m_vecEnergyHole[i].x);
		m_vecEnergyHole[i].x	= max(WALL_X1, m_vecEnergyHole[i].x);
		m_vecEnergyHole[i].y	= min(WALL_Y2 - 80, m_vecEnergyHole[i].y);
		m_vecEnergyHole[i].y	= max(WALL_Y1, m_vecEnergyHole[i].y);
		m_vecEnergyHole[i].nIsOver	= false;

		SDL_Rect	rc;
		// is ball over energy hole? correct ball angle then
		int nIndex = 0;
		while(g_Ball.GetPositionAndDiameter(rc, nIndex) == true) {
			int	nCatet1	= int((m_vecEnergyHole[i].x + 80/2) - (rc.x + rc.w / 2));
			int	nCatet2	= int((m_vecEnergyHole[i].y + 80/2) - (rc.y + rc.w / 2));
			double	fDist		= sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
			if(fDist < 80/2) {
				g_TutorialDlg.AddDialog(rc.x + rc.w / 2, rc.y + rc.w / 2, 0, 5);
				m_vecEnergyHole[i].nIsOver	= true;
				int	nAngle	= (int)(57.3 * asin(nCatet2 / fDist));
				if(nCatet1 > 0)	nAngle	= g_Ball.GetAngle(nIndex - 1) - (90 + nAngle) % 360;
				else			nAngle	= g_Ball.GetAngle(nIndex - 1) - (270 - nAngle) % 360;
				if(abs(nAngle) < 180)	g_Ball.ChangeAngle(nIndex - 1, nAngle < 0 ? true : false);
				else					g_Ball.ChangeAngle(nIndex - 1, nAngle < 0 ? false : true);
			}
		}
		// is bullet over energy hole? correct bullet angle then
		g_Bullet.GetPositionAndSize(rc, nIndex, true);
		while(g_Bullet.GetPositionAndSize(rc, nIndex, false) == true) {
			int	nYm		= rc.y + rc.h / 2;
			int	nCatet1	= int(m_vecEnergyHole[i].x + 80/2) - (rc.x + rc.w / 2);
			int	nCatet2	= int(m_vecEnergyHole[i].y + 80/2) - nYm;
			double	fDist		= sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
			if(fDist < 80/2) {
				g_TutorialDlg.AddDialog(rc.x + rc.w / 2, rc.y + rc.h / 2, 0, 6);
				m_vecEnergyHole[i].nIsOver	= true;
				if(nCatet2 > 0)	//if(nYe > nYm)
					g_Bullet.ChangeAngle(nIndex - 1, false);
				else
					g_Bullet.ChangeAngle(nIndex - 1, true);
			}
		}
	}
}
