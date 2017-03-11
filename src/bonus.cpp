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

#include <SDL.h>

CBonus::CBonus()
{
    m_bounses.reserve(20);
    m_stack.reserve(20);
    m_nStackPos = 0;
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
    SDL_Rect rc;
    bool bNewFrame = false;
    static int nFrameStack = 0;
    static Uint32 dwFrameTime = 0;
    if (dwFrameTime + 60 < SDL_GetTicks())
    {
        dwFrameTime = SDL_GetTicks();
        nFrameStack++;
        nFrameStack %= 15;
        bNewFrame = true;
    }

    for (auto& bonus : m_bounses)
    {
        const auto type = bonus.type;
        if (bNewFrame == true)
        {
            bonus.nFrame++;
            bonus.nFrame %= 15;
        }
        rc.x = bonus.nFrame * 36;
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
    int nCount = m_stack.size();
    bool bFist2 = true;
    if (nCount > 0)
    {
        int nCountOnScreen = nCount > 2 ? 3 : nCount;
        m_nStackPos %= nCount;
        int nPos = m_nStackPos - 1;
#define SMALL_BONUS_X 532
        int nX = SMALL_BONUS_X;
        if (nPos == -1)
        {
            nPos = nCount - 1;
        }
        do
        {
            if (nCount == 1)
            {
                nX += 24;
            }
            else if (nCount == 2 && m_nStackPos == 0 && bFist2 == true)
            {
                bFist2 = false;
                nX += 24;
                nPos = 0;
            }
            if (nX == SMALL_BONUS_X + 24)
            {
                rc.x = rc.y = 0;
                render(nX, 445, eImage::Transp, &rc);
                a::tutDlg()->AddDialog(SMALL_BONUS_X + 24 + 12, 445, 0, 2);
            }
            rc.x = nFrameStack * 24;
            rc.y = (int)m_stack[nPos] * 24;
            render(nX, 445 + (nX == SMALL_BONUS_X + 24 ? 0 : 6), eImage::BonusesSmall, &rc);
            nX += 24;
            nPos++;
            nPos %= nCount;
        } while (--nCountOnScreen);
        /*      g_FontTutorial.SetRect(SMALL_BONUS_X + 24, 470, 24, 80);
                g_FontTutorial.DrawString(0, 0, "test", 2);*/
    }
}

void CBonus::RemoveAll(bool bAndStackToo)
{
    m_bounses.clear();
    if (bAndStackToo == true)
    {
        m_nStackPos = 0;
        m_stack.clear();
    }
}

void CBonus::AddBonus(int x, int y, eType type)
{
    sBonus bonus;
    bonus.type = type;
    bonus.nFrame = a::rnd().Get(10);
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

void CBonus::SetPosStack(bool bNext)
{
    int nCount = m_stack.size();
    if (nCount > 0)
    {
        if (bNext == true)
        {
            m_nStackPos++;
            m_nStackPos %= nCount;
        }
        else
        {
            m_nStackPos--;
            if (m_nStackPos < 0)
            {
                m_nStackPos = nCount - 1;
            }
        }
    }
    else
    {
        m_nStackPos = 0;
    }
}

void CBonus::AddToStack(eType type)
{
    m_stack.push_back(type);
}

CBonus::eType CBonus::GetBonusFromStack()
{
    int nCount = m_stack.size();
    if (nCount > 0)
    {
        eType type = m_stack[m_nStackPos];
        m_stack[m_nStackPos] = m_stack[m_stack.size() - 1];
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
    //if(nPos >= 0 && nPos < m_stack.size())
    return m_stack[nPos];
    //return -1;
}
