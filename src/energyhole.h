/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "defines.h"

#include <vector>

class CEnergyHole final
{
public:
    CEnergyHole();
    ~CEnergyHole();

    void AddEnergyHole();
    void RemoveAll();
    void Draw();
    void Move();

private:
    struct sEnergyHole
    {
        float x;
        float y;
        Uint32 frameTime;
        int frame;
        Uint32 move_time;
        int angle;
        bool is_over;
    };
    std::vector<sEnergyHole> m_holes;
};
