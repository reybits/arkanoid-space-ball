// Monster.h: interface for the CMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTER_H__CCBBB237_B6DA_4D36_A5DC_17EC9BA84A4B__INCLUDED_)
#define AFX_MONSTER_H__CCBBB237_B6DA_4D36_A5DC_17EC9BA84A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMonster {
	friend class CArkanoidSBGame;
public:
	enum _MONST1 { MONST_PATROL, MONST_COPTER, MONST_EYE, MONST_BLACKHOLE, MONST_FIGHTER, MONST_HAND, MONST_TURBINE, MONST_UFO, MONST_STONE1, MONST_STONE2, MONST_END };
	enum _MONST2 { M_WRECKAGE1, M_WRECKAGE2 };
	bool IsAcrossBall(SDL_Rect rc, bool bRemoveMonster);
	bool IsAcrossBall2(SDL_Rect rc);
	bool IsAcrossPaddle(int nX, int nY, int nWidth, int nHeight);
	bool IsAcrossPaddle2(int nX, int nY, int nWidth, int nHeight);
	bool IsAcross(int nX, int nY, int nWidth, int nHeight, bool bRemoveMonster);
	void Move(bool bBackWall, int nPaddleY, int nPaddleHeight);
	void AddMonster(int x, int y, int nType);
	void AddMonster2();
	void Draw();
	void Draw2();
	void RemoveAll();
	CMonster();
	virtual ~CMonster();

protected:
	int m_nSndPatrol;
	bool m_bBackWall;
	struct _MONSTER {
		int	nType;
		double	x, y;
		int	w, h, nFrame, nFramesCount, nAngle;
		double	fSpeed;	// wreckage related
		SDL_Surface* pSurface;
		int	nCaptureMode;			// MONST_HAND: 0 - none, 1 - ball captured, 2 - ball released
		int	nCapturedBallIndex;	// MONST_HAND: captured ball index
	};
	void ChangeMonsterAngle(int nPos);
   void RemoveByPos(int nPos);
	std::vector<_MONSTER>m_vecMonster2;
private:
	std::vector<_MONSTER>m_vecMonster;
};

#endif // !defined(AFX_MONSTER_H__CCBBB237_B6DA_4D36_A5DC_17EC9BA84A4B__INCLUDED_)
