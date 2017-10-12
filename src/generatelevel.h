/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"

class CGenerateLevel final
{
public:
    CGenerateLevel();
    ~CGenerateLevel();

    void LoadPatterns();
    void Generate(int level, bool bUseCustom);

private:
    void _CloneLevelBy(int type);
    bool _IsPatternExist(int pattern);
    void _FillLevel(int level, int nPos);
    void _RollVertical();
    void _RollHorizontal();
    int _CalculateBricksCount();
    void _RollLevel();
    int GetBrick(int nX, int nY) const;
    void SetBrick(int nX, int nY, int type);

private:
    Uint8* m_patterns;
    unsigned m_patternsCount;
    bool m_bClonedByHorizontal;
    bool m_bClonedByVertical;
    enum eClone
    {
        CLONE_VERTICAL,
        CLONE_HORIZONTAL
    };
    int m_anPrevPatterns[10];
    Uint8 m_level[BRICKS_WIDTH][BRICKS_HEIGHT];
};
