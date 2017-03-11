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

    size_t GetBonusesOnScreen() const;
    void Move(int nRacketType);

    enum class eType : unsigned
    {
        BALL_SPLIT,
        BALL_FIRE,
        BALL_BLUE,
        BALL_EXPAND,
        BALL_SHRINK,
        PADDLE_MAGNET,
        PADDLE_FIRE,
        PADDLE_MISSILE,
        PADDLE_LASER,
        PADDLE_ENGINE,
        PADDLE_EXPAND,
        PADDLE_SHRINK,
        WALL,
        FREEZE,
        DESTROY,

        Count // Only marker!
    };
    eType IsAcross(int nX, int nY, int nWidth, int nHeight);
    void AddBonus(int x, int y, eType type);
    void RemoveAll(bool bAndStackToo);
    void Draw();
    void SetPosStack(bool bNext);
    void AddToStack(eType type);
    eType GetBonusFromStack();
    size_t GetCountInStack() const;
    eType GetTypeInStack(size_t nPos) const;

private:
    struct sBonus
    {
        eType type;
        int nFrame;
        int nRadius;
        int nSin, nCos;
        float origx, origy;
        int x, y;
    };
    std::vector<sBonus> m_bounses;
    std::vector<eType> m_stack;
    int m_nStackPos;
};
