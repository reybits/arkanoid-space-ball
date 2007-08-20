// Monster.cpp: implementation of the CMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "monster.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMonster::CMonster() {
	m_nSndPatrol	= -1;
	m_vecMonster.reserve(20);
	m_vecMonster2.reserve(10);
	RemoveAll();
}

CMonster::~CMonster() {
}

void CMonster::RemoveAll() {
	StopSound(m_nSndPatrol);
	m_vecMonster.clear();
	m_vecMonster2.clear();
}

void CMonster::Draw() {
	SDL_Rect	rc;
	rc.y	= 0;
	static Uint32	dwTime	= 0;
	bool				bUpdate	= false;
	if(dwTime + 30 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		bUpdate	= true;
	}
	for(int i = 0; i < (int)m_vecMonster.size(); i++) {
		if(bUpdate == true && m_vecMonster[i].nCaptureMode == 0) {
			m_vecMonster[i].nFrame++;
			m_vecMonster[i].nFrame	%= m_vecMonster[i].nFramesCount;
		}
		rc.x	= m_vecMonster[i].nFrame * m_vecMonster[i].w;
		rc.w	= m_vecMonster[i].w;
		rc.h	= m_vecMonster[i].h;
		Blit((int)m_vecMonster[i].x, (int)m_vecMonster[i].y, m_vecMonster[i].pSurface, &rc);
	}
}

void CMonster::Draw2() {
	SDL_Rect	rc, rc2;
	rc.y	= 0;
	SetRect(&rc2, WALL_X1, WALL_Y1, SCREEN_WIDTH - WALL_X1, WALL_Y2 - WALL_Y1);
	SDL_SetClipRect(g_psurfScreen, &rc2);
	static Uint32	dwTime	= 0;
	bool				bUpdate	= false;
	if(dwTime + 50 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		bUpdate	= true;
	}
	for(int i = 0; i < (int)m_vecMonster2.size(); i++) {
		if(bUpdate == true) {
			m_vecMonster2[i].nFrame++;
			m_vecMonster2[i].nFrame	%= m_vecMonster2[i].nFramesCount;
		}
		rc.x	= m_vecMonster2[i].nFrame * m_vecMonster2[i].w;
		rc.w	= m_vecMonster2[i].w;
		rc.h	= m_vecMonster2[i].h;
		Blit((int)m_vecMonster2[i].x, (int)m_vecMonster2[i].y, m_vecMonster2[i].pSurface, &rc);
	}
	SDL_SetClipRect(g_psurfScreen, 0);
}

void CMonster::AddMonster(int x, int y, int nType) {
	if(nType >= 0 && nType < MONST_END) {
		_MONSTER	monster;
		monster.nType			= nType;
		monster.x				= (double)x;
		monster.y				= (double)y;
		monster.nFrame			= 0;
		monster.nCaptureMode	= 0;
		//monster.nCapturedBallIndex	= -1;
		monster.nAngle			= g_Rnd.Get(360);
		switch(nType) {
		case MONST_PATROL:
			if(m_nSndPatrol == -1 && g_Arkanoid.m_nGameState == CArkanoidSBGame::GS_GAME_ACTIVE)	m_nSndPatrol	= PlaySound(2);
			monster.nFramesCount	= 10;
			monster.pSurface		= m_pMonstPatrol;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_COPTER:
			monster.nFramesCount	= 5;
			monster.pSurface		= m_pMonstCopter;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_EYE:
			monster.nFramesCount	= 30;
			monster.pSurface		= m_pMonstEye;
			monster.w				= 32;
			monster.h				= 32;
			break;
		case MONST_BLACKHOLE:
			monster.nFramesCount	= 5;
			monster.pSurface		= m_pMonstBlackHole;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_FIGHTER:
			monster.nFramesCount	= 10;
			monster.pSurface		= m_pMonstFighter;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_HAND:
			monster.nFramesCount	= 30;
			monster.pSurface		= m_pMonstHand;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_TURBINE:
			monster.nFramesCount	= 20;
			monster.pSurface		= m_pMonstTurbine;
			monster.w				= 32;
			monster.h				= 32;
			break;
		case MONST_UFO:
			monster.nFramesCount	= 10;
			monster.pSurface		= m_pMonstUfo;
			monster.w				= 64;
			monster.h				= 48;
			break;
		case MONST_STONE1:
			monster.nFramesCount	= 30;
			monster.pSurface		= m_pMonstStone1;
			monster.w				= 32;
			monster.h				= 24;
			break;
		case MONST_STONE2:
			monster.nFramesCount	= 30;
			monster.pSurface		= m_pMonstStone2;
			monster.w				= 32;
			monster.h				= 24;
			break;
		}
		m_vecMonster.push_back(monster);
		g_TutorialDlg.AddDialog(int(monster.x + monster.w / 2), int(monster.y + monster.h / 2), 1, nType);
	}
}

void CMonster::AddMonster2() {
	_MONSTER	monster;
	monster.nType	= g_Rnd.Get(2);
	switch(monster.nType) {
	case M_WRECKAGE1:
		monster.nFramesCount	= 10;
		monster.pSurface		= m_pMonstWreckage1;
		monster.w				= 64;
		monster.h				= 48;
		break;
	case M_WRECKAGE2:
		monster.nFramesCount	= 30;
		monster.pSurface		= m_pMonstWreckage2;
		monster.w				= 64;
		monster.h				= 48;
		break;
	}
	monster.fSpeed	= 0.5 + g_Rnd.Get(10) / 5.0;
	monster.x		= -monster.w;
	monster.y		= WALL_Y1 + g_Rnd.Get(WALL_Y2 - WALL_Y1);
	monster.nFrame	= g_Rnd.Get(monster.nFramesCount);
	monster.nAngle	= 70 + g_Rnd.Get(40);
	m_vecMonster2.push_back(monster);
	//printf("added (total: %d)\n", m_vecMonster.size());
}

void CMonster::Move(bool bBackWall, int nPaddleY, int nPaddleHeight) {
	int	i;
	SDL_Rect	rc;
	m_bBackWall	= bBackWall;
	double	fSpeed	= g_fSpeedCorrection * 0.7;

	for(i = 0; i < (int)m_vecMonster.size(); i++) {
		if(m_vecMonster[i].nType == MONST_PATROL) {
			m_vecMonster[i].x	+= (2 * g_fSpeedCorrection);
			if(m_vecMonster[i].x > SCREEN_WIDTH) {
				RemoveByPos(i--);
			}
		}
		else if(m_vecMonster[i].nType == MONST_COPTER) {
			int	nAngle = 0, nPos = 0;
			double	fMinDist	= SCREEN_WIDTH;
			bool	bFindBall	= false;
			while(g_Ball.GetPositionAndDiameter(rc, nPos) == true) {
				int	nCatet1	= (rc.x + rc.w / 2) - ((int)m_vecMonster[i].x + 32);
				int	nCatet2	= (rc.y + rc.w / 2) - ((int)m_vecMonster[i].y + 24);
				double fDist		= sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
				if(fDist < fMinDist && rc.h != CBall::TYPE_BLUE) {
					bFindBall	= true;
					fMinDist	= fDist;
					nAngle	= (int)(57.3 * asin(nCatet2 / fDist));
					if(nCatet1 > 0)	nAngle	= (90 + nAngle) % 360;
					else					nAngle	= (270 - nAngle) % 360;
				}
			}
			if(bFindBall == true) {
				for(int nIterations = 0; nIterations < 2; nIterations++) {
					if(m_vecMonster[i].nAngle < nAngle) {
						if(abs(m_vecMonster[i].nAngle - nAngle) >= 180)	m_vecMonster[i].nAngle--;
						else								m_vecMonster[i].nAngle++;
					}
					else {
						if(abs(m_vecMonster[i].nAngle - nAngle) < 180)	m_vecMonster[i].nAngle--;
						else								m_vecMonster[i].nAngle++;
					}
					m_vecMonster[i].nAngle	+= 360;
					m_vecMonster[i].nAngle	%= 360;
					if(nAngle == m_vecMonster[i].nAngle)	break;
				}
				//m_vecMonster[i].nAngle	%= 360;
				m_vecMonster[i].x	+= (fSpeed * g_fSin[m_vecMonster[i].nAngle]);
				m_vecMonster[i].y	-= (fSpeed * g_fCos[m_vecMonster[i].nAngle]);
			}
		}
		else if(m_vecMonster[i].nType == MONST_HAND) {
			if(m_vecMonster[i].nCaptureMode == 0) {
				int	nAngle = 0, nPos = 0;
				double	fMinDist	= SCREEN_WIDTH;
				bool	bFindBall	= false;
				while(g_Ball.GetPositionAndDiameter(rc, nPos) == true) {
					int	nCatet1	= (rc.x + rc.w / 2) - ((int)m_vecMonster[i].x + 32);
					int	nCatet2	= (rc.y + rc.w / 2) - ((int)m_vecMonster[i].y + 24);
					double fDist		= sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
					if(fDist < 3) {
						m_vecMonster[i].nCaptureMode			= 1;
						m_vecMonster[i].nCapturedBallIndex	= nPos - 1;
						m_vecMonster[i].nFrame					= 4;
						m_vecMonster[i].nAngle					= 240 + g_Rnd.Get(60);
						g_Ball.BallCaptured(m_vecMonster[i].nCapturedBallIndex, true);
					}
					else if(fDist < fMinDist) {
						bFindBall	= true;
						fMinDist	= fDist;
						nAngle	= (int)(57.3 * asin(nCatet2 / fDist));
						if(nCatet1 > 0)	nAngle	= (90 + nAngle) % 360;
						else					nAngle	= (270 - nAngle) % 360;
					}
				}
				if(bFindBall == true) {
					for(int nIterations = 0; nIterations < 1 + g_fSpeedCorrection * 8; nIterations++) {
						if(m_vecMonster[i].nAngle < nAngle) {
							if(abs(m_vecMonster[i].nAngle - nAngle) >= 180)	m_vecMonster[i].nAngle--;
							else								m_vecMonster[i].nAngle++;
						}
						else {
							if(abs(m_vecMonster[i].nAngle - nAngle) < 180)	m_vecMonster[i].nAngle--;
							else								m_vecMonster[i].nAngle++;
						}
						m_vecMonster[i].nAngle	+= 360;
						m_vecMonster[i].nAngle	%= 360;
						if(nAngle == m_vecMonster[i].nAngle)	break;
					}
					double	fSpeed	= g_fSpeedCorrection * 2.5;
					m_vecMonster[i].x	+= (fSpeed * g_fSin[m_vecMonster[i].nAngle]);
					m_vecMonster[i].y	-= (fSpeed * g_fCos[m_vecMonster[i].nAngle]);
				}
			}
			else {
				m_vecMonster[i].x	+= (fSpeed * g_fSin[m_vecMonster[i].nAngle]);
				m_vecMonster[i].y	-= (fSpeed * g_fCos[m_vecMonster[i].nAngle]);
				ChangeMonsterAngle(i);
				if(m_vecMonster[i].nCaptureMode == 1) {
					g_Ball.SetCapturedBallPos(m_vecMonster[i].nCapturedBallIndex, m_vecMonster[i].x, m_vecMonster[i].y);
					if(abs((int)m_vecMonster[i].x - (RACKET_X - 140)) < 5) {
						g_Ball.BallCaptured(m_vecMonster[i].nCapturedBallIndex, false);
						m_vecMonster[i].nCaptureMode	= 2;
						m_vecMonster[i].nAngle			= 90;
						m_vecMonster[i].nFrame			= 0;
					}
				}
				else {
					if(m_vecMonster[i].x > SCREEN_WIDTH)	RemoveByPos(i--);
				}
			}
		}
		else if(m_vecMonster[i].nType == MONST_EYE) {
			if(m_vecMonster[i].x < RACKET_X - 60)
				m_vecMonster[i].x	+= fSpeed;
			if(m_vecMonster[i].y + m_vecMonster[i].h / 2 - 5 + g_Rnd.Get(10) < nPaddleY + nPaddleHeight / 2)
				m_vecMonster[i].y	+= fSpeed;
			else
				m_vecMonster[i].y	-= fSpeed;
		}
		else {
			m_vecMonster[i].x	+= (fSpeed * g_fSin[m_vecMonster[i].nAngle]);
			m_vecMonster[i].y	-= (fSpeed * g_fCos[m_vecMonster[i].nAngle]);
			if(m_vecMonster[i].nType == MONST_STONE1 || m_vecMonster[i].nType == MONST_STONE2) {
				if(m_vecMonster[i].x < WALL_X1 || m_vecMonster[i].x > (m_bBackWall == true ? WALL_X2 - m_vecMonster[i].w : SCREEN_WIDTH) ||
					m_vecMonster[i].y < WALL_Y1 || m_vecMonster[i].y + m_vecMonster[i].h > WALL_Y2) {
					if(m_vecMonster[i].x < SCREEN_WIDTH) {
						g_Exploision.AddExploision((int)m_vecMonster[i].x + (m_vecMonster[i].w - 45) / 2, (int)m_vecMonster[i].y + (m_vecMonster[i].h - 41) / 2, 0);
					}
					RemoveByPos(i--);
				}
			}
			else {
				ChangeMonsterAngle(i);
			}
		}
	}

	for(i = 0; i < (int)m_vecMonster2.size(); i++) {
		fSpeed	= g_fSpeedCorrection * m_vecMonster2[i].fSpeed;
		m_vecMonster2[i].x	+= fSpeed * g_fSin[m_vecMonster2[i].nAngle];
		m_vecMonster2[i].y	-= fSpeed * g_fCos[m_vecMonster2[i].nAngle];
		if(m_vecMonster2[i].x > SCREEN_WIDTH) {
			swap(m_vecMonster2[i], m_vecMonster2.back());
			m_vecMonster2.resize(m_vecMonster2.size() - 1);
			i--;
		}
	}
}

bool CMonster::IsAcross(int nX, int nY, int nWidth, int nHeight, bool bRemoveMonster) {
	bool	bIsAcross	= false;

	for(int i = 0; i < (int)m_vecMonster.size(); i++) {
		int	nRadius1	= (nWidth + nHeight) / 4;
		int	nRadius2	= (m_vecMonster[i].w - (m_vecMonster[i].nType == MONST_HAND ? 20 : 0) + m_vecMonster[i].h) / 4;
		int	nCatet1	= int((m_vecMonster[i].x + m_vecMonster[i].w / 2) - (nX + nWidth / 2));
		int	nCatet2	= int((m_vecMonster[i].y + m_vecMonster[i].h / 2) - (nY + nHeight / 2));
		if(sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2) <= nRadius1 + nRadius2) {
			bIsAcross	= true;
			g_Arkanoid.m_nGetReadyMonsters++;
			if(bRemoveMonster == true) {
				g_Exploision.AddExploision((int)m_vecMonster[i].x + (m_vecMonster[i].w - 45) / 2, (int)m_vecMonster[i].y + (m_vecMonster[i].h - 41) / 2, 0);
				RemoveByPos(i--);
			}
		}
	}

	return bIsAcross;
}

bool CMonster::IsAcrossPaddle(int nX, int nY, int nWidth, int nHeight) {
	bool	bIsAcross	= false;

	for(int i = 0; i < (int)m_vecMonster.size(); i++) {
		int	nRadius1	= (nWidth + nHeight) / 4;
		int	nRadius2	= (m_vecMonster[i].w - (m_vecMonster[i].nType == MONST_HAND ? 20 : 0) + m_vecMonster[i].h) / 4;
		int	nCatet1	= int((m_vecMonster[i].x + m_vecMonster[i].w / 2) - (nX + nWidth / 2));
		int	nCatet2	= int((m_vecMonster[i].y + m_vecMonster[i].h / 2) - (nY + nHeight / 2));
		if(sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2) <= nRadius1 + nRadius2) {
			bIsAcross	= true;
			g_Arkanoid.m_nGetReadyMonsters++;
			if(m_vecMonster[i].nType == MONST_STONE2) {
				g_Arkanoid.m_bCanMovePaddle		= false;
				g_Arkanoid.m_nCanMovePaddleCount	= 3;
			}
			else if(m_vecMonster[i].nType == MONST_PATROL || m_vecMonster[i].nType == MONST_STONE1) {
				for(int a = 0; a < 6; a++) {
					g_Exploision.AddExploision(nX - (45 - 12) / 2 + (10 - g_Rnd.Get(20)), nY - (41 - nHeight) / 2 + (nHeight / 2 - g_Rnd.Get(nHeight)), 0);
				}
				g_Arkanoid.AddGetReeadyInfo(m_vecMonster[i].nType == MONST_PATROL ? "Kamikaze impact paddle" : "Meteor impact paddle");
				g_Arkanoid.RemoveOneLives();
			}
			g_Exploision.AddExploision((int)m_vecMonster[i].x + (m_vecMonster[i].w - 45) / 2, (int)m_vecMonster[i].y + (m_vecMonster[i].h - 41) / 2, 0);
			RemoveByPos(i--);
		}
	}

	return bIsAcross;
}

bool CMonster::IsAcrossPaddle2(int nX, int nY, int nWidth, int nHeight) {
	bool	bIsAcross	= false;

	for(int i = 0; i < (int)m_vecMonster2.size(); i++) {
		int	nRadius1	= (nWidth + nHeight) / 4;
		int	nRadius2	= (m_vecMonster2[i].w + m_vecMonster2[i].h) / 4;
		int	nCatet1	= int((m_vecMonster2[i].x + m_vecMonster2[i].w / 2) - (nX + nWidth / 2));
		int	nCatet2	= int((m_vecMonster2[i].y + m_vecMonster2[i].h / 2) - (nY + nHeight / 2));
		if(sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2) <= nRadius1 + nRadius2) {
			bIsAcross	= true;
			for(int a = 0; a < 6; a++) {
				g_Exploision.AddExploision(nX - (45 - 12) / 2 + (10 - g_Rnd.Get(20)), nY - (41 - nHeight) / 2 + (nHeight / 2 - g_Rnd.Get(nHeight)), 0);
			}
			g_Exploision.AddExploision((int)m_vecMonster2[i].x + (m_vecMonster2[i].w - 45) / 2, (int)m_vecMonster2[i].y + (m_vecMonster2[i].h - 41) / 2, 0);
			swap(m_vecMonster2[i], m_vecMonster2.back());
			m_vecMonster2.resize(m_vecMonster2.size() - 1);
			i--;
		}
	}

	return bIsAcross;
}

void CMonster::ChangeMonsterAngle(int nPos) {
	if(m_vecMonster[nPos].nType == MONST_HAND && m_vecMonster[nPos].nCaptureMode == 2)
		return;
	m_vecMonster[nPos].nAngle	%= 360;
	if(m_vecMonster[nPos].x < WALL_X1) {
		if(m_vecMonster[nPos].nAngle >= 180 && m_vecMonster[nPos].nAngle < 270)
			m_vecMonster[nPos].nAngle	= 180 - (m_vecMonster[nPos].nAngle - 180);
		else if(m_vecMonster[nPos].nAngle >= 270 && m_vecMonster[nPos].nAngle < 360)
			m_vecMonster[nPos].nAngle	= 360 - m_vecMonster[nPos].nAngle;
	}
	else if(m_vecMonster[nPos].x + m_vecMonster[nPos].w > WALL_X2) {
		if(m_vecMonster[nPos].nAngle >= 0 && m_vecMonster[nPos].nAngle < 90)
			m_vecMonster[nPos].nAngle	= 360 - m_vecMonster[nPos].nAngle;
		else if(m_vecMonster[nPos].nAngle >= 90 && m_vecMonster[nPos].nAngle < 180)
			m_vecMonster[nPos].nAngle	= 180 + 180 - m_vecMonster[nPos].nAngle;
	}
	else if(m_vecMonster[nPos].y < WALL_Y1) {
		if(m_vecMonster[nPos].nAngle >= 0 && m_vecMonster[nPos].nAngle < 90)
			m_vecMonster[nPos].nAngle	= 180 - m_vecMonster[nPos].nAngle;
		else if(m_vecMonster[nPos].nAngle >= 270 && m_vecMonster[nPos].nAngle < 360)
			m_vecMonster[nPos].nAngle	= 180 + (360 - m_vecMonster[nPos].nAngle);
	}
	else if(m_vecMonster[nPos].y + m_vecMonster[nPos].h > WALL_Y2) {
		if(m_vecMonster[nPos].nAngle >= 90 && m_vecMonster[nPos].nAngle < 180)
			m_vecMonster[nPos].nAngle	= 180 - m_vecMonster[nPos].nAngle;
		else if(m_vecMonster[nPos].nAngle >= 180 && m_vecMonster[nPos].nAngle < 270)
			m_vecMonster[nPos].nAngle	= 360 - (m_vecMonster[nPos].nAngle - 180);
	}

	m_vecMonster[nPos].x	= max(m_vecMonster[nPos].x, WALL_X1);
	m_vecMonster[nPos].x	= min(m_vecMonster[nPos].x, WALL_X2 - m_vecMonster[nPos].w);
	m_vecMonster[nPos].y	= max(m_vecMonster[nPos].y, WALL_Y1);
	m_vecMonster[nPos].y	= min(m_vecMonster[nPos].y, WALL_Y2 - m_vecMonster[nPos].h);
}


bool CMonster::IsAcrossBall(SDL_Rect rc, bool bRemoveMonster) {
	bool	bIsAcross	= false;

	for(int i = 0; i < (int)m_vecMonster.size(); i++) {
		if(m_vecMonster[i].nType != MONST_HAND) {
			int	nRadius	= rc.w / 2;
			int	nCatet1	= int(rc.x + nRadius - (m_vecMonster[i].x + m_vecMonster[i].w / 2));
			int	nCatet2	= int(rc.y + nRadius - (m_vecMonster[i].y + m_vecMonster[i].h / 2));
			if(sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2) < nRadius + ((m_vecMonster[i].w + m_vecMonster[i].h) / 4)) {
				g_Arkanoid.AddScore(25);
				bIsAcross	= true;
				if(bRemoveMonster == true) {
					g_Arkanoid.m_nGetReadyMonsters++;
					g_Exploision.AddExploision((int)m_vecMonster[i].x + (m_vecMonster[i].w - 45) / 2, (int)m_vecMonster[i].y + (m_vecMonster[i].h - 41) / 2, 0);
					RemoveByPos(i--);
				}
			}
		}
	}

	return bIsAcross;
}


bool CMonster::IsAcrossBall2(SDL_Rect rc) {
	bool	bIsAcross	= false;

	for(int i = 0; i < (int)m_vecMonster2.size(); i++) {
		int	nRadius	= rc.w / 2;
		int	nCatet1	= int(rc.x + nRadius - (m_vecMonster2[i].x + m_vecMonster2[i].w / 2));
		int	nCatet2	= int(rc.y + nRadius - (m_vecMonster2[i].y + m_vecMonster2[i].h / 2));
		if(sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2) < nRadius + ((m_vecMonster2[i].w + m_vecMonster2[i].h) / 4)) {
			g_Arkanoid.AddScore(100);
			bIsAcross	= true;
			g_Arkanoid.m_nGetReadyMonsters++;
			g_Exploision.AddExploision((int)m_vecMonster2[i].x + (m_vecMonster2[i].w - 45) / 2, (int)m_vecMonster2[i].y + (m_vecMonster2[i].h - 41) / 2, 0);
			swap(m_vecMonster2[i], m_vecMonster2.back());
			m_vecMonster2.resize(m_vecMonster2.size() - 1);
			i--;
		}
	}

	return bIsAcross;
}


/*!
    \fn CMonster::RemoveByPos(int nPos)
 */
void CMonster::RemoveByPos(int nPos) {
	if(m_vecMonster[nPos].nType == MONST_PATROL) {
		StopSound(m_nSndPatrol);
	}
	else if(m_vecMonster[nPos].nType == MONST_HAND) {
		g_Ball.BallCaptured(m_vecMonster[nPos].nCapturedBallIndex, false);
	}
	swap(m_vecMonster[nPos], m_vecMonster.back());
	m_vecMonster.resize(m_vecMonster.size() - 1);
}
