/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "tutorialdlg.h"
#include "accessor.h"
#include "defines.h"
#include "mystring.h"

#include <SDL.h>

CTutorialDlg::CTutorialDlg(sOptions& options)
    : m_options(options)
{
}

CTutorialDlg::~CTutorialDlg()
{
}

void CTutorialDlg::AddDialog(int nX, int nY, int nCategory, int nMsgId)
{
    if (m_options.tutorialMode == true && m_abShownDialogs[nCategory][nMsgId] == false)
    {
        m_abShownDialogs[nCategory][nMsgId] = true; // do not show this dialog again
        sTutorial   tut;
        tut.nX          = nX;
        tut.nY          = nY;
        tut.nCategory   = nCategory;
        tut.nMsgId      = nMsgId;
        m_vecTutorialDlg.push_back(tut);
        m_tutorialMode = m_options.tutorialMode;
    }
}

bool CTutorialDlg::ShowDialog()
{
    const char* pachText[] =
    {
        "CANNON\n\nDANGER! Cannons fire on you (it's shots are inverted your mouse for 3 seconds).",
        "SELF DESTRUCTION\n\nWhen on level stay 5 bricks or less self destruction activated and starts within 20 seconds.",
        "CATCH BONUS\n\nYou catched bonus placed in to stack. Use right mouse button to use bonus. Use mouse scroll to select bonus.",
        "CANNON\n\nDANGER! Cannons fire on you (it's shots are frozen your mouse for 3 seconds).",
        "BRICKS DESTROYED\nAll bricks destroyed but probabaly not all bonuses captured.",
        "GRAVITATIONAL ANOMALY\n\nATTENTION! Gravitational anomalies effect on flights of balls and missiles.",
        "BOMB\n\nThis brick blows up some bricks around.",
        "BONUS BRICK\n\nATTENTION! This brick always has a bonus.",
        "STRONG BRICKS\n\n15 - hit-bricks. These bricks not need to be cleared.",
    };
    const char* pachMonsters[] =
    {
        "KAMIKAZE\n\nDANGER! It�s a kamikaze - this monster blows up the paddle.",
        "HELICOPTER\n\nFollows the nearest ball.",
        "ROBOTIC EYE\n\nATTENTION! Robotic eye follows your paddle.",
        "SUPERNOVA\n\nThis monster disturb to ball moving.",
        "SPACESHIP\n\nThis monster disturb to ball moving.",
        "ROBOTIC HAND\n\nATTENTION! Robotic hand catches your paddle and holds it at the moment.",
        "TURBINE\n\nThis monster disturb to ball moving.",
        "UFO\n\nThis monster disturb to ball moving.",
        "FIERY METEORITE\n\nDANGER! Fiery meteorite blows up the paddle.",
        "ICE-METEORITE\n\nDANGER! Ice-meteorite freezes up the paddle for 3 seconds.",
    };
    const char* pachBonuses[] =
    {
        "SPLIT BALL\n\nThis bonus doubles number of your balls.",
        "FIREBALL\n\nYour ball will burn through bricks for 8 seconds.",
        "GHOST BALL\n\nATTENTION! Your ball will crossing through bricks for 8 seconds. (Bad bonus!)",
        "EXPAND BALL\n\nThis bonus increases your ball size.",
        "SHRINK BALL\n\nATTENTION! This bonus decreases your ball size. (Bad bonus!)",
        "MAGNET\n\nThis bonus make magnetic paddle.\nUseful for more accuracy aiming.",
        "PLASMA CANNON\n\nOne�shot plasma cannon. A disposable shot of this cannon burns through all bricks.",
        "ROCKET LAUNCHER\n\nRocket launcher with 8 rockets. Every rocket blows up some bricks around.",
        "LASER\n\nLaser gun for 40 shots. The one shot destroys the one 1-hit brick.",
        "ENGINE\n\nAllow padlle to fly. Be careful, Kamikaze and Fiery meteorite within!",
        "EXPAND PADDLE\n\nThis bonus increases your paddle size.",
        "SHRINK PADDLE\n\nATTENTION! This bonus decreases your paddle size. (Bad bonus!)",
        "FORCE FIELD\n\nForce field. You will not worry about your ball for 30 seconds.",
        "CLOCK\n\nATTENTION! This bonus slows down time for 30 seconds.",
        "NUCLEAR BOMB\n\nThis bonus decreases all bricks to 1-hit-bricks and kills all monsters.",
    };

    if (m_options.tutorialMode == true && m_vecTutorialDlg.size() > 0)
    {
        EnableCursor(true);
        int x   = m_vecTutorialDlg[0].nX + 15;
        int y   = m_vecTutorialDlg[0].nY - 188 - 15;
        int ndy = 20;

        SDL_Rect    rc;
        SetRect(&rc, 0, 0, 256, 188);
        if (m_vecTutorialDlg[0].nX > SCREEN_WIDTH / 2)
        {
            rc.x    = 256;
            x       = m_vecTutorialDlg[0].nX - 256 - 15;
        }
        if (m_vecTutorialDlg[0].nY < SCREEN_HEIGHT / 2)
        {
            rc.y    = 188;
            y       = m_vecTutorialDlg[0].nY + 15;
            ndy += 26;
        }

        int nButton = -1;
        if (g_nCursorY >= y + ndy + 110 && g_nCursorY <= y + ndy + 110 + 20)
        {
            if (g_nCursorX > x + 115 && g_nCursorX < x + 115 + 100)
            {
                nButton = 0;
            }
            if (g_nCursorX > x + 220 && g_nCursorX < x + 220 + 25)
            {
                nButton = 1;
            }
        }
        // show dialog
        Blit(x, y, g_pTutorialDlg, &rc);
        // show labels
        SetRect(&rc, 115 + (nButton == 0 ? 256 : 0), 376, 100, 20);
        Blit(x + 115, y + ndy + 110, g_pTutorialDlg, &rc);
        SetRect(&rc, 220 + (nButton == 1 ? 256 : 0), 376, 25, 20);
        Blit(x + 220, y + ndy + 110, g_pTutorialDlg, &rc);
        if (!m_tutorialMode)
        {
            SetRect(&rc, 0, 376, 12, 20);
            Blit(x + 197, y + ndy + 110, g_pTutorialDlg, &rc);
        }
        // show message
        const char*  pchText    = 0;
        switch (m_vecTutorialDlg[0].nCategory)
        {
        case 0:
            pchText = pachText[m_vecTutorialDlg[0].nMsgId];
            break;
        case 1:
            pchText = pachMonsters[m_vecTutorialDlg[0].nMsgId];
            break;
        case 2:
            pchText = pachBonuses[m_vecTutorialDlg[0].nMsgId];
            break;
        }
        a::fntTut()->SetRect(x + 20, y + ndy, 216, 110);
        a::fntTut()->DrawString2(0, 0, pchText);
        //a::fntTut()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (nButton == 0 && g_bMouseLB == true)
        {
            g_bMouseLB  = false;
            m_tutorialMode = !m_tutorialMode;
        }

        if (g_bMouseRB || (nButton == 1 && g_bMouseLB))
        {
            g_bMouseRB = false;
            g_bMouseLB = false;
            EnableCursor(false);
            m_options.tutorialMode = m_tutorialMode;
            m_vecTutorialDlg[0] = m_vecTutorialDlg.back();
            m_vecTutorialDlg.pop_back();
        }

        return true;
    }

    return false;
}

void CTutorialDlg::Reset()
{
    memset(m_abShownDialogs, 0, sizeof(m_abShownDialogs));
}
