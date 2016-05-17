// SinusString.h: interface for the CSinusString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINUSSTRING_H__0CDD30E7_5A9C_4F6E_A69E_0CD5F6B61DA4__INCLUDED_)
#define AFX_SINUSSTRING_H__0CDD30E7_5A9C_4F6E_A69E_0CD5F6B61DA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSinusString {
public:
	void SetString(const char *pchString, int nX, int nY);
	void Draw();
	CSinusString();
	virtual ~CSinusString();

protected:
	Uint8	*m_pbySymbs;
	int m_nSinPos;
	int m_nAmplifier;
	int m_nY;
	float m_nX;
	int m_nStartPos;
	int	m_nStrLen;
	char *m_pchString;
};

#endif // !defined(AFX_SINUSSTRING_H__0CDD30E7_5A9C_4F6E_A69E_0CD5F6B61DA4__INCLUDED_)
