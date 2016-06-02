// Bonus.cpp: implementation of the CBonus class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "bonus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBonus::CBonus() {
	m_vecBonuses.reserve(20);
	m_vecBonusesStack.reserve(20);
	RemoveAll(true);
}

CBonus::~CBonus() {
}

void CBonus::Move(int nRacketType) {
	bool	bNewPos	= false;
	static Uint32	dwPosTime	= 0;
	if(dwPosTime + 10 < SDL_GetTicks()) {
		dwPosTime	= SDL_GetTicks();
		bNewPos	= true;
	}

	for(size_t i = 0, size = m_vecBonuses.size(); i < size; )
    {
		m_vecBonuses[i].origx	+= (g_fSpeedCorrection * 0.4);
		m_vecBonuses[i].x	= (int)m_vecBonuses[i].origx;
		//m_vecBonuses[i].x	= int(m_vecBonuses[i].origx + g_fSin[m_vecBonuses[i].nSin] * m_vecBonuses[i].nRadius / 2);
		m_vecBonuses[i].y	= int(m_vecBonuses[i].origy + g_fCos[m_vecBonuses[i].nCos] * m_vecBonuses[i].nRadius);

		if(bNewPos == true)
		{
			//m_vecBonuses[i].nSin	+= 3;
			//m_vecBonuses[i].nSin	%= 360;
			m_vecBonuses[i].nCos	+= 3;
			m_vecBonuses[i].nCos	%= 360;

			if(m_vecBonuses[i].nRadius < 20)	m_vecBonuses[i].nRadius++;
		}

		if(m_vecBonuses[i].x > SCREEN_WIDTH)
		{
			//g_pMainFrame->m_pImix->SamplePlay(m_nSampleBonusDown, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * m_vecBonuses[i].x)));
			m_vecBonuses[i] = m_vecBonuses[--size];
			m_vecBonuses.pop_back();
			continue;
		}

		i++;
	}
}

void CBonus::Draw() {
	SDL_Rect	rc;
	bool	bNewFrame	= false;
	static int		nFrameStack	= 0;
	static Uint32	dwFrameTime	= 0;
	if(dwFrameTime + 60 < SDL_GetTicks()) {
		dwFrameTime	= SDL_GetTicks();
		nFrameStack++;	nFrameStack	%= 15;
		bNewFrame	= true;
	}

	for(size_t i = 0; i < m_vecBonuses.size(); i++) {
		int	nType	= m_vecBonuses[i].nType;
		if(bNewFrame == true) {
			m_vecBonuses[i].nFrame++;
			m_vecBonuses[i].nFrame	%= 15;
		}
		rc.x	= m_vecBonuses[i].nFrame * 36;
		rc.y	= nType * 36;
		rc.w	= rc.h	= 36;
		Blit(m_vecBonuses[i].x, m_vecBonuses[i].y, m_pBonuses, &rc);
		rc.y	= 0;	rc.w	= rc.h	= 60;
		if(nType == TYPE_BALL_BLUE || nType == TYPE_BALL_SHRINK || nType == TYPE_PADDLE_SHRINK) {
			rc.x	= 0;
			Blit(m_vecBonuses[i].x - 12, m_vecBonuses[i].y - 12, g_pBonusesAura, &rc);
		}
		else {
			rc.x	= 60;
			Blit(m_vecBonuses[i].x - 12, m_vecBonuses[i].y - 12, g_pBonusesAura, &rc);
		}
	}

	rc.w	= rc.h	= 24;
	int	nCount	= m_vecBonusesStack.size();
	bool	bFist2	= true;
	if(nCount > 0) {
		int	nCountOnScreen	= nCount > 2 ? 3 : nCount;
		m_nStackPos	%= nCount;
		int	nPos	= m_nStackPos - 1;
#define SMALL_BONUS_X	532
		int	nX	= SMALL_BONUS_X;
		if(nPos == -1)	nPos	= nCount - 1;
		do {
			if(nCount == 1) {
				nX	+= 24;
			}
			else if(nCount == 2 && m_nStackPos == 0 && bFist2 == true) {
				bFist2	= false;
				nX	+= 24;
				nPos	= 0;
			}
			if(nX == SMALL_BONUS_X + 24) {
				rc.x	= rc.y	= 0;
				Blit(nX, 445, g_pTransp, &rc);
				g_TutorialDlg.AddDialog(SMALL_BONUS_X + 24 + 12, 445, 0, 2);
			}
			rc.x	= nFrameStack * 24;
			rc.y	= m_vecBonusesStack[nPos] * 24;
			Blit(nX, 445 + (nX == SMALL_BONUS_X + 24 ? 0 : 6), g_pBonusesSmall, &rc);
			nX	+= 24;
			nPos++;	nPos	%= nCount;
		} while(--nCountOnScreen);
/*		g_FontTutorial.SetRect(SMALL_BONUS_X + 24, 470, 24, 80);
		g_FontTutorial.DrawString(0, 0, "test", 2);*/
	}
}

void CBonus::RemoveAll(bool bAndStackToo) {
	m_vecBonuses.clear();
	if(bAndStackToo == true) {
		m_nStackPos	= 0;
		m_vecBonusesStack.clear();
	}
}

void CBonus::AddBonus(int x, int y, int nType) {
	_BONUS	bonus;
	bonus.nType			= nType;
	bonus.nFrame		= g_Rnd.Get(10);
	bonus.nRadius		= 0;
	bonus.nSin			= 0;
	bonus.nCos			= 0;
	bonus.origx			= (float)x;
	bonus.origy			= (float)y;
	bonus.x				= x;
	bonus.y				= y;
	m_vecBonuses.push_back(bonus);
	g_TutorialDlg.AddDialog(bonus.x + 18, bonus.y + 18, 2, nType);
}

int CBonus::IsAcross(int nX, int nY, int nWidth, int nHeight)
{
    for(size_t i = 0, size = m_vecBonuses.size(); i < size; i++)
    {
        if(m_vecBonuses[i].x + 36 >= nX && m_vecBonuses[i].x <= nX + nWidth && m_vecBonuses[i].y + 36 >= nY && m_vecBonuses[i].y <= nY + nHeight)
        {
            int nType = m_vecBonuses[i].nType;
            m_vecBonuses[i] = m_vecBonuses[--size];
            m_vecBonuses.pop_back();
            return nType;
        }
    }

    return -1;
}

int CBonus::GetBonusesOnScreen() {
	return	m_vecBonuses.size();
}


/*!
    \fn CBonus::SetPosStack(bool bNext)
 */
void CBonus::SetPosStack(bool bNext) {
	int	nCount	= m_vecBonusesStack.size();
	if(nCount > 0) {
		if(bNext == true) {
			m_nStackPos++;	m_nStackPos	%= nCount;
		}
		else {
			m_nStackPos--;
			if(m_nStackPos < 0)	m_nStackPos	= nCount - 1;
		}
	}
	else {
		m_nStackPos	= 0;
	}
}


/*!
    \fn CBonus::AddToStack(int nType)
 */
void CBonus::AddToStack(int nType) {
   m_vecBonusesStack.push_back(nType);
}


/*!
    \fn CBonus::GetBonusFromStack()
 */
int CBonus::GetBonusFromStack()
{
    int	nCount = m_vecBonusesStack.size();
    if(nCount > 0)
    {
        int nType = m_vecBonusesStack[m_nStackPos];
        m_vecBonusesStack[m_nStackPos] = m_vecBonusesStack[m_vecBonusesStack.size() - 1];
        m_vecBonusesStack.pop_back();
        return nType;
    }
    return -1;
}


/*!
    \fn CBonus::GetCountInStack()
 */
int CBonus::GetCountInStack()
{
	return m_vecBonusesStack.size();
}


/*!
    \fn CBonus::GetTypeInStack()
 */
int CBonus::GetTypeInStack(int nPos) {
	//if(nPos >= 0 && nPos < m_vecBonusesStack.size())
		return m_vecBonusesStack[nPos];
	//return -1;
}
