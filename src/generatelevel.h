// GenerateLevel.h: interface for the CGenerateLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERATELEVEL_H__8C33A485_AE94_4D18_8E89_0D465E446B4C__INCLUDED_)
#define AFX_GENERATELEVEL_H__8C33A485_AE94_4D18_8E89_0D465E446B4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGenerateLevel {
public:
	void LoadPatterns();
	void Generate(int nLevel, bool bUseCustom);
	CGenerateLevel();
	virtual ~CGenerateLevel();

protected:
	Uint8 *m_pbyLevelPatterns;
	int m_nPatterns;
	bool m_bClonedByHorizontal;
	bool m_bClonedByVertical;
	enum { CLONE_VERTICAL, CLONE_HORIZONTAL };
	void _CloneLevelBy(int nType);
	int m_anPrevPatterns[10];
	bool _IsPatternExist(int nPattern);
	void _FillLevel(int nLevel, int nPos);
	void _RollVertical();
	void _RollHorizontal();
	int _CalculateBricksCount();
	void _RollLevel();
	int GetBrick(int nX, int nY);
	void SetBrick(int nX, int nY, int nType);
	Uint8 m_abyLevel[BRICKS_WIDTH][BRICKS_HEIGHT];
};

#endif // !defined(AFX_GENERATELEVEL_H__8C33A485_AE94_4D18_8E89_0D465E446B4C__INCLUDED_)
