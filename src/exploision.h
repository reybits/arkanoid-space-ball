/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

class CExploision final
{
public:
    CExploision();
    ~CExploision();

    int GetCount();
    void RemoveAll();
    void AddExploision(int x, int y, int nType);
    void Draw(bool bBricks);

protected:
    struct _EXPLOISION
    {
        int x, y;
        int nType;
        int nFrame;
    };
    std::vector<_EXPLOISION>m_vecExploisions;
};
