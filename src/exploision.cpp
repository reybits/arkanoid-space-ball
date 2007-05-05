// Exploision.cpp: implementation of the CExploision class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "exploision.h"

#define	NUM_OF_FRAMES	 9
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExploision::CExploision() {
	m_vecExploisions.reserve(30);
}

CExploision::~CExploision() {
}

void CExploision::Draw(bool bBricks) {
	static Uint32	dwTime	= 0;
	static bool		bUpdate	= false;
	if(bBricks == true) {
		bUpdate	= false;
		if(dwTime + 80 < SDL_GetTicks()) {
			dwTime	= SDL_GetTicks();
			bUpdate	= true;
		}
	}
	SDL_Rect	rc;
	rc.w	= 45;	rc.h	= 41;
	for(int i = 0; i < m_vecExploisions.size(); i++) {
		if((bBricks == true && m_vecExploisions[i].nType == 3) || (bBricks == false && m_vecExploisions[i].nType != 3)) {
			rc.x	= 40 + m_vecExploisions[i].nType * 45;
			rc.y	= m_vecExploisions[i].nFrame * 41;
			Blit(m_vecExploisions[i].x, m_vecExploisions[i].y, m_pExploision, &rc);
			if(bUpdate == true && ++m_vecExploisions[i].nFrame == NUM_OF_FRAMES) {
				swap(m_vecExploisions[i], m_vecExploisions.back());
				m_vecExploisions.resize(m_vecExploisions.size() - 1);
				i--;
			}
		}
	}
}

void CExploision::AddExploision(int x, int y, int nType) {
	_EXPLOISION	expl;
	expl.nType		= nType;
	expl.x			= x;
	expl.y			= y;
	expl.nFrame		= 0;
	m_vecExploisions.push_back(expl);
	PlaySound(15);
}

void CExploision::RemoveAll() {
	m_vecExploisions.clear();
}

int CExploision::GetCount() {
	return	m_vecExploisions.size();
}
