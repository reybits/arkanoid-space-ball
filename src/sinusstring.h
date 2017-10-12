/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

class CSinusString final
{
public:
    CSinusString();
    ~CSinusString();

    void SetString(const char* pchString, int nX, int nY);
    void Draw();

protected:
    unsigned char* m_pbySymbs;
    int m_nSinPos;
    int m_nAmplifier;
    int m_nY;
    float m_nX;
    int m_nStartPos;
    int m_nStrLen;
    char* m_pchString;
};
