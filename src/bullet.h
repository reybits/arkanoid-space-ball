/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include <SDL.h>
#include <vector>

class CBullet final
{
public:
    CBullet();
    ~CBullet();

    bool GetPositionAndSize(SDL_Rect& rc, int& nPos, bool bFromFirst);
    void Move();
    bool IsAcross(int nX, int nY, int nWidth, int nHeight, bool& bRemoveAll, int& nType);
    enum _TYPES { TYPE_LASER, TYPE_MISSILE, TYPE_PLASMA };
    void AddBullets(int y, int nType);
    void RemoveAll();
    void Draw();
    void ChangeAngle(int nPos, bool bRotate);

protected:
    int GetAngle(int nPos);
    void RemoveByPos(int nPos);
    struct _BULLET
    {
        int     nType;
        float       x, y;
        float       nAngle;
        int     w;  // laser related
        Uint32  dwLaser;
    };
    std::vector<_BULLET>m_vecBullets;
};
