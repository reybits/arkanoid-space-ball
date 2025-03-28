/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"

class CLevelEditor final
{
public:
    CLevelEditor();
    ~CLevelEditor();

    void Draw();
    void Load();
    void Save();

private:
    Uint8* m_levels;
    int m_nCurrentLevel;
    int m_nLevelsCount;
    bool m_bIsSelectBrickMode;
    int m_nBrickType;
    Uint8 m_level[BRICKS_WIDTH][BRICKS_HEIGHT];
};
