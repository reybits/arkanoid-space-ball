/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "bonus.h"
#include "accessor.h"
#include "defines.h"
#include "random.h"
#include "tutorialdlg.h"
#include "videosystem/videosystem.h"

CBonus::CBonus()
{
    m_bounses.reserve(20);
    m_stack.reserve(20);
    m_stackIdx = 0;
}

CBonus::~CBonus()
{
}

void CBonus::Move(int nRacketType)
{
    bool bNewPos = false;
    static Uint32 dwPosTime = 0;
    if (dwPosTime + 10 < SDL_GetTicks())
    {
        dwPosTime = SDL_GetTicks();
        bNewPos = true;
    }

    for (size_t i = 0, size = m_bounses.size(); i < size;)
    {
        auto& bonus = m_bounses[i];

        bonus.origx += (g_fSpeedCorrection * 0.4);
        bonus.x = (int)bonus.origx;
        //bonus.x = int(bonus.origx + g_fSin[bonus.nSin] * bonus.nRadius / 2);
        bonus.y = int(bonus.origy + g_fCos[bonus.nCos] * bonus.nRadius);

        if (bNewPos == true)
        {
            //bonus.nSin  += 3;
            //bonus.nSin  %= 360;
            bonus.nCos += 3;
            bonus.nCos %= 360;

            if (bonus.nRadius < 20)
            {
                bonus.nRadius++;
            }
        }

        if (bonus.x > SCREEN_WIDTH)
        {
            //g_pMainFrame->m_pImix->SamplePlay(m_nSampleBonusDown, 100, (int)(-100 + ((200.0 / SCREEN_WIDTH) * bonus.x)));
            m_bounses[i] = m_bounses[--size];
            m_bounses.pop_back();
            continue;
        }

        i++;
    }
}

void CBonus::Draw()
{
    static int frameIndex = 0;
    static Uint32 lastTime = 0;
    const bool nextFrame = lastTime + 60 < SDL_GetTicks();
    if (nextFrame)
    {
        lastTime = SDL_GetTicks();
        frameIndex = (frameIndex + 1) % 15;
    }

    SDL_Rect rc;
    for (auto& bonus : m_bounses)
    {
        const auto type = bonus.type;
        if (nextFrame == true)
        {
            bonus.frame = (bonus.frame + 1) % 15;
        }
        rc.x = bonus.frame * 36;
        rc.y = (int)type * 36;
        rc.w = rc.h = 36;
        render(bonus.x, bonus.y, eImage::Bonuses, &rc);
        rc.y = 0;
        rc.w = rc.h = 60;
        if (type == eType::BALL_BLUE || type == eType::BALL_SHRINK || type == eType::PADDLE_SHRINK)
        {
            rc.x = 0;
            render(bonus.x - 12, bonus.y - 12, eImage::BonusesAura, &rc);
        }
        else
        {
            rc.x = 60;
            render(bonus.x - 12, bonus.y - 12, eImage::BonusesAura, &rc);
        }
    }

    rc.w = rc.h = 24;
    const size_t size = m_stack.size();
    bool bFist2 = true;
    if (size > 0)
    {
        size_t countOnScreen = std::min<size_t>(3, size);
        m_stackIdx %= size;
        size_t pos = (m_stackIdx + size - 1) % size;

#define SMALL_BONUS_X 532
        int nX = SMALL_BONUS_X;
        do
        {
            if (size == 1)
            {
                nX += 24;
            }
            else if (size == 2 && m_stackIdx == 0 && bFist2 == true)
            {
                bFist2 = false;
                nX += 24;
                pos = 0;
            }

            if (nX == SMALL_BONUS_X + 24)
            {
                rc.x = rc.y = 0;
                render(nX, 445, eImage::Transp, &rc);
                a::tutDlg()->AddDialog(SMALL_BONUS_X + 24 + 12, 445, 0, 2);
            }

            rc.x = frameIndex * 24;
            rc.y = (int)m_stack[pos] * 24;
            render(nX, 445 + (nX == SMALL_BONUS_X + 24 ? 0 : 6), eImage::BonusesSmall, &rc);
            nX += 24;
            pos = (pos + 1) % size;
        } while (--countOnScreen);
    }
}

void CBonus::RemoveAll(bool bAndStackToo)
{
    m_bounses.clear();
    if (bAndStackToo == true)
    {
        m_stackIdx = 0;
        m_stack.clear();
    }
}

void CBonus::AddBonus(int x, int y, eType type)
{
    sBonus bonus;
    bonus.type = type;
    bonus.frame = a::rnd().Get(10);
    bonus.nRadius = 0;
    bonus.nSin = 0;
    bonus.nCos = 0;
    bonus.origx = (float)x;
    bonus.origy = (float)y;
    bonus.x = x;
    bonus.y = y;
    m_bounses.push_back(bonus);
    a::tutDlg()->AddDialog(bonus.x + 18, bonus.y + 18, 2, (int)type);
}

CBonus::eType CBonus::IsAcross(int nX, int nY, int nWidth, int nHeight)
{
    for (size_t i = 0, size = m_bounses.size(); i < size; i++)
    {
        const auto& bonus = m_bounses[i];

        if (bonus.x + 36 >= nX && bonus.x <= nX + nWidth && bonus.y + 36 >= nY && bonus.y <= nY + nHeight)
        {
            const auto type = bonus.type;
            m_bounses[i] = m_bounses[--size];
            m_bounses.pop_back();
            return type;
        }
    }

    return eType::Count;
}

size_t CBonus::GetBonusesOnScreen() const
{
    return m_bounses.size();
}

void CBonus::SetPosStack(bool next)
{
    size_t size = m_stack.size();
    if (size > 0)
    {
        if (next == true)
        {
            m_stackIdx = (m_stackIdx + 1) % size;
        }
        else
        {
            m_stackIdx = (m_stackIdx + size - 1) % size;
        }
    }
    else
    {
        m_stackIdx = 0;
    }
}

void CBonus::AddToStack(eType type)
{
    m_stack.push_back(type);
}

CBonus::eType CBonus::GetBonusFromStack()
{
    size_t size = m_stack.size();
    if (size > 0)
    {
        eType type = m_stack[m_stackIdx];
        m_stack[m_stackIdx] = m_stack[size - 1];
        m_stack.pop_back();
        return type;
    }

    return eType::Count;
}

size_t CBonus::GetCountInStack() const
{
    return m_stack.size();
}

CBonus::eType CBonus::GetTypeInStack(size_t nPos) const
{
    return m_stack[nPos];
}
