/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <vector>

class CBonus final
{
public:
    CBonus();
    ~CBonus();

    int GetBonusesOnScreen();
    void Move(int nRacketType);
    int IsAcross(int nX, int nY, int nWidth, int nHeight);
    enum _TYPES
    {
        TYPE_BALL_SPLIT, TYPE_BALL_FIRE, TYPE_BALL_BLUE, TYPE_BALL_EXPAND, TYPE_BALL_SHRINK,
        TYPE_PADDLE_MAGNET, TYPE_PADDLE_FIRE, TYPE_PADDLE_MISSILE, TYPE_PADDLE_LASER,
        TYPE_PADDLE_ENGINE, TYPE_PADDLE_EXPAND, TYPE_PADDLE_SHRINK,
        TYPE_WALL,
        TYPE_FREEZE,
        TYPE_DESTROY,
        TYPE_LAST_BONUS // Only marker!
    };
    void AddBonus(int x, int y, int nType);
    void RemoveAll(bool bAndStackToo);
    void Draw();
    void SetPosStack(bool bNext);
    void AddToStack(int nType);
    int GetBonusFromStack();
    int GetCountInStack();
    int GetTypeInStack(int nPos);

protected:
    struct _BONUS
    {
        int nType;
        int nFrame;
        int nRadius;
        int nSin, nCos;
        float origx, origy;
        int x, y;
    };
    std::vector<_BONUS>m_vecBonuses;
    std::vector<int>m_vecBonusesStack;
    int m_nStackPos;
};
