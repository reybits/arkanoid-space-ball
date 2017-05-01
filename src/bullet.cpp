/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "bullet.h"
#include "accessor.h"
#include "defines.h"
#include "exploision.h"
#include "game.h"
#include "videosystem/videosystem.h"

CBullet::CBullet()
{
    m_bullets.reserve(10);
}

CBullet::~CBullet()
{
}

void CBullet::Move()
{
    for (size_t i = 0, size = m_bullets.size(); i < size; i++)
    {
        auto& bullet = m_bullets[i];

        if (bullet.type != eBulletType::LASER)
        {
            int nAngle = GetAngle(i);
            bullet.x += g_fSin[nAngle] * 2 * g_fSpeedCorrection;
            bullet.y -= g_fCos[nAngle] * 2 * g_fSpeedCorrection;
            int nX = (int)bullet.x;
            int nY = (int)bullet.y;
            if (nX < WALL_X1 || nY < WALL_Y1 || nY + 20 > WALL_Y2)
            {
                if (bullet.type == eBulletType::MISSILE)
                {
                    a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
                }
                else
                {
                    a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 1);
                }
                size = RemoveByPos(i);
            }
        }
        else
        {
            if (bullet.dwLaser + 30 < SDL_GetTicks())
            {
                size = RemoveByPos(i);
            }
        }
    }
}

void CBullet::Draw()
{
    Uint32 dwTime = SDL_GetTicks();
    static Uint32 dwFrame = 0;
    static int nFrame = 0;
    if (dwFrame + 100 < dwTime)
    {
        dwFrame = dwTime;
        nFrame++;
        nFrame %= 10;
    }

    SDL_Rect rc;
    for (const auto& bullet : m_bullets)
    {
        switch (bullet.type)
        {
        case eBulletType::LASER:
            rc.x = 0;
            rc.y = 40; // + bullet.nFrameFlame * 20;
            rc.w = bullet.w;
            rc.h = 20;
            break;
        case eBulletType::MISSILE:
            rc.x = 54 * nFrame;
            rc.y = 0;
            rc.w = 54;
            rc.h = 20;
            break;
        case eBulletType::PLASMA:
            rc.x = 54 * nFrame;
            rc.y = 20;
            rc.w = 54;
            rc.h = 20;
            break;
        }
        render(bullet.x, bullet.y, eImage::Bullets, &rc);
    }
}

void CBullet::RemoveAll()
{
    m_bullets.clear();
}

void CBullet::AddBullets(int y, eBulletType type)
{
    sBullet bullet;
    bullet.type = type;
    bullet.x = type != eBulletType::LASER ? RACKET_X : a::ark()->m_nLaserX;
    bullet.y = y;
    bullet.nAngle = 270;
    bullet.w = RACKET_X - a::ark()->m_nLaserX - 15 + 8;
    bullet.dwLaser = SDL_GetTicks();
    m_bullets.push_back(bullet);
}

bool CBullet::IsAcross(int nX, int nY, int nWidth, int nHeight, bool& bRemoveAll, eBulletType& type)
{
    for (size_t i = 0, size = m_bullets.size(); i < size; i++)
    {
        auto& bullet = m_bullets[i];

        if (bullet.type == eBulletType::LASER)
        {
            if (bullet.x + bullet.w >= nX && bullet.x <= nX + nWidth && bullet.y + (20 - 2) / 2 + 2 >= nY && bullet.y + (20 - 2) / 2 <= nY + nHeight)
            {
                a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 2);
                //RemoveByPos(i);
                bullet.x++; // do not allow to across again
                bRemoveAll = false;
                type = bullet.type;
                return true;
            }
        }
        else
        {
            if (bullet.x + 54 >= nX && bullet.x <= nX + nWidth && bullet.y + 20 >= nY && bullet.y <= nY + nHeight)
            {
                type = bullet.type;
                switch (type)
                {
                case eBulletType::MISSILE:
                    a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 0);
                    RemoveByPos(i);
                    bRemoveAll = true;
                    break;

                case eBulletType::PLASMA:
                    a::expl()->AddExploision(nX - (45 - BRICK_W) / 2, nY - (41 - BRICK_H) / 2, 1);
                    bRemoveAll = true;
                    break;

                case eBulletType::LASER: // do nothing
                    break;
                }
                return true;
            }
        }
    }

    return false;
}

sBulletDescription CBullet::getDescription(size_t idx) const
{
    const auto& bullet = m_bullets[idx];

    sBulletDescription desc;

    desc.x = bullet.x;
    desc.y = bullet.y;
    switch (bullet.type)
    {
    case eBulletType::MISSILE:
        desc.w = 46;
        desc.h = 20;
        break;

    case eBulletType::PLASMA:
        desc.w = 30;
        desc.h = 20;
        break;

    case eBulletType::LASER:
        desc.y += (20 - 2) / 2;
        desc.w = bullet.w;
        desc.h = 2;
        break;
    }

    return desc;
}

size_t CBullet::RemoveByPos(size_t nPos)
{
    const auto size = m_bullets.size();
    m_bullets[nPos] = m_bullets[size - 1];
    m_bullets.pop_back();

    return size - 1;
}

void CBullet::ChangeAngle(size_t nPos, bool bRotate)
{
    float fAngle = 0.5f * g_fSpeedCorrection;
    if (bRotate == false)
    {
        m_bullets[nPos].nAngle += (360 - fAngle);
    }
    else
    {
        m_bullets[nPos].nAngle += fAngle;
    }
}

int CBullet::GetAngle(size_t nPos)
{
    return (int)(m_bullets[nPos].nAngle + 360) % 360;
}
