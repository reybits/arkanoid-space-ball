// Ball.h: interface for the CBall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BALL_H__CA9466F6_E670_48F4_9FC1_437634D6E33F__INCLUDED_)
#define AFX_BALL_H__CA9466F6_E670_48F4_9FC1_437634D6E33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBall {
public:
	int IsThisBallOverObject(int nPos, int nX, int nY, int nWidth, int nHeight);
	bool RemoveOne(int &nX, int &nY);
	void ChangeAllBallsSpeed(bool bIncrease);
	void ChangeBallAngle(int nPos, bool bRotate);
	void SetAllBallsToBlue();
	void SetAllBallsToFire();
	void SplitBalls();
	bool GetPositionAndDiameter(SDL_Rect &rc, int &nPos);
	void ChangeBallSize(int nDelta);
	int GetBallsCount();
	bool StartBall(int nTop, int nBottom);
	int Move(bool bBackWall, SDL_Rect rcRacket, int nRacketType, int &nPaddleX);
	enum _TYPES { TYPE_WHITE, TYPE_RED, TYPE_BLUE };
	void AddBall(int x, int y);
	void AddBall(int x, int y, int nAngle);
	void RemoveAll();
	void Draw(int nPaddleType);
	CBall();
	virtual ~CBall();
	void ChangeAngle(int nPos, bool bIncrease);
	int GetAngle(int nPos);
	void SlowDown();
	int GetType();
   int GetTypeCount();
	void BallCaptured(int nIndex, bool bIsCaptured);
	void SetCapturedBallPos(int nIndex, float fX, float fY);

protected:
	void AddFBs(int nPos);
	bool m_bBackWall;
	int m_nType;
	int m_nPrevTypeCount;
	struct _BALL {
		float	x, y;
		int		nYoffset;
		int		nDiameter;
		int		nImpacts;
		float	nAngle;
		float	fSpeed, fOldSpeed;
		bool	bAlreadyImpact;
		Uint32	dwTimeCanAddFb;
		bool	bIsCaptured;	// is ball captured by monster HAND?
	};
	std::vector<_BALL>m_vecBalls;
	struct _FB {
		int	nType;
		int	nX, nY;
		int	nFrame;
	};
	std::vector<_FB>m_vecFBalls;
	std::vector<int>m_vecBrickIndex;
	void IncrementBallSpeed(int nPos);
	void ImpactWithWallAngle(int nPos);
   int CalcDiameter(int nIndex);
	int m_nBack;
};

#endif // !defined(AFX_BALL_H__CA9466F6_E670_48F4_9FC1_437634D6E33F__INCLUDED_)
