/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <cstddef>
#include <vector>

class CExploision final
{
public:
    CExploision();
    ~CExploision();

    size_t GetCount();
    void RemoveAll();
    void AddExploision(int x, int y, int type);
    void Draw(bool bBricks);

private:
    struct eExpl
    {
        int x, y;
        int type;
        int nFrame;
    };
    std::vector<eExpl> m_exlosions;
};
