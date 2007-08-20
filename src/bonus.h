// Bonus.h: interface for the CBonus class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONUS_H__7CA3E36C_E48B_45D0_90E2_C70408596DF2__INCLUDED_)
#define AFX_BONUS_H__7CA3E36C_E48B_45D0_90E2_C70408596DF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBonus {
public:
	int GetBonusesOnScreen();
	void Move(int nRacketType);
	int IsAcross(int nX, int nY, int nWidth, int nHeight);
	enum _TYPES {
		TYPE_BALL_SPLIT, TYPE_BALL_FIRE, TYPE_BALL_BLUE, TYPE_BALL_EXPAND, TYPE_BALL_SHRINK,
		TYPE_PADDLE_MAGNET, TYPE_PADDLE_FIRE, TYPE_PADDLE_MISSILE, TYPE_PADDLE_LASER,
		TYPE_PADDLE_ENGINE, TYPE_PADDLE_EXPAND, TYPE_PADDLE_SHRINK,
		TYPE_WALL,
		TYPE_FREEZE,
		TYPE_DESTROY,
		TYPE_LAST_BONUS	// Only marker!
	};
	void AddBonus(int x, int y, int nType);
	void RemoveAll(bool bAndStackToo);
	void Draw();
	CBonus();
	virtual ~CBonus();
	void SetPosStack(bool bNext);
	void AddToStack(int nType);
	int GetBonusFromStack();
	int GetCountInStack();
	int GetTypeInStack(int nPos);

protected:
	struct _BONUS {
		int	nType;
		int	nFrame;
		int	nRadius;
		int	nSin, nCos;
		double	origx, origy;
		int	x, y;
	};
	std::vector<_BONUS>m_vecBonuses;
	std::vector<int>m_vecBonusesStack;
	int m_nStackPos;
};

#endif // !defined(AFX_BONUS_H__7CA3E36C_E48B_45D0_90E2_C70408596DF2__INCLUDED_)
