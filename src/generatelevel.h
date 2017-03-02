/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"

#include <SDL.h>

class CGenerateLevel final
{
public:
    CGenerateLevel();
    ~CGenerateLevel();

    void LoadPatterns();
    void Generate(int nLevel, bool bUseCustom);

protected:
    Uint8* m_pbyLevelPatterns;
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
