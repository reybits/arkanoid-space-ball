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
#include <vector>

class CBullet final
{
public:
    CBullet();
    ~CBullet();

    size_t getCount() const { return m_bullets.size(); }
    sBulletDescription getDescription(size_t idx) const;

    void Move();

    bool IsAcross(int nX, int nY, int nWidth, int nHeight, bool& bRemoveAll, eBulletType& type);
    void AddBullets(int y, eBulletType type);
    void RemoveAll();
    void Draw();
    void ChangeAngle(size_t nPos, bool bRotate);

private:
    int GetAngle(size_t nPos);
    size_t RemoveByPos(size_t nPos);

private:
    struct sBullet
    {
        eBulletType type;
        float x;
        float y;
        float nAngle;
        int w; // laser related
        Uint32 dwLaser;
    };
    std::vector<sBullet> m_bullets;
};
