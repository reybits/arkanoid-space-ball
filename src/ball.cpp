// Ball.cpp: implementation of the CBall class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "ball.h"

const int INIT_ANGLE = 250;
const float INIT_BALL_SPEED = 1.5f;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBall::CBall() {
	m_nBack	= 0;
	m_vecBrickIndex.reserve(8);
	m_vecBalls.reserve(10);
	m_vecFBalls.reserve(200);
	RemoveAll();
}

CBall::~CBall() {
}

int CBall::Move(bool bBackWall, SDL_Rect rcRacket, int nRacketType, int &nPaddleX) {
	int	nBallsLose	= 0;
	m_bBackWall	= bBackWall;

	Uint32	dwTime	= SDL_GetTicks();

	static Uint32	dwBlueTime	= 0;
	if(dwBlueTime + 1000 < dwTime) {
		dwBlueTime	= dwTime;
		if(m_nType != TYPE_WHITE && --m_nPrevTypeCount == 0)	m_nType	= TYPE_WHITE;
	}
	static Uint32	dwTimeAddFb	= 0;
	bool				bAddFB		= false;
	if(dwTimeAddFb + 20 < dwTime) {
		dwTimeAddFb	= dwTime;
		bAddFB		= true;
	}


	// move balls
	for(size_t nPos = 0; nPos < m_vecBalls.size(); nPos++) {
		m_vecBalls[nPos].bAlreadyImpact	= false;
		if(m_vecBalls[nPos].bIsCaptured == true)	continue;

		if(m_vecBalls[nPos].fSpeed != 0) {
			if(bAddFB == true)	AddFBs(nPos);
			float	fSpeed		= m_vecBalls[nPos].fSpeed * g_fSpeedCorrection;
			m_vecBalls[nPos].x	+= fSpeed * g_fSin[GetAngle(nPos)];
			m_vecBalls[nPos].y	-= fSpeed * g_fCos[GetAngle(nPos)];
			//if(GetAngle(nPos) > 0 && GetAngle(nPos) < 180) {
				// ��� �������� �� ������������ � ������ ��������
				if(IsThisBallOverObject(nPos, rcRacket.x, rcRacket.y, 16, rcRacket.h) > 0) {
					nPaddleX	+= 5;
					PlaySound(11);
					if(nRacketType == CArkanoidSBGame::RT_MAGNET) {
						m_vecBalls[nPos].fOldSpeed	= m_vecBalls[nPos].fSpeed;
						m_vecBalls[nPos].fSpeed		= 0;
						m_vecBalls[nPos].nYoffset	= int(m_vecBalls[nPos].y - rcRacket.y);
					}
					m_vecBalls[nPos].nAngle	= (210 + (120.0 / rcRacket.h) * ((rcRacket.y + rcRacket.h) - (m_vecBalls[nPos].y + CalcDiameter(m_vecBalls[nPos].nDiameter) / 2)));
					if(m_vecBalls[nPos].nAngle < 210)
						m_vecBalls[nPos].nAngle	= 210;
					else if(m_vecBalls[nPos].nAngle > 329)
						m_vecBalls[nPos].nAngle	= 329;
				}
			//}

			if(m_vecBalls[nPos].bAlreadyImpact == false) {
				// calculate object size
				SDL_Rect	rc;
				memset(&rc, 0, sizeof(SDL_Rect));
				m_vecBrickIndex.clear();
				if(m_nType != TYPE_BLUE) {
					for(size_t nBrick = 0; nBrick < g_Arkanoid.m_vecLevelBricks.size(); nBrick++) {
						int	nX	= int(g_Arkanoid.m_vecLevelBricks[nBrick].fX);
						int	nY	= int(g_Arkanoid.m_vecLevelBricks[nBrick].fY);
						if(IsThisBallOverObject(nPos, nX, nY, BRICK_W, BRICK_H) > 0) {
							if(m_nType == TYPE_RED) {
								g_Arkanoid.DoImpact(nBrick, true);
								g_Exploision.AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
							}
							else {
								m_vecBrickIndex.push_back(nBrick);
								if(rc.x < nX)	rc.x	= nX;
								if(rc.y < nY)	rc.y	= nY;
								if(rc.w < nX + BRICK_W)	rc.w	= nX + BRICK_W;
								if(rc.h < nY + BRICK_H)	rc.h	= nY + BRICK_H;
							}
						}
					}
				}
				int	nIsOver;
				if(m_nType == TYPE_WHITE && (nIsOver = IsThisBallOverObject(nPos, rc.x, rc.y, rc.w - rc.x, rc.h - rc.y)) > 0) {
					//printf("%d x %d, %d x %d\n", rc.x, rc.y, rc.w - rc.x, rc.h - rc.y);
					m_vecBalls[nPos].bAlreadyImpact	= true;
					if(m_nType == TYPE_WHITE) {
						//printf("Brick change angle %d", GetAngle(nPos));
						const int	anAngles[8]	= { 180, 360, 360, 180, 540, 360, 360, 540 };
						int	nAngle180	= (GetAngle(nPos) + 180) % 360;
						//printf("Pos: %d - Angle: %d", nIsOver, GetAngle(nPos));
						m_vecBalls[nPos].nAngle	= anAngles[nIsOver - 1] - GetAngle(nPos);
						//printf(" -> %d%s\n", GetAngle(nPos), GetAngle(nPos) > 359 ? " !" : "");
						IncrementBallSpeed(nPos);
						//printf(" -> %d\n", GetAngle(nPos));
						m_nBack	= 1;
						while(IsThisBallOverObject(nPos, rc.x, rc.y, rc.w - rc.x, rc.h - rc.y) > 0) {
							m_vecBalls[nPos].x	+= (g_fSin[nAngle180]);
							m_vecBalls[nPos].y	-= (g_fCos[nAngle180]);
						}
						m_nBack	= 0;
						m_vecBalls[nPos].x	= int(m_vecBalls[nPos].x);
						m_vecBalls[nPos].y	= int(m_vecBalls[nPos].y);
						//printf("ball pos corrected (%.2d x %.2d)\n", (int)m_vecBalls[i].x, (int)m_vecBalls[i].y);

						for(size_t nBrickIndex = 0; nBrickIndex < m_vecBrickIndex.size(); nBrickIndex++) {
							g_Arkanoid.DoImpact(m_vecBrickIndex[nBrickIndex], false);
						}
					}
				}
			}

			ImpactWithWallAngle(nPos);

			if(m_vecBalls[nPos].x > SCREEN_WIDTH) {
				PlaySound(14);
				swap(m_vecBalls[nPos], m_vecBalls.back());
				m_vecBalls.resize(m_vecBalls.size() - 1);
				nPos--;
				nBallsLose++;
			}
		}
		else {
			m_vecBalls[nPos].y	= rcRacket.y + m_vecBalls[nPos].nYoffset;
			if((m_vecBalls[nPos].x + CalcDiameter(m_vecBalls[nPos].nDiameter)) > rcRacket.x)
				m_vecBalls[nPos].x	-= 2;
			if((m_vecBalls[nPos].x + CalcDiameter(m_vecBalls[nPos].nDiameter)) < rcRacket.x)
				m_vecBalls[nPos].x++;
		}
	}
	return	nBallsLose;
}

void CBall::Draw(int nPaddleType) {
	SDL_Rect	rc;

	Uint32	dwTime	= SDL_GetTicks();

	static Uint32	dwFrame	= 0;
	static int		nFrame	= 0;
	if(dwFrame + 50 < dwTime) {
		dwFrame	= dwTime;
		nFrame++;	nFrame	%= 10;
	}
// 	static Uint32	dwTimeAddFb	= 0;
// 	bool				bAddFB		= false;
// 	if(dwTimeAddFb + 20 < dwTime) {
// 		dwTimeAddFb	= dwTime;
// 		bAddFB		= true;
// 	}

	bool	bFirst	= true;
//	static int	nStep	= 0;
	for(size_t nPos = 0; nPos < m_vecBalls.size(); nPos++) {
// 		if(bAddFB == true)	AddFBs(nPos);

		// show vector
		if(nPaddleType == CArkanoidSBGame::RT_MAGNET && m_vecBalls[nPos].bIsCaptured == false && ((bFirst == true && m_vecBalls[nPos].fSpeed == 0) || m_vecBalls[nPos].fSpeed != 0)) {
			if(bFirst == true  && m_vecBalls[nPos].fSpeed == 0)	bFirst	= false;
			rc.y	= 0;	rc.w	= 4;	rc.h	= 4;
			int	nFrame2		= 0;
			float	x	= m_vecBalls[nPos].x + (CalcDiameter(m_vecBalls[nPos].nDiameter) - 4) / 2;
			float	y	= m_vecBalls[nPos].y + (CalcDiameter(m_vecBalls[nPos].nDiameter) - 4) / 2;
/*			x	+= g_fSin[GetAngle(nPos)] * nStep;
			y	-= g_fCos[GetAngle(nPos)] * nStep;
			nStep++;
			nStep	%= 15;*/
			do {
				rc.x	= nFrame2++ * 4;
				nFrame2	%= 5;
				Blit(int(x), int(y), m_pVector, &rc);
				x	+= g_fSin[GetAngle(nPos)] * 15;
				y	-= g_fCos[GetAngle(nPos)] * 15;
			} while(x > WALL_X1 && x + 4 < (m_bBackWall == true ? WALL_X2 : SCREEN_WIDTH) && y > WALL_Y1 && y + 4 < WALL_Y2);
		}

		switch(m_vecBalls[nPos].nDiameter) {
		case 0:	rc.y	= 0;	break;
		case 1:	rc.y	= 12;	break;
		case 2:	rc.y	= 32;	break;
		case 3:	rc.y	= 56;	break;
		case 4:	rc.y	= 88;	break;
		}
		rc.x	= nFrame * CalcDiameter(m_vecBalls[nPos].nDiameter);
		rc.y	+= m_nType * 128;
		rc.w	= rc.h	= CalcDiameter(m_vecBalls[nPos].nDiameter);
		Blit((int)m_vecBalls[nPos].x, (int)m_vecBalls[nPos].y, m_pBall, &rc);
	}

	static Uint32	dwFBtime		= 0;
	bool				bFBupdate	= false;
	if(dwFBtime + 50 < dwTime) {
		dwFBtime		= dwTime;
		bFBupdate	= true;
	}
	rc.w	= 12;	rc.h	= 12;
	for(size_t i = 0; i < m_vecFBalls.size(); i++) {
		rc.x	= 12 * m_nType;
		rc.y	= 12 * m_vecFBalls[i].nFrame;
		Blit(m_vecFBalls[i].nX, m_vecFBalls[i].nY, m_pFB, &rc);
		if(bFBupdate == true && ++m_vecFBalls[i].nFrame == 6) {
			swap(m_vecFBalls[i], m_vecFBalls.back());
			m_vecFBalls.resize(m_vecFBalls.size() - 1);
			i--;
		}
	}
}

void CBall::RemoveAll() {
	m_vecBalls.clear();
	m_vecFBalls.clear();
	m_nType				= TYPE_WHITE;
	m_nPrevTypeCount	= 0;
}

void CBall::AddBall(int x, int y) {
	_BALL	ball;
	ball.x				= (float)x;
	ball.y				= (float)y;
	ball.nYoffset		= y - (int)g_Arkanoid.m_nRacketY;
	ball.nDiameter		= 1;
	ball.fSpeed			= 0;
	ball.fOldSpeed		= -1;
	ball.nImpacts		= 0;
	ball.nAngle			= INIT_ANGLE;
	ball.dwTimeCanAddFb	= 0;
	ball.bAlreadyImpact	= false;
	ball.bIsCaptured	= false;
	m_vecBalls.push_back(ball);
}

void CBall::AddBall(int x, int y, int nAngle) {
	_BALL	ball;
	ball.x				= (float)x;
	ball.y				= (float)y;
	ball.nYoffset		= 0;
	ball.nDiameter		= 1;
	ball.fSpeed			= INIT_BALL_SPEED;
	ball.fOldSpeed		= -1;
	ball.nImpacts		= 0;
	ball.nAngle			= nAngle;
	ball.dwTimeCanAddFb	= 0;
	ball.bAlreadyImpact	= false;
	ball.bIsCaptured	= false;
	m_vecBalls.push_back(ball);
}

int CBall::IsThisBallOverObject(int nPos, int nX, int nY, int nWidth, int nHeight) {
	// calculate first angle
	int	nObjAngle	= int(57.3 * atan((float)nWidth / (float)nHeight));
	int	nObjXc	= nX + nWidth / 2;	// center of object
	int	nObjYc	= nY + nHeight / 2;
	int	nBallR	= CalcDiameter(m_vecBalls[nPos].nDiameter) / 2;	// ball radius
	int	nBallXc	= int(m_vecBalls[nPos].x + nBallR);	// ball center coords
	int	nBallYc	= int(m_vecBalls[nPos].y + nBallR);
	int	nCatet1	= nObjXc - nBallXc;
	int	nCatet2	= nObjYc - nBallYc;

	float	fDist1		= sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
	int		nAngle	= int(57.3 * asin(nCatet2 / fDist1));
	if(nCatet1 > 0)	nAngle	= 90 + nAngle;
	else			nAngle	= 270 - nAngle;
	nAngle	= ((360 + nAngle) % 360);

	int	nIsOver;
	float	fDist2;
	if(nAngle < nObjAngle) {
		fDist2	= (nHeight / 2) / g_fCos[nAngle];
		nIsOver	= 1;
	}
	else if(nAngle < 90) {
		fDist2	= (nWidth / 2) / g_fCos[90 - nAngle];
		nIsOver	= 2;
	}
	else if(nAngle < 180 - nObjAngle) {
		fDist2	= (nWidth / 2) / g_fCos[nAngle - 90];
		nIsOver	= 3;
	}
	else if(nAngle < 180) {
		fDist2	= (nHeight / 2) / g_fCos[180 - nAngle];
		nIsOver	= 4;
	}
	else if(nAngle < 180 + nObjAngle) {
		fDist2	= (nHeight / 2) / g_fCos[nAngle - 180];
		nIsOver	= 5;
	}
	else if(nAngle < 270) {
		fDist2	= (nWidth / 2) / g_fCos[270 - nAngle];
		nIsOver	= 6;
	}
	else if(nAngle < 360 - nObjAngle) {
		fDist2	= (nWidth / 2) / g_fCos[nAngle - 270];
		nIsOver	= 7;
	}
	else /*if(nAngle < 360) */{
		fDist2	= (nHeight / 2) / g_fCos[360 - nAngle];
		nIsOver	= 8;
	}

	if(fDist2 + nBallR + m_nBack > fDist1)	return	nIsOver;

	return	0;
}

bool CBall::StartBall(int nTop, int nBottom) {
	for(size_t i = 0; i < m_vecBalls.size(); i++) {
		if(m_vecBalls[i].fSpeed == 0) {
			if(m_vecBalls[i].fOldSpeed != -1) {
				m_vecBalls[i].fSpeed	= m_vecBalls[i].fOldSpeed;
			}
			else {
				m_vecBalls[i].fSpeed	= INIT_BALL_SPEED;
			}
			return	true;
		}
	}

	return	false;
}

void CBall::ImpactWithWallAngle(int nPos) {
	// ��������� ������������ ���� �� ������� � AddExploision
	if(m_vecBalls[nPos].x <= WALL_X1) {
		if(m_nType == TYPE_RED)
			g_Exploision.AddExploision(WALL_X1 - 16, (int)m_vecBalls[nPos].y + CalcDiameter(m_vecBalls[nPos].nDiameter) / 2 - 16, 0);
			//g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * m_vecBalls[nPos].x)));

		if((GetAngle(nPos) >= 180 && GetAngle(nPos) < 270) || (GetAngle(nPos) >= 270 && GetAngle(nPos) < 360)) {
			m_vecBalls[nPos].nAngle	= 360 - GetAngle(nPos);
		}
		IncrementBallSpeed(nPos);
	}
	else if(m_vecBalls[nPos].x + CalcDiameter(m_vecBalls[nPos].nDiameter) >= WALL_X2 && m_bBackWall == true) {
		if(m_nType == TYPE_RED)
			g_Exploision.AddExploision(WALL_X2 - 16, (int)m_vecBalls[nPos].y + CalcDiameter(m_vecBalls[nPos].nDiameter) / 2 - 16, 0);
		//g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * m_vecBalls[nPos].x)));

		if((GetAngle(nPos) >= 0 && GetAngle(nPos) < 90) || (GetAngle(nPos) >= 90 && GetAngle(nPos) < 180)) {
			m_vecBalls[nPos].nAngle	= 360 - GetAngle(nPos);
		}
		IncrementBallSpeed(nPos);
	}
	else if(m_vecBalls[nPos].y <= WALL_Y1) {
		if(m_nType == TYPE_RED)
			g_Exploision.AddExploision((int)m_vecBalls[nPos].x + CalcDiameter(m_vecBalls[nPos].nDiameter) / 2 - 16, WALL_Y1 - 16, 0);
		//g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * m_vecBalls[nPos].x)));

		if(GetAngle(nPos) >= 0 && GetAngle(nPos) < 90) {
			m_vecBalls[nPos].nAngle	= 180 - GetAngle(nPos);
		}
		else if(GetAngle(nPos) >= 270 && GetAngle(nPos) < 360) {
			m_vecBalls[nPos].nAngle	= 540 - GetAngle(nPos);
		}
		IncrementBallSpeed(nPos);
	}
	else if(m_vecBalls[nPos].y + CalcDiameter(m_vecBalls[nPos].nDiameter) >= WALL_Y2) {
		if(m_nType == TYPE_RED)
			g_Exploision.AddExploision((int)m_vecBalls[nPos].x + CalcDiameter(m_vecBalls[nPos].nDiameter) / 2 - 16, WALL_Y2 - 16, 0);
		//g_pMainFrame->m_pImix->SamplePlay(m_nSampleBallUp, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * m_vecBalls[nPos].x)));

		if(GetAngle(nPos) >= 90 && GetAngle(nPos) < 180) {
			m_vecBalls[nPos].nAngle	= 180 - GetAngle(nPos);
		}
		else if(GetAngle(nPos) >= 180 && GetAngle(nPos) < 270) {
			m_vecBalls[nPos].nAngle	= 540 - GetAngle(nPos);
		}
		IncrementBallSpeed(nPos);
	}

	m_vecBalls[nPos].x	= std::max<int>(m_vecBalls[nPos].x, WALL_X1);
	m_vecBalls[nPos].x	= std::min<int>(m_vecBalls[nPos].x, (m_bBackWall == true ? WALL_X2 - CalcDiameter(m_vecBalls[nPos].nDiameter) : SCREEN_WIDTH + 1));
	m_vecBalls[nPos].y	= std::max<int>(m_vecBalls[nPos].y, WALL_Y1);
	m_vecBalls[nPos].y	= std::min<int>(m_vecBalls[nPos].y, (WALL_Y2 - CalcDiameter(m_vecBalls[nPos].nDiameter)));
}

int CBall::GetBallsCount() {
	return	m_vecBalls.size();
}

void CBall::ChangeBallSize(int nDelta) {
	for(size_t i = 0; i < m_vecBalls.size(); i++) {
		m_vecBalls[i].nDiameter	+= nDelta;
		m_vecBalls[i].nDiameter	= std::min<int>(m_vecBalls[i].nDiameter, 4);
		m_vecBalls[i].nDiameter	= std::max<int>(m_vecBalls[i].nDiameter, 0);
	}
}

bool CBall::GetPositionAndDiameter(SDL_Rect &rc, int &nPos) {
	for(; (size_t)nPos < m_vecBalls.size(); nPos++) {
		if(m_vecBalls[nPos].bIsCaptured == false) {
			rc.x	= (int)m_vecBalls[nPos].x;
			rc.y	= (int)m_vecBalls[nPos].y;
			rc.w	= CalcDiameter(m_vecBalls[nPos].nDiameter);
			rc.h	= m_nType;
			nPos++;
			return	true;
		}
	}

	return	false;
}

void CBall::SetAllBallsToBlue() {
	m_nType				= TYPE_BLUE;
	m_nPrevTypeCount	= 8;
}

void CBall::SetAllBallsToFire() {
	m_nType				= TYPE_RED;
	m_nPrevTypeCount	= 8;
}

void CBall::SplitBalls() {
	std::vector<_BALL>tmp;
	tmp.reserve(m_vecBalls.size());
	for(size_t i = 0; i < m_vecBalls.size(); i++) {
		tmp.push_back(m_vecBalls[i]);
	}
	for(size_t i = 0; i < tmp.size(); i++) {
		tmp[i].nAngle		= ((int)tmp[i].nAngle + 45) % 360;
		tmp[i].bIsCaptured	= false;
		m_vecBalls.push_back(tmp[i]);
	}
}

void CBall::ChangeBallAngle(int nPos, bool bRotate) {
	if(m_vecBalls[nPos].fSpeed == 0.0)
		return;
	if(bRotate == false)
		m_vecBalls[nPos].nAngle	+= 360 - 90 + g_Rnd.Get(90);
	else
		m_vecBalls[nPos].nAngle	+= 45;//180;

	//m_vecBalls[nPos].bAlreadyImpact	= true;
}

void CBall::ChangeAllBallsSpeed(bool bIncrease) {
	for(size_t i = 0; i < m_vecBalls.size(); i++) {
		if(m_vecBalls[i].fSpeed != 0) {
			if(bIncrease == true) {
				if(m_vecBalls[i].fSpeed < INIT_BALL_SPEED + 5.0)
					m_vecBalls[i].fSpeed	+= .5;
			}
			else {
				if(m_vecBalls[i].fSpeed > INIT_BALL_SPEED)
					m_vecBalls[i].fSpeed	-= .5;
			}
		}
	}
}

bool CBall::RemoveOne(int &nX, int &nY) {
	if(m_vecBalls.size() > 0) {
		PlaySound(14);
		nX	= (int)m_vecBalls[0].x;
		nY	= (int)m_vecBalls[0].y;
		swap(m_vecBalls[0], m_vecBalls.back());
		m_vecBalls.resize(m_vecBalls.size() - 1);
		return	true;
	}

	return	false;
}

void CBall::IncrementBallSpeed(int nPos) {
	m_vecBalls[nPos].nImpacts++;
	if(m_vecBalls[nPos].nImpacts > 20) {
		m_vecBalls[nPos].nImpacts	= 0;
		if(m_vecBalls[nPos].fSpeed < INIT_BALL_SPEED + 5.0)
			m_vecBalls[nPos].fSpeed	+= .1;
	}
	m_vecBalls[nPos].nAngle	+= 360;
}


void CBall::AddFBs(int nPos) {
	int	nDiameter	= CalcDiameter(m_vecBalls[nPos].nDiameter);
	for(int i = 0; i < nDiameter / 6; i++) {
		int	nR		= nDiameter / 2 - 4;
		int	nXb	= (int)(m_vecBalls[nPos].x + nDiameter / 2);
		int	nYb	= (int)(m_vecBalls[nPos].y + nDiameter / 2);

		_FB	fb;
		fb.nType		= m_nType;
		fb.nX			= nXb + nR - g_Rnd.Get(nR * 2) - 6;
		fb.nY			= nYb + nR - g_Rnd.Get(nR * 2) - 6;
		fb.nFrame	= 0;
		m_vecFBalls.push_back(fb);
	}
}



/*!
    \fn CBall::ChangeAngle(int nPos)
 */
void CBall::ChangeAngle(int nPos, bool bIncrease) {
	if(m_vecBalls[nPos].fSpeed != 0.0) {
		float	fAngle	= 0.5 * g_fSpeedCorrection;
		if(bIncrease == true) {
			m_vecBalls[nPos].nAngle	+= fAngle;
		}
		else {
			m_vecBalls[nPos].nAngle	+= (360 - fAngle);
		}
		//if(m_vecBalls[nPos].nAngle > 359) m_vecBalls[nPos].nAngle	-= 359;
	}
}


/*!
    \fn CBall::GetAngle(int nPos)
 */
int CBall::GetAngle(int nPos) {
	return	(int)(m_vecBalls[nPos].nAngle + 360) % 360;
}


/*!
    \fn CBall::SlowDown()
 */
void CBall::SlowDown() {
	for(size_t i = 0; i < m_vecBalls.size(); i++) {
		if(m_vecBalls[i].fSpeed != 0.0)	m_vecBalls[i].fSpeed	= INIT_BALL_SPEED;
	}
}


/*!
    \fn CBall::CalcDiameter(int nIndex)
 */
int CBall::CalcDiameter(int nIndex) {
	const int	anDiameters[]	= { 12, 20, 24, 32, 40 };
	if(nIndex >= 0 && nIndex < 5) {
		return anDiameters[nIndex];
	}
	return 0;
}


/*!
    \fn CBall::GetType()
 */
int CBall::GetType() {
   return m_nType;
}


/*!
   \fn CBall::GetTypeCount()
   How many ticks to normal state
 */
int CBall::GetTypeCount() {
   return m_nPrevTypeCount;
}


/*!
   \fn CBall::BallCaptured(iny nIndex, bool bIsCaptured)
   bIsCaptured:
   true	- monster HAND capture ball, move that ball with monster
	false	- monster release ball - start it some direction
 */
void CBall::BallCaptured(int nIndex, bool bIsCaptured) {
	if(nIndex >= 0 && nIndex < (int)m_vecBalls.size()) {
		if(bIsCaptured == false && m_vecBalls[nIndex].bIsCaptured == true) {
			m_vecBalls[nIndex].fSpeed	+= 0.5;
			m_vecBalls[nIndex].nAngle	= 30 + g_Rnd.Get(120);
		}
		m_vecBalls[nIndex].bIsCaptured	= bIsCaptured;
	}
}


/*!
    \fn CBall::SetBallPos(int nIndex, float fX, float fY)
 */
void CBall::SetCapturedBallPos(int nIndex, float fX, float fY) {
	if(nIndex >= 0 && nIndex < (int)m_vecBalls.size() && m_vecBalls[nIndex].bIsCaptured == true) {
		int	nRadius	= CalcDiameter(m_vecBalls[nIndex].nDiameter) / 2;
		m_vecBalls[nIndex].x	= fX + 26 - nRadius;
		m_vecBalls[nIndex].y	= fY + 33 - nRadius;
	}
}
