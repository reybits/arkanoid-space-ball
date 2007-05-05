// Exploision.h: interface for the CExploision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXPLOISION_H__4DE8944D_5255_46A8_B55C_A8FD5124F15B__INCLUDED_)
#define AFX_EXPLOISION_H__4DE8944D_5255_46A8_B55C_A8FD5124F15B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExploision {
public:
	int GetCount();
	void RemoveAll();
	void AddExploision(int x, int y, int nType);
	void Draw(bool bBricks);
	CExploision();
	virtual ~CExploision();

protected:
	struct _EXPLOISION {
		int		x, y;
		int		nType;
		int		nFrame;
	};
	std::vector<_EXPLOISION>m_vecExploisions;
};

#endif // !defined(AFX_EXPLOISION_H__4DE8944D_5255_46A8_B55C_A8FD5124F15B__INCLUDED_)
