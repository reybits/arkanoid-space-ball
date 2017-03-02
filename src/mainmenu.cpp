/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "arkanoidsb.h"
#include "accessor.h"
#include "bonus.h"
#include "defines.h"
#include "mainmenu.h"
#include "monster.h"
#include "mystring.h"
#include "sinusstring.h"
#include "utils.h"
#include "version.h"

#include <cstdio>

char VersionString[10];

CMainMenu::CMainMenu(sOptions& options, sHighscores& highscores)
    : m_options(options)
    , m_highscores(highscores)
    , m_optionsDirty(false)
    , m_bGetNameMode(false)
    , m_nMenuType(MT_MAIN)
    , m_bInitHelp(true)
    , m_bReturnToGame(false)
    , m_bIsSaveAvailable(false)
{
    m_achStoredLevelInfo[0] = 0;
    m_achName[0] = 0;

    snprintf(VersionString, sizeof(VersionString), "%d.%d.%d", APP_VerMajor, APP_VerMinor, APP_VerRelease);
}

CMainMenu::~CMainMenu()
{
}

int CMainMenu::DrawMenu()
{
    int nRetCode = -1;
    DrawBackground();

    switch (m_nMenuType)
    {
    case MT_MAIN:
        nRetCode = DrawMenuMain();
        break;
    case MT_STARTGAME:
        nRetCode = DrawStartGame();
        break;
    case MT_RULES:
        DrawMenuRules();
        break;
    case MT_HIGHSCORE:
        DrawMenuHighScore();
        break;
    case MT_OPTIONS:
        DrawMenuOptions();
        break;
    }

    const char achTips[] =
        "TIP: Try change video mode to increase performance.   "\
        "TIP: Kamikaze and fiery meteorite blows up the paddle.   "\
        "TIP: Ice-meteorite freezes up the paddle for 3 seconds.   "\
        "TIP: To gain additional scores try to destroy very strong bricks.   "\
        "TIP: Nuclear bomb blows up some bricks around.   "\
        "TIP: Cannon fires on you (shots are frozen or invert paddle movement).   "\
        "TIP: Red brick always has a bonus.   "\
        "TIP: Right mouse button - return to main menu.   "\
        " ";
    static bool bInit = true;
    if (bInit == true)
    {
        bInit = false;
        a::fntSin()->SetString(achTips, SCREEN_WIDTH, 420);
    }
    if (m_nMenuType == MT_MAIN)
    {
        a::fntSin()->Draw();
    }

    if (m_nMenuType == MT_MAIN)
    {
        a::fnt1()->DrawString(5, SCREEN_HEIGHT - 5 - 14, VersionString);
    }
    else if ((IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)) || g_bMouseRB == true)
    {
        SendEsc();
    }

    return nRetCode;
}

void CMainMenu::DrawBackground()
{
    if (m_nMenuType == MT_MAIN)
    {
        Blit(0, 0, m_pBackground, 0);
    }
    else
    {
        Blit(0, 0, m_pBackground2, 0);
    }
}

int CMainMenu::DrawMenuMain()
{
    SDL_Rect rc;
    SetRect(&rc, 0, 0, 248, 29);

    int nItem = -1;
    int nCount = 5;
    for (int i = 0; i < nCount; i++)
    {
        if (true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y + i * 29, i))
        {
            nItem = i;
        }
    }

    if (g_bMouseLB == true)
    {
        g_bMouseLB = false;
        switch (nItem + 1)
        {
        case MT_QUIT:
            return 0; // exit

        default:
            SetMenuType(nItem + 1);
            break;
        }
    }

    return -1; // do nothing
}

int CMainMenu::DrawStartGame()
{
    //a::fnt2()->DrawString(0, MENU_ITEM_Y - 60, "SELECT APPROPRIATE ACTION", 2);
    int nItem   = -1;
    if (true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80, B_NEWGAME))
    {
        nItem   = 0;
    }
    if (true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 29, B_RESTORE))
    {
        if (m_bIsSaveAvailable == true)
        {
            nItem = 1;
        }
    }
    if (true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 58, B_CUSTOMLEVELS))
    {
        nItem   = 7;
    }
    if (true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 87, B_LEVELEDITOR))
    {
        nItem   = 8;
    }
    if (true == DrawMenuButton(MENU_ITEM_X + 62, MENU_ITEM_Y - 80 + 116, B_CANCEL))
    {
        nItem   = 6;
    }
    a::fnt2()->DrawString(0, MENU_ITEM_Y - 110, m_achStoredLevelInfo, 2);
    a::fnt1()->DrawString(0, MENU_ITEM_Y + 120, "Do you wish to get some help while playing (Tutorial)?", 2);
    a::fnt2()->DrawString(0, MENU_ITEM_Y + 120 + 20 + 2, (m_options.tutorialMode == true ? "YES" : "NO"), 2);
    if (true == DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, MENU_ITEM_Y + 120 + 20, B_PREV))
    {
        nItem   = 2;
    }
    if (true == DrawMenuButton(SCREEN_WIDTH / 2 + 40, MENU_ITEM_Y + 120 + 20, B_NEXT))
    {
        nItem   = 3;
    }
    a::fnt1()->DrawString(0, MENU_ITEM_Y + 170, "Do you wish to auto use bonuses (like usual arkanoid)?", 2);
    a::fnt2()->DrawString(0, MENU_ITEM_Y + 170 + 20 + 2, (m_options.autoBonusMode == true ? "YES" : "NO"), 2);
    if (true == DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, MENU_ITEM_Y + 170 + 20, B_PREV))
    {
        nItem   = 4;
    }
    if (true == DrawMenuButton(SCREEN_WIDTH / 2 + 40, MENU_ITEM_Y + 170 + 20, B_NEXT))
    {
        nItem   = 5;
    }

    if (g_bMouseLB == true)
    {
        g_bMouseLB  = false;
        switch (nItem)
        {
        case 0:
            return 1;   // start new game
        case 1:
            return 2;   // restore game
        case 2:
        case 3:
            m_options.tutorialMode = !m_options.tutorialMode;
            break;
        case 4:
        case 5:
            m_options.autoBonusMode = !m_options.autoBonusMode;
            break;
        case 6:
            m_nMenuType = MT_MAIN;
            break;
        case 7:
            return 3;   // custom levels
        case 8:
            return 4;   // level editor
        }
    }

    return -1;  // do nothing
}

void CMainMenu::PlaySFX(int nItem)
{
    static int  nPrevItem   = -1;
    if (nItem != -1 && nItem != nPrevItem)
    {
        PlaySound(5);// g_pMainFrame->m_pImix->SamplePlay(m_nSampleOver);
    }
    nPrevItem   = nItem;
}

void CMainMenu::DrawMenuRules()
{
    SDL_Rect    rc;
    int i   = 120;

    static int  nPage   = 0;

    if (nPage == 0)
    {
        a::fnt2()->DrawString(0, 20, "BASIC CONTROLS", CMyString::FONT_ALIGN_CENTER);
        a::fnt1()->SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
        a::fnt1()->DrawString2(0,  50, "To manage your paddle, simply move your mouse in the direction you want the paddle to move. The Left mouse button will launch the sphere into play and shoot your weapons - if you have collected the appropriate power-up. The Esc key, or Pause key will pause the game and bring up menu screens.");

        a::fnt2()->DrawString(0, 180, "GOALS", CMyString::FONT_ALIGN_CENTER);
        a::fnt1()->DrawString2(0, 210, "There are two main goals of \"Arkanoid: Space Ball\":");
        a::fnt1()->DrawString2(0, 230, "a) Keep your ball in play for as long as possible to advance to the next round.");
        a::fnt1()->DrawString2(0, 270, "b) Accumulate the highest score possible by breaking as many bricks, collecting as many power-ups, and destroying as many monsters as you can.");
        a::fnt1()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    else if (nPage == 1)
    {
        a::fnt2()->DrawString(0, 20, "RULES OF PLAY", CMyString::FONT_ALIGN_CENTER);
        a::fnt1()->SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
        a::fnt1()->DrawString2(0,  50, "To start, you are given three lives. The number of reserve lives is displayed on the scoreboard on the top of the screen. The game is over if you have no reserve spheres. Every 5000 points will get you a reserve life. Each level in \"Arkanoid: Space Ball\" is comprised of a varying number of bricks. Most of these bricks are destructible by bouncing your ball off of them (or shooting them) between one and four times. When all such one-to-four-hit bricks have been cleared, you will have successfully completed the level and will automatically advance to the next rlevel. Note that many levels contain 15-time-hit bricks that do not need to be cleared. All levels of the game are grouped into sets of 5. When you will complete a given set of 5 levels you reach a Bonus level. You have to reach as many scores as you can by shaking down enemy gunnery or debris flying on you at a Bonus level. If you have a Bonus level with gunnery you have to pass 5 balls. In case of debris �you have to pass 3 pushing off debris with paddle.\nThere are two modes of the game: the first one is usual when bonuses activate by taking them at once. The second mode is an accumulation of bonuses when you may chose any balls for using.");
        //a::fnt1()->DrawString(0, 350, "�� �� ������ ��������� 3 ������������ �������� � ��������.");
        a::fnt1()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    else if (nPage == 2)
    {
        a::fnt2()->DrawString(0, 20, "BRICKS", CMyString::FONT_ALIGN_CENTER);
        a::fnt1()->DrawString(0, 50, "One strike bricks (easy to destroy). Red animated brick always has a bonus.", CMyString::FONT_ALIGN_CENTER);
        for (i = BOX_0; i < BOX_DBL_0; i++)
        {
            DrawBrick(70 + ((i - 1) % 25) * 20, 80 + ((i - 1) / 25) * 30, i);
        }
        a::fnt1()->DrawString(0, 160, "Strong bricks (4-strike briks)", CMyString::FONT_ALIGN_CENTER);
        for (i = BOX_DBL_0; i < BOX_IM_0; i++)
        {
            DrawBrick(220 + ((i - BOX_DBL_0) % 10) * 20, 190 + ((i - BOX_DBL_0) / 10) * 30, i);
        }
        a::fnt1()->DrawString(0, 300, "Very strong (3), movable (3), mortiras (2), bombs (2)", CMyString::FONT_ALIGN_CENTER);
        int x = (SCREEN_WIDTH - (BOX_END - BOX_IM_0) * 20 - 3 * 15) / 2;
        for (i = BOX_IM_0; i < BOX_END; i++)
        {
            if (i == BOX_MOV_0 || i == BOX_SHOOT_0 || i == BOX_BOMB_0)
            {
                x += 15;
            }
            DrawBrick(x, 330, i);
            x   += BRICK_W;
        }
    }
    else if (nPage == 3)
    {
        a::fnt2()->DrawString(0, 20, "POWER-UPS", CMyString::FONT_ALIGN_CENTER);
        a::fnt1()->SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
        a::fnt1()->DrawString2(0, 50, "Power-ups are available across all environments. They have the potential to both help and hinder you as you try to complete each round.");
        a::fnt1()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        static int      anFrameBonus[]  = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1 };
        static Uint32   dwTimeBonus = 0;
        bool                bFrameStep  = false;
        if (dwTimeBonus + 30 < SDL_GetTicks())
        {
            dwTimeBonus = SDL_GetTicks();
            bFrameStep  = true;
        }
        const char*  pachDescr[]    =
        {
            "Split balls",
            "Fireballs for 8 seconds (*)",
            "Ghost balls for 8 seconds",
            "Expand ball size",
            "Shrink ball size",
            "Magnetic balls",
            "Plazma canon",
            "Missile cannon (*)",
            "Laser cannon",
            "Flying paddle (*)",
            "Expand paddle size",
            "Shrink paddle size",
            "Force field for 30 seconds",
            "Slowdown balls and objects",
            "Decrease brick strength"
        };
        for (int nBon = 0; nBon < CBonus::TYPE_LAST_BONUS; nBon++)
        {
            rc.x    = 36 * anFrameBonus[nBon];
            rc.y    = 36 * nBon;
            rc.w    = 36;
            rc.h    = 36;
            int nX  = 45;
            if (nBon % 2 == 1)
            {
                nX    += SCREEN_WIDTH / 2;
            }
            Blit(nX, i, m_pBonuses, &rc);
            //          if(nBon == CBonus::TYPE_BALL_BLUE || nBon == CBonus::TYPE_BALL_SHRINK || nBon == CBonus::TYPE_PADDLE_SHRINK) {
            //              Blit(nX - 12, i - 12, g_pBonusesAura, 0);
            //          }
            a::fnt1()->DrawString(nX + 45, i + 8, pachDescr[nBon]);
            if (nBon % 2 == 1)
            {
                i += 36;
            }
            if (bFrameStep == true)
            {
                anFrameBonus[nBon]++;
                anFrameBonus[nBon]  %= 15;
            }
        }
        a::fnt3()->DrawString(0, 400, "(*) - available only in registered version", 2);
    }
    else if (nPage == 4)
    {
        if (m_bInitHelp == true)
        {
            m_bInitHelp = false;
            a::monst()->RemoveAll();
            for (int i = CMonster::MONST_PATROL; i < CMonster::MONST_END; i++)
            {
                a::monst()->AddMonster(40 + (i % 2 == 1 ? SCREEN_WIDTH / 2 : 0), 100 + (i / 2) * 60, i);
            }
        }
        a::fnt2()->DrawString(0, 20, "OBJECTS", CMyString::FONT_ALIGN_CENTER);
        a::monst()->Draw();
        const char*  pachDescr[]    =
        {
            "It's a kamikaze - this monster blows up the paddle.", "Follows the nearest ball.",
            "Robotic eye follows your paddle.", "Disturb to ball moving",
            "This monster disturb to ball moving.", "Robotic hand catches your paddle and holds it at the moment.",
            "This monster disturb to ball moving.", "This monster disturb to ball moving.",
            "Fiery meteorite blows up the paddle.", "Ice-meteorite freezes up the paddle for 3 seconds."
        };
        for (int nBon = 0; nBon < 10; nBon++)
        {
            int nX  = 120;
            if (nBon % 2 == 1)
            {
                nX  += SCREEN_WIDTH / 2;
            }
            a::fnt1()->SetRect(nX, 0, SCREEN_WIDTH / 2 - 120 - 40, SCREEN_HEIGHT);
            a::fnt1()->DrawString2(0, 100 + (nBon / 2) * 60, pachDescr[nBon]);
        }
        a::fnt1()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    else if (nPage == 5)
    {
        a::fnt2()->DrawString(0, 100, "CREDITS", CMyString::FONT_ALIGN_CENTER);
        a::fnt3()->DrawString(0, 130, "'WE' Group", 2);
#if !defined(REMOVE_URLS)
        a::fnt1()->DrawString(0,  150, "http://www.wegroup.org", 2);
        a::fnt1()->DrawString(0,  170, "Copyright (c) 2006-2012. All Rights Reserved.", 2);
        a::fnt2()->DrawString(0, 220, "THANKS TO", CMyString::FONT_ALIGN_CENTER);
        a::fnt3()->DrawString(0, 250, "SDL, SDL-image, SDL-mixer projects", 2);
        a::fnt1()->DrawString(0,  270, "http://www.libsdl.org", 2);
        a::fnt3()->DrawString(0, 290, "glSDL / David Olofson", 2);
        a::fnt1()->DrawString(0,  310, "http://www.olofson.net", 2);
        a::fnt3()->DrawString(0, 330, "Music / Sergey Eybog", 2);
        a::fnt1()->DrawString(0,  350, "http://www.lesser-vibes.com", 2);
#else
        a::fnt1()->DrawString(0,  150, "Copyright (c) 2006-2007. All Rights Reserved.", 2);
        a::fnt2()->DrawString(0, 220, "THANKS TO", CMyString::FONT_ALIGN_CENTER);
        a::fnt3()->DrawString(0, 250, "SDL, SDL-image, SDL-mixer projects", 2);
        a::fnt3()->DrawString(0, 270, "glSDL / David Olofson", 2);
        a::fnt3()->DrawString(0, 290, "Music / Sergey Eybog", 2);
#endif
    }

    if (IsKeyPressed(SDLK_LEFT) && IsKeyStateChanged(SDLK_LEFT) && nPage > 0)
    {
        nPage--;
    }
    if (IsKeyPressed(SDLK_RIGHT) && IsKeyStateChanged(SDLK_RIGHT) && nPage < 5)
    {
        nPage++;
    }

    char    achBuf[20];
    sprintf(achBuf, "Page %d of %d", nPage + 1, 6);
    a::fnt1()->DrawString(10, SCREEN_HEIGHT - 22, achBuf, CMyString::FONT_ALIGN_RIGHT);

    int nItem   = -1;
    if (true == DrawMenuButton(MENU_ITEM_X + 62, 430, B_OK))
    {
        nItem   = 0;
    }
    if (nPage > 0 && true == DrawMenuButton(SCREEN_WIDTH / 2 - 62 - 77 - 10, 430, B_PREV))
    {
        nItem   = 1;
    }
    if (nPage < 5 && true == DrawMenuButton(SCREEN_WIDTH / 2 + 62 + 10, 430, B_NEXT))
    {
        nItem   = 2;
    }

    if (g_bMouseLB == true)
    {
        g_bMouseLB  = false;
        switch (nItem)
        {
        case 0:
            m_nMenuType = MT_MAIN;
            break;
        case 1:
            if (nPage > 0)
            {
                nPage--;
            }
            break;
        case 2:
            if (nPage < 5)
            {
                nPage++;
            }
            break;
        }
    }
}

void CMainMenu::DrawBrick(int nX, int nY, int nType)
{
    SDL_Rect    src;
    static int      nFrame  = 0;
    static Uint32   dwTime  = 0;
    static bool     bDir        = true;
    if (dwTime + 100 < SDL_GetTicks())
    {
        dwTime  = SDL_GetTicks();
        nFrame++;
        nFrame  %= 5;
        if (!nFrame)
        {
            bDir  = !bDir;
        }
    }

    if (nType < BOX_40)
    {
        src.y   = ((nType - 1) / 10) * BRICK_H;
    }
    else if (nType < BOX_DBL_0)
    {
        if (bDir || nType == BOX_49)
        {
            src.y = ((nType - 1) / 10 + nFrame) * BRICK_H;
        }
        else
        {
            src.y = ((nType - 1) / 10 + 4 - nFrame) * BRICK_H;
        }
    }
    else
    {
        src.y   = ((nType - 1) / 10 + 4) * BRICK_H;
    }
    src.x   = ((nType - 1) % 10) * BRICK_W;
    src.w   = BRICK_W;
    src.h   = BRICK_H;
    Blit(nX, nY, m_pBricks, &src);
}

void CMainMenu::DrawMenuHighScore()
{
    int nScoreY         = 130;
    char    achBuf[102];

    a::fnt2()->DrawString(20, nScoreY - 20, "BEST PLAYERS NAME");
    a::fnt2()->DrawString(100, nScoreY - 20, "SCORE", 1);
    a::fnt2()->DrawString(20, nScoreY - 20, "LEV", 1);
    auto& h = m_highscores.entries;
    for (int i = 0; i < 10; i++)
    {
        if (m_highscores.lastPos == i)
        {
            a::fnt2()->DrawNumber(i + 1, 20, (i + 1) * 20 + nScoreY);
            if (m_bGetNameMode == false)
            {
                a::fnt3()->DrawString(60, (i + 1) * 20 + nScoreY, h[i].name);
            }
            else
            {
                a::fnt1()->DrawString(0, 80, "Enter Your name, please", CMyString::FONT_ALIGN_CENTER);
                int nLen = strlen(m_achName);
                if (nLen < 100)
                {
                    m_achName[nLen]     = GetKey();
                    m_achName[nLen + 1] = 0;
                }
                static Uint32   dwTime  = 0;
                static bool bCursor     = true;
                if (dwTime + 500 < SDL_GetTicks())
                {
                    dwTime  = SDL_GetTicks();
                    bCursor = !bCursor;
                }
                snprintf(achBuf, sizeof(achBuf), "%s%s", m_achName, bCursor == true ? "_" : "");
                a::fnt3()->DrawString(60, (i + 1) * 20 + nScoreY, achBuf);

                if (IsKeyPressed(SDLK_RETURN) && IsKeyStateChanged(SDLK_RETURN))
                {
                    if (strlen(m_achName) == 0)
                    {
                        strcpy(m_achName, "Anonymous");
                    }
                    else
                    {
                        strcpy(h[m_highscores.lastPos].name, m_achName);
                    }
                    m_bGetNameMode  = false;
                }
                //nLen  = strlen(m_achName);
                if (IsKeyPressed(SDLK_BACKSPACE) && IsKeyStateChanged(SDLK_BACKSPACE) && nLen > 0)
                {
                    m_achName[nLen - 1] = 0;
                }
            }
            a::fnt2()->DrawNumber(h[i].score, 100, (i + 1) * 20 + nScoreY, 1);
            a::fnt2()->DrawNumber(h[i].level, 20, (i + 1) * 20 + nScoreY, 1);
        }
        else
        {
            a::fnt2()->DrawNumber(i + 1, 20, (i + 1) * 20 + nScoreY);
            a::fnt1()->DrawString(60, (i + 1) * 20 + nScoreY, h[i].name);
            a::fnt2()->DrawNumber(h[i].score, 100, (i + 1) * 20 + nScoreY, 1);
            a::fnt2()->DrawNumber(h[i].level, 20, (i + 1) * 20 + nScoreY, 1);
        }
    }

    if (m_bGetNameMode == false)
    {
        if (true == DrawMenuButton(MENU_ITEM_X + 62, 430, B_OK))
        {
            if (g_bMouseLB == true)
            {
                g_bMouseLB  = false;
                m_nMenuType = MT_MAIN;
            }
        }
    }
}

void CMainMenu::DrawMenuOptions()
{
    int nPos = -1;
    int nButton = -1;
    for (int i = 0; i < 5; i++)
    {
        if (DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, 100 + i * 50, B_PREV))
        {
            nPos = i;
            nButton = 0;
        }
        if (DrawMenuButton(SCREEN_WIDTH / 2 + 40, 100 + i * 50, B_NEXT))
        {
            nPos = i;
            nButton = 1;
        }
    }

    int nItem = -1;
    if (m_optionsDirty)
    {
        if (DrawMenuButton(MENU_ITEM_X, 360, B_OK))
        {
            nItem = 0;
        }
        if (DrawMenuButton(MENU_ITEM_X + 124, 360, B_CANCEL))
        {
            nItem = 1;
        }
    }
    else
    {
        if (DrawMenuButton(MENU_ITEM_X + 62, 360, B_OK))
        {
            nItem = 0;
        }
    }

    if (g_bMouseLB == true)
    {
        g_bMouseLB  = false;

        if (nItem != -1)
        {
            g_bMouseLB  = false;
            if (m_optionsDirty && nItem == 0)
            {
                m_optionsDirty = false;
                SwitchFullscreen();
            }
            else
            {
                m_nMenuType = MT_MAIN;
                if (m_bReturnToGame == true)
                {
                    g_nGameMode = APPS_GAME;
                }
            }
        }

        switch (nPos)
        {
        case 0:
            if (nButton == 0)
            {
                m_options.musicVolume--;
            }
            else if (nButton == 1)
            {
                m_options.musicVolume++;
            }
            SetVolumeMusic(m_options.musicVolume);
            break;

        case 1:
            if (nButton == 0)
            {
                m_options.soundVolume--;
            }
            else if (nButton == 1)
            {
                m_options.soundVolume++;
            }
            SetVolumeSound(m_options.soundVolume);
            break;

        case 2:
            if (nButton != -1)
            {
                m_options.fullscreen = !m_options.fullscreen;
                m_optionsDirty = true;
            }
            break;

        case 3:
            if (nButton != -1)
            {
                const uint32_t bppCount = sizeof(g_bppList) / sizeof(g_bppList[0]);
                if (nButton == 0)
                {
                    m_options.bppIdx = (m_options.bppIdx + 1) % bppCount;
                }
                else
                {
                    m_options.bppIdx = (m_options.bppIdx + bppCount - 1) % bppCount;
                }
                m_optionsDirty = true;
            }
            break;

        case 4:
            if (nButton == 0 || nButton == 1)
            {
                m_options.opengl = !m_options.opengl;
                m_optionsDirty = true;
            }
            break;
        }
    }

    a::fnt1()->DrawString(0, 100 - 15, "Music volume", 2);
    a::fnt1()->DrawString(0, 150 - 15, "Sound Effects volume", 2);
    a::fnt1()->DrawString(0, 200 - 15, "Fullscreen mode", 2);
    a::fnt1()->DrawString(0, 250 - 15, "Color depth (bits-per-pixel)", 2);
    a::fnt1()->DrawString(0, 300 - 15, "Use OpenGL rendering", 2);
    if (m_optionsDirty)
    {
        a::fnt3()->DrawString(0, 340, "Apply settings", 2);
    }

    char achBuf[10];
    sprintf(achBuf, "%d%%", m_options.musicVolume * 10);
    a::fnt2()->DrawString(0, 100 + 2, achBuf, 2);//    a::fnt2()->DrawNumber(m_options.musicVolume, 82, 180, 2);
    sprintf(achBuf, "%d%%", m_options.soundVolume * 10);
    a::fnt2()->DrawString(0, 150 + 2, achBuf, 2);//a::fnt2()->DrawNumber(m_options.soundVolume, 82, 210, 2);
    a::fnt2()->DrawString(0, 200 + 2, (m_options.fullscreen == true ? "YES" : "NO"), 2);
    a::fnt2()->DrawNumber(g_bppList[m_options.bppIdx], 0, 250 + 2, 2);
    a::fnt2()->DrawString(0, 300 + 2, (m_options.opengl == true ? "YES" : "NO"), 2);

    a::fnt1()->DrawString(0, SCREEN_HEIGHT - 52, "<Shift> + <+> or <Shift> + <-> - increase/decrease music volume.\n"\
                      "<+> or <->  - increase/decrease effects volume", CMyString::FONT_ALIGN_CENTER);
    a::fnt1()->DrawString(0, SCREEN_HEIGHT - 20, "<Esc> return to main menu.", CMyString::FONT_ALIGN_CENTER);
}

void CMainMenu::SetMenuType(int nType, bool bReturnToGame)
{
    EnableCursor(true);
    m_nMenuType = nType;
    m_bInitHelp = true;
    m_bReturnToGame = bReturnToGame;
    // get stored state info
    if (nType == MT_STARTGAME)
    {
        char buffer[MAX_PATH];
        snprintf(buffer, sizeof(buffer), "%ssave", a::userProfile());
        m_bIsSaveAvailable  = false;
        FILE* file = fopen(buffer, "rb");
        if (file)
        {
            sSave str;
            if (sizeof(sSave) == fread(&str, 1, sizeof(sSave), file))
            {
                EncodeDecode(&str, sizeof(sSave));
                if (strncmp(str.mag, "WEGR", 4) == 0)
                {
                    m_bIsSaveAvailable = true;
                    snprintf(m_achStoredLevelInfo, sizeof(m_achStoredLevelInfo), "LEVEL: %d, SCORE: %d, LIVES: %d", str.level + 1, str.score, str.lives);
                }
            }
            fclose(file);
        }
        if (m_bIsSaveAvailable == false)
        {
            strcpy(m_achStoredLevelInfo, "NO PREVIOUSLY STORED GAME AVAILABLE");
        }
    }
}

char CMainMenu::GetKey()
{
    const Uint8 abyScans[]  =
    {
        /*      1    2    3    4    5    6    7    8    9    0    -    =        */
        SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_0, SDLK_MINUS, SDLK_EQUALS,
        /*      Q    W    E    R    T    Y    U    I    O    P    [    ]        */
        SDLK_q, SDLK_w, SDLK_e, SDLK_r, SDLK_t, SDLK_y, SDLK_u, SDLK_i, SDLK_o, SDLK_p, SDLK_LEFTBRACKET, SDLK_RIGHTBRACKET,
        /*      A    S    D    F    G    H    J    K    L    ;    '    \        */
        SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_g, SDLK_h, SDLK_j, SDLK_k, SDLK_l, SDLK_SEMICOLON, SDLK_QUOTE, SDLK_BACKSLASH,
        /*      Z    X    C    V    B    N    M    ,    .    /    Spc  `        */
        SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA, SDLK_PERIOD, SDLK_SLASH, SDLK_SPACE, SDLK_BACKQUOTE
    };
    const Uint8 abyKeysLover[]  =
    {
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ', '`',
    };
    const Uint8 abyKeysUpper[]  =
    {
        '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', ' ', '~',
    };

    for (size_t i = 0; i < sizeof(abyScans); i++)
    {
        if (IsKeyPressed(abyScans[i]) && IsKeyStateChanged(abyScans[i]))
        {
            if (g_modState & KMOD_SHIFT)
            {
                return abyKeysUpper[i];
            }
            else
            {
                return abyKeysLover[i];
            }
        }
    }
    return 0;
}

void CMainMenu::SetEnterNameMode()
{
    SetMenuType(MT_HIGHSCORE);
    m_bGetNameMode = true;

    auto& h = m_highscores.entries;
    for (int i = 0; i < 10; i++)
    {
        if (m_highscores.lastScore > h[i].score)
        {
            for (int a = 9; a > i; a--)
            {
                memcpy(&h[a], &h[a - 1], sizeof(h[0]));
            }
            strncpy(h[i].name, "Anonymous", sizeof(h[i].name));
            h[i].score = m_highscores.lastScore;
            h[i].level = m_highscores.lastLevel;
            m_highscores.lastPos = i;
            return;
        }
    }
}

bool CMainMenu::DrawMenuButton(int nX, int nY, int nButtonId)
{
    struct _BUTTON
    {
        int x, y, w;
    };
    static const _BUTTON buttons[] =
    {
        { 0,   0, 248 },                    // start new game
        { 0,  29, 248 },                    // highscores
        { 0,  58, 248 },                    // options
        { 0,  87, 248 },                    // help
        { 0, 116, 248 },                    // exit
        { 0, 145, 248 },                    // register
        { 0, 174, 124 }, { 124, 174, 124 }, // OK, CANCEL
        { 0, 203,  77 }, { 124, 203,  77 }, // <, >
        { 0, 232, 248 },                    // start new game
        { 0, 261, 248 },                    // restore game
        { 0, 290, 248 },                    // buy game
        { 0, 319, 248 },                    // level editor
        { 0, 348, 248 },                    // custom levels
    };
    const int w = buttons[nButtonId].w;
    bool bIsOver = false;
    if (g_nCursorX > nX
        && g_nCursorX < (nX + w)
        && g_nCursorY > nY
        && g_nCursorY < (nY + 24))
    {
        bIsOver = true;
        PlaySFX(nButtonId);
    }

    SDL_Rect rc;
    SetRect(&rc, buttons[nButtonId].x + (bIsOver ? 248 : 0), buttons[nButtonId].y, buttons[nButtonId].w, 29);
    Blit(nX, nY, m_pMainMenuIcons, &rc);

    return bIsOver;
}

void CMainMenu::SendEsc()
{
    g_bMouseRB      = false;
    m_nMenuType     = MT_MAIN;
    m_bGetNameMode  = false;
    if (m_bReturnToGame == true)
    {
        g_nGameMode = APPS_GAME;
    }
}
