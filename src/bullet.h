// Bullet.h: interface for the CBullet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BULLET_H__EC7B9A82_2C3E_4B9F_901F_C27257812739__INCLUDED_)
#define AFX_BULLET_H__EC7B9A82_2C3E_4B9F_901F_C27257812739__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBullet {
public:
	bool GetPositionAndSize(SDL_Rect &rc, int& nPos, bool bFromFirst);
	void Move();
	bool IsAcross(int nX, int nY, int nWidth, int nHeight, bool &bRemoveAll, int &nType);
	enum _TYPES { TYPE_LASER, TYPE_MISSILE, TYPE_PLASMA };
	void AddBullets(int y, int nType);
	void RemoveAll();
	void Draw();
	CBullet();
	virtual ~CBullet();
	void ChangeAngle(int nPos, bool bRotate);

protected:
	int GetAngle(int nPos);
	void RemoveByPos(int nPos);
	struct _BULLET {
		int		nType;
		double		x, y;
		double		nAngle;
		int		w;	// laser related
		Uint32	dwLaser;
	};
	std::vector<_BULLET>m_vecBullets;
};

#endif // !defined(AFX_BULLET_H__EC7B9A82_2C3E_4B9F_901F_C27257812739__INCLUDED_)
