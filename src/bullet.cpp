// Bullet.cpp: implementation of the CBullet class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "bullet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBullet::CBullet() {
	m_vecBullets.reserve(10);
}

CBullet::~CBullet() {
}

void CBullet::Move() {
	for(size_t i = 0; i < m_vecBullets.size(); i++) {
		if(m_vecBullets[i].nType != TYPE_LASER) {
			int	nAngle	= GetAngle(i);
			m_vecBullets[i].x	+= g_fSin[nAngle] * 2 * g_fSpeedCorrection;
			m_vecBullets[i].y	-= g_fCos[nAngle] * 2 * g_fSpeedCorrection;
			int	nX	= (int)m_vecBullets[i].x;
			int	nY	= (int)m_vecBullets[i].y;
			if(nX < WALL_X1 || nY < WALL_Y1 || nY + 20 > WALL_Y2) {
				if(m_vecBullets[i].nType == TYPE_MISSILE)
					g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
				else
					g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 1);
				RemoveByPos(i);
			}
		}
		else {
			if(m_vecBullets[i].dwLaser + 30 < SDL_GetTicks()) {
				RemoveByPos(i);
			}
		}
	}
}

void CBullet::Draw() {
	Uint32	dwTime	= SDL_GetTicks();
	static Uint32	dwFrame	= 0;
	static int		nFrame	= 0;
	if(dwFrame + 100 < dwTime) {
		dwFrame		= dwTime;
		nFrame++;	nFrame	%= 10;
	}

	SDL_Rect	rc;
	for(size_t i = 0; i < m_vecBullets.size(); i++) {
		switch(m_vecBullets[i].nType) {
		case TYPE_LASER:
			rc.x	= 0;
			rc.y	= 40;// + m_vecBullets[i].nFrameFlame * 20;
			rc.w	= m_vecBullets[i].w;
			rc.h	= 20;
			break;
		case TYPE_MISSILE:
			rc.x	= 54 * nFrame;
			rc.y	= 0;
			rc.w	= 54;
			rc.h	= 20;
			break;
		case TYPE_PLASMA:
			rc.x	= 54 * nFrame;
			rc.y	= 20;
			rc.w	= 54;
			rc.h	= 20;
			break;
		}
		Blit((int)m_vecBullets[i].x, (int)m_vecBullets[i].y, m_pBullet, &rc);
	}
}

void CBullet::RemoveAll() {
	m_vecBullets.clear();
}

void CBullet::AddBullets(int y, int nType) {
	_BULLET	bullet;
	bullet.nType			= nType;
	bullet.x					= nType != TYPE_LASER ? RACKET_X : g_Arkanoid.m_nLaserX;
	bullet.y					= y;
	bullet.nAngle			= 270;
	bullet.w					= RACKET_X - g_Arkanoid.m_nLaserX - 15 + 8;
	bullet.dwLaser			= SDL_GetTicks();
	m_vecBullets.push_back(bullet);
}

bool CBullet::IsAcross(int nX, int nY, int nWidth, int nHeight, bool &bRemoveAll, int &nType) {
	for(size_t i = 0; i < m_vecBullets.size(); i++) {
		if(m_vecBullets[i].nType == TYPE_LASER) {
			if(m_vecBullets[i].x + m_vecBullets[i].w >= nX && m_vecBullets[i].x <= nX + nWidth &&
						m_vecBullets[i].y + (20 - 2) / 2 + 2 >= nY && m_vecBullets[i].y + (20 - 2) / 2 <= nY + nHeight) {
				g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 2);
				//RemoveByPos(i);
				m_vecBullets[i].x++;	// do not allow to across again
				bRemoveAll	= false;
				nType			= m_vecBullets[i].nType;
				return	true;
			}
		}
		else {
			if(m_vecBullets[i].x + 54 >= nX && m_vecBullets[i].x <= nX + nWidth &&
						m_vecBullets[i].y + 20 >= nY && m_vecBullets[i].y <= nY + nHeight) {
				switch(m_vecBullets[i].nType) {
					case TYPE_MISSILE:
						g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
						RemoveByPos(i);
						bRemoveAll	= true;
						nType			= m_vecBullets[i].nType;
						break;
					case TYPE_PLASMA:
						g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 1);
						bRemoveAll	= true;
						nType			= m_vecBullets[i].nType;
						break;
				}
				return	true;
			}
		}
	}

	return	false;
}


bool CBullet::GetPositionAndSize(SDL_Rect &rc, int& nPos, bool bFromFirst) {
	if(bFromFirst == true) {
		nPos	= 0;
		return	true;
	}

	while(nPos < (int)m_vecBullets.size()) {
		rc.x	= (int)m_vecBullets[nPos].x;
		rc.y	= (int)m_vecBullets[nPos].y;
		// ��� � ��������� ������ � ������
		switch(m_vecBullets[nPos].nType) {
		case TYPE_MISSILE:	rc.w	= 46;		rc.h	= 20;	break;
		case TYPE_PLASMA:		rc.w	= 30;		rc.h	= 20;	break;
		case TYPE_LASER:		rc.y	+= (20 - 2) / 2;	rc.w	= m_vecBullets[nPos].w;		rc.h	= 2;	break;
		}
		nPos++;
		return	true;
	}

	return	false;
}

void CBullet::RemoveByPos(int nPos) {
	swap(m_vecBullets[nPos], m_vecBullets.back());
	m_vecBullets.resize(m_vecBullets.size() - 1);
}


/*!
    \fn CBullet::ChangeAngle(int nPos, bool bInc)
 */
void CBullet::ChangeAngle(int nPos, bool bRotate) {
	float	fAngle	= 0.5 * g_fSpeedCorrection;
	if(bRotate == false) {
		m_vecBullets[nPos].nAngle	+= (360 - fAngle);
	}
	else {
		m_vecBullets[nPos].nAngle	+= fAngle;
	}
}

int CBullet::GetAngle(int nPos) {
	return	(int)(m_vecBullets[nPos].nAngle + 360) % 360;
}
