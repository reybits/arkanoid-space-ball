// Krakout2Game.h: interface for the CArkanoidSBGame class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_Krakout2GAME_H__92AE3841_D5BF_4537_A99B_BD3768D88454__INCLUDED_)
#define AFX_Krakout2GAME_H__92AE3841_D5BF_4537_A99B_BD3768D88454__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CArkanoidSBGame {
	friend class CGenerateLevel;
	friend class CBall;
	friend class CBullet;
	friend class CMonster;
public:
	enum { GS_GAME_ACTIVE, /*GS_GAME_RESTORE, */GS_GAME_PAUSED, GS_GET_READY, GS_GAME_OVER, GS_GAME_AREYOUSURE_EXIT, GS_GAME_REMINDER1, GS_GAME_REMINDER2 };
	enum _RACKET_TYPE { RT_NORMAL, RT_LASER, RT_MISSILE, RT_PLASMA, RT_MAGNET, RT_ENGINE };
	void SetPause();
	bool DrawReminder(int nReminderType);
	void AddScore(int nScore);
	void DoImpact(int nIndex, bool bRemoveAll);
	//void DoImpact2(int nBx, int nBy, int nBrick, bool bRemoveAll);
	void ProcessBonus(int nBonusType);
	void GetGameData(int &nScore, int &nLevel);
	bool DrawScreen();
	void InitNewGame(bool bIsCustomLevels);
	CArkanoidSBGame();
	virtual ~CArkanoidSBGame();
	void Unload();
	bool DrawGetReady();
	void AddGetReeadyInfo(const char *pchString);
	void RemoveOneLives();
	void DrawBackground();
	void RestoreGame();
	void SendEsc();
	void LoadBackground();
	void FreeBackground();

private:
	int m_nLaserX;
	std::vector<_BRICK>m_vecLevelBricks;

protected:
	void DrawPause();
	int m_nLevelPrev;	// store previous level number: -1 - usualy, -2 - inform, that we restore game
	struct _STARS {
		float	fX, fY, fSpeed;
		int	nType, nFrame;
	};
	std::vector<_STARS>m_vecStars;
	int m_nBonusLevelType;
	int m_nBonusLevelTicks;
	int m_nSndWall;	// store channel number for stop
	CGenerateLevel m_GenLev;
	Uint32 m_dwUnregisterdTime;
	void	_Cheater();
	int m_nScoreToAdditionalBall;
	Uint8 m_byChar;
	struct _BRICK_BULLET {
		int		nType;
		int		nAngle;
		int		nFrame;
		float		fX, fY;
	};
	std::vector<_BRICK_BULLET>m_vecBrickBullets;
	int m_nTotalBonusesInLevel;
	void ChangeBrick(int nIndex, Uint8 byToBrickType, bool bRemoveAll = false);
	int m_nCanMovePaddleCount;
	bool m_bPaddleIsInvert;
	int m_nPaddleInvertCount;
	bool m_bCanMovePaddle;
	int m_nCountSelfDestruct;
	bool m_bSelfDestructStarted;
	int m_nCountBackWall;
	bool m_bBackWall;
	int m_nAddNewMonsterCount;
	int m_nSlowDownCount;
	bool m_bSlowDown;
	void DoShoot();
	int m_nClockFrame;
	int m_nLives;
	void DrawStatistic();
	bool DoGameOver();
	void DoGameActive();
	int m_nGameState;
	float m_nRacketX;
	float m_nRacketY;
	int m_nRacketSize;
	int m_nRacketType;
	int m_nPaddleMissileCount;
	void DrawPaddle();
	void InitLevel(int nLevel, bool bRestore = false);
	void DoBomb(int nIndex);
	int m_nScore;
	int m_nBrickCount;
	void DrawBrick(int nIndex, bool bNextFrame, bool bNextFrameAnim);
	void DrawBricks();
	int m_nCurrentLevel;
	int m_nLevelImage;
   void ResetAll();
   void DrawBrickBullets();
   void MoveBrickBullets();
   int CalcBrickBulletsAngle(int nIndex, int nX, int nY);
	bool IsEmptyBrickPos(const int nSkipPos, const int nX, const int nY);
	void DrawAreYouSure();
	//void DrawRestoreGame();
	int m_nGetReadyScore;
	int m_nGetReadyMonsters;
	int m_nGetReadyBonuses;
	int m_nGetReadyBricks;
	int m_nGetReadyBallsLose;
	Uint32 m_dwCombosTime;
	int m_nCombosBricks;
	char *m_pchGetReeadyInfo;
    bool m_bTutorialPause;
    bool m_bIsCustomLevels;
};

#endif // !defined(AFX_Krakout2GAME_H__92AE3841_D5BF_4537_A99B_BD3768D88454__INCLUDED_)
