/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "game.h"
#include "accessor.h"
#include "ball.h"
#include "bonus.h"
#include "bullet.h"
#include "coolstring.h"
#include "defines.h"
#include "energyhole.h"
#include "exploision.h"
#include "mainmenu.h"
#include "monster.h"
#include "mystring.h"
#include "random.h"
#include "tutorialdlg.h"
#include "utils.h"
#include "videosystem/videosystem.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

// paddle size can be small (50px), default (80px), expanded (110px), big (140px)
#define PADDLE_HEIGHT (50 + m_nRacketSize * 30)
#define SCORE_TO_ADDITIONAL_BALL 5000

CGame::CGame(const sOptions& options)
    : m_options(options)
    , m_nBonusLevelType(-1)
    , m_nSndWall(-1)
    , m_dwUnregisterdTime(0)
    , m_nGameState(-1)
    , m_nCurrentLevel(0)
    , m_nLevelImage(-1)
    , m_pchGetReeadyInfo(nullptr)
    , m_bIsCustomLevels(false)
{
    m_bricks.reserve(BRICKS_WIDTH * BRICKS_HEIGHT);
}

CGame::~CGame()
{
    delete[] m_pchGetReeadyInfo;
}

void CGame::InitNewGame(bool bIsCustomLevels)
{
    m_bIsCustomLevels = bIsCustomLevels;
    m_bTutorialPause = false;
    m_nCurrentLevel = 0;
    m_nLevelPrev = -1;
    m_nScore = 0;
    m_nLives = 3;
    m_nScoreToAdditionalBall = SCORE_TO_ADDITIONAL_BALL;
    a::bonus()->RemoveAll(true);
    //EnableMusic();

    InitLevel(m_nCurrentLevel);
    //g_pMainFrame->m_pImix->SamplePlay(m_nSampleGetReady);
}

void CGame::DoGameActive()
{
    if (m_bTutorialPause == true)
    {
        return;
    }
    if (IsKeyPressed(SDLK_p) && IsKeyStateChanged(SDLK_p))
    {
        SetPause();
    }

    SDL_Rect rc;
    Uint32 dwTime = SDL_GetTicks();
    static Uint32 dwSecondTime = 0;
    if (dwSecondTime + 1000 < dwTime)
    {
        dwSecondTime = dwTime;
        if (m_nBonusLevelType == -1)
        {
            static int nCountCopter = 30;
            if (m_nCurrentLevel % 4 != 0) // each 4 level only meteors
            {
                if (--nCountCopter == 0)
                {
                    nCountCopter = 50 + a::rnd().Get(30);
                    a::monst()->AddMonster(50, (int)m_nRacketY + (PADDLE_HEIGHT - 48) / 2, CMonster::eMonsters::PATROL);
                }
            }
            if (++m_nAddNewMonsterCount == 10)
            {
                m_nAddNewMonsterCount = 0;
                if (m_nCurrentLevel % 4 == 0) // each 4 level only meteors
                {
                    auto type = (CMonster::eMonsters)((unsigned)CMonster::eMonsters::STONE1 + a::rnd().Get(2));
                    a::monst()->AddMonster(100 + a::rnd().Get(200), 100 + a::rnd().Get(320), type);
                }
                else
                {
                    auto type = (CMonster::eMonsters)((unsigned)CMonster::eMonsters::COPTER + a::rnd().Get((unsigned)CMonster::eMonsters::WRECKAGE1));
                    a::monst()->AddMonster(100 + a::rnd().Get(200), 100 + a::rnd().Get(320), type);
                }
                //for(int a = 0; a < CMonster::eMonsters::END; a++) {
                //  a::monst()->AddMonster(100 + a::rnd().Get(200), 100 + a::rnd().Get(320), a);
                //}
            }
            if (m_bSlowDown == true && --m_nSlowDownCount == 0)
            {
                m_bSlowDown = false;
            }
            if (m_bCanMovePaddle == false && --m_nCanMovePaddleCount == 0)
            {
                m_bCanMovePaddle = true;
            }
            if (m_bPaddleIsInvert == true && --m_nPaddleInvertCount == 0)
            {
                m_bPaddleIsInvert = false;
            }
            if (m_bSelfDestructStarted == true && m_nCountSelfDestruct > 0)
            {
                m_nCountSelfDestruct--;
            }
            if (m_bBackWall == true && --m_nCountBackWall == 0)
            {
                StopSound(m_nSndWall);
                m_bBackWall = false;
            }
        }
        else if (m_nBonusLevelType == 1)
        {
            m_nBonusLevelTicks--;
            auto type = (CMonster::eMonsters)((unsigned)CMonster::eMonsters::WRECKAGE1 + a::rnd().Get(2));
            a::monst()->AddMonster(0, 0, type);
        }
    }

    if (m_bCanMovePaddle == true)
    {
        const float speed = (m_bPaddleIsInvert ? -1.0f : 1.0f) * 1.1f; // * g_fSpeedCorrection;
        m_nRacketY += g_nMouseDY * speed;

        if (IsKeyPressed(SDLK_UP))
        {
            m_nRacketY -= g_fSpeedCorrection;
        }
        else if (IsKeyPressed(SDLK_DOWN))
        {
            m_nRacketY += g_fSpeedCorrection;
        }

        if (m_nRacketType == RT_ENGINE)
        {
            m_nRacketX += g_nMouseDX * speed;

            if (IsKeyPressed(SDLK_LEFT))
            {
                m_nRacketX -= g_fSpeedCorrection;
            }
            else if (IsKeyPressed(SDLK_RIGHT))
            {
                m_nRacketX += g_fSpeedCorrection;
            }
        }
    }

    m_nRacketY = std::max<int>(m_nRacketY, WALL_Y1);
    m_nRacketY = std::min<int>(m_nRacketY, WALL_Y2 - PADDLE_HEIGHT);
    m_nRacketX = std::max<int>(m_nRacketX, WALL_X1 + 50);
    m_nRacketX = std::min<int>((m_nRacketType == RT_ENGINE ? m_nRacketX : RACKET_X), RACKET_X + 5);

    static Uint32 dwRacketTime = 0;
    if (m_nRacketX > RACKET_X)
    {
        if (dwRacketTime + 15 < SDL_GetTicks())
        {
            dwRacketTime = SDL_GetTicks();
            m_nRacketX--;
        }
    }

    if (m_nBonusLevelType == -1)
    {
        if (m_bIsCustomLevels == true)
        {
            //          if(IsKeyPressed(SDLK_n) && IsKeyStateChanged(SDLK_n)) {
            //              m_bricks.clear();
            //              //InitLevel(++m_nCurrentLevel);
            //          }
        }
        if (m_nBrickCount == 0)
        {
            a::coolstr()->Add("you have destroyed all bricks!");
            a::tutDlg()->AddDialog(WALL_X2, WALL_Y1, 0, 4);
            if (a::expl()->GetCount() == 0 && (m_options.autoBonusMode == true || a::bonus()->GetBonusesOnScreen() == 0) /* && a::coolstr()->GetCount() == 0*/)
            {
                AddGetReeadyInfo("You have destroyed all bricks!");
                InitLevel(++m_nCurrentLevel);
            }
        }
        // max number of bricks before self destruction
        else if (m_nBrickCount <= 5 && m_bSelfDestructStarted == false && m_nCountSelfDestruct != -1)
        {
            a::coolstr()->Add("timer activated");
            a::tutDlg()->AddDialog(WALL_X2, WALL_Y1, 0, 1);
            m_bSelfDestructStarted = true;
            m_nCountSelfDestruct = 20;
        }
        if (a::ball()->GetBallsCount() == 0)
        {
            AddGetReeadyInfo("You lost balls");
            RemoveOneLives();
        }

        if (g_bMouseLB == true || (IsKeyPressed(SDLK_SPACE) && IsKeyStateChanged(SDLK_SPACE)))
        {
            g_bMouseLB = false;
            DoShoot();
            a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT);
        }

        if (m_options.autoBonusMode == false && g_bMouseRB == true)
        {
            g_bMouseRB = false;
            ProcessBonus(a::bonus()->GetBonusFromStack());
        }
        // correct it after paddle, because it should move as before
        if (m_bSlowDown == true)
        {
            g_fSpeedCorrection *= 0.4f;
        }

        rc.x = (int)m_nRacketX;
        rc.y = (int)m_nRacketY;
        rc.h = PADDLE_HEIGHT;
        //if(IsKeyPressed(SDLK_n) && IsKeyStateChanged(SDLK_n) || IsKeyPressed(SDLK_m)) {
        if (m_nBrickCount > 0) // don't move balls if no bricks on screen
        {
            int nPx = (int)m_nRacketX;
            m_nGetReadyBallsLose += a::ball()->Move(m_bBackWall, rc, m_nRacketType, nPx);
            m_nRacketX = nPx;
        }
        a::bullet()->Move();
        a::monst()->Move(m_bBackWall, m_nRacketY, PADDLE_HEIGHT);
        MoveBrickBullets();
        a::hole()->Move();
        a::bonus()->Move(m_nRacketType);

        CBonus::eType bonusType;
        if ((bonusType = a::bonus()->IsAcross(m_nRacketX, m_nRacketY, 12, PADDLE_HEIGHT)) != CBonus::eType::Count)
        {
            if (m_options.autoBonusMode == true)
            {
                ProcessBonus(bonusType);
            }
            else
            {
                a::bonus()->AddToStack(bonusType);
            }
        }

        if (true == a::monst()->IsAcrossPaddle(m_nRacketX, m_nRacketY, 12, PADDLE_HEIGHT))
        {
            AddScore(15);
        }

        for (size_t i = 0, size = m_vecBrickBullets.size(); i < size;)
        {
            auto& bb = m_vecBrickBullets[i];
            if (bb.fX + 10 > m_nRacketX && bb.fX < m_nRacketX + 12 && bb.fY + 10 > m_nRacketY && bb.fY < m_nRacketY + PADDLE_HEIGHT)
            {
                a::expl()->AddExploision(bb.fX - 12, bb.fY - 10, 0);
                if (bb.nType == 0)
                {
                    m_bPaddleIsInvert = true;
                    m_nPaddleInvertCount += 3;
                }
                else
                {
                    m_bCanMovePaddle = false;
                    m_nCanMovePaddleCount = 3;
                }
                m_vecBrickBullets[i] = m_vecBrickBullets[--size];
                m_vecBrickBullets.pop_back();
                continue;
            }

            i++;
        }

        auto bullets = a::bullet();
        for (size_t idx = 0, size = bullets->getCount(); idx < size; idx++)
        {
            const auto& desc = bullets->getDescription(idx);
            if (true == a::monst()->IsAcross(desc.x, desc.y, desc.w, desc.h, true))
            {
                AddScore(25);
            }

            for (size_t i = 0, size = m_vecBrickBullets.size(); i < size;)
            {
                auto& bb = m_vecBrickBullets[i];
                if (bb.fX + 10 > desc.x && bb.fX < desc.x + desc.w && bb.fY + 10 > desc.y && bb.fY < desc.y + desc.h)
                {
                    a::expl()->AddExploision(bb.fX - 12, bb.fY - 10, 0);
                    m_vecBrickBullets[i] = m_vecBrickBullets[--size];
                    m_vecBrickBullets.pop_back();
                    continue;
                }

                i++;
            }
        }

        const auto balls = a::ball();
        for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
        {
            const auto& desc = balls->getDescription(idx);
            if (desc.bIsCaptured == false)
            {
                if (desc.type != eBallType::BLUE)
                {
                    for (size_t i = 0, size = m_vecBrickBullets.size(); i < size;)
                    {
                        auto& bb = m_vecBrickBullets[i];
                        if (balls->IsThisBallOverObject(idx, bb.fX, bb.fY, 10, 10) > 0)
                        {
                            balls->ChangeBallAngle(idx, false);
                            AddScore(15);
                            a::expl()->AddExploision(bb.fX - 12, bb.fY - 10, 0);
                            m_vecBrickBullets[i] = m_vecBrickBullets[--size];
                            m_vecBrickBullets.pop_back();
                            continue;
                        }
                        i++;
                    }

                    if (true == a::monst()->IsAcrossBall(desc, true))
                    {
                        balls->ChangeBallAngle(idx, true);
                    }
                }
            }
        }
    }
    else if (m_nBonusLevelType == 0)
    {
        rc.x = (int)m_nRacketX;
        rc.y = (int)m_nRacketY;
        rc.h = PADDLE_HEIGHT;
        int nPx = (int)m_nRacketX;
        m_nGetReadyBallsLose += a::ball()->Move(m_bBackWall, rc, m_nRacketType, nPx);
        m_nRacketX = nPx;
        if (m_nBrickCount == 0 && a::expl()->GetCount() == 0 && a::coolstr()->GetCount() == 0)
        {
            AddGetReeadyInfo("All cannons destroyed, bonus 3000 score!");
            AddScore(3000);
            InitLevel(++m_nCurrentLevel);
        }
        if (m_nGetReadyBallsLose == 5)
        {
            AddGetReeadyInfo("You lost 5 balls, no bonus score");
            PlaySound(99); //g_pMainFrame->m_pImix->SamplePlay(m_nSampleGetReady);
            InitLevel(++m_nCurrentLevel);
        }
    }
    else if (m_nBonusLevelType == 1)
    {
        if (g_bMouseLB == true)
        {
            g_bMouseLB = false;
            a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT);
        }
        rc.x = m_nRacketX;
        rc.y = m_nRacketY;
        rc.h = PADDLE_HEIGHT;
        int nPx = m_nRacketX;
        m_nGetReadyBallsLose += a::ball()->Move(m_bBackWall, rc, m_nRacketType, nPx);
        m_nRacketX = nPx;
        a::monst()->Move(m_bBackWall, (int)m_nRacketY, PADDLE_HEIGHT);
        if (true == a::monst()->IsAcrossPaddle2((int)m_nRacketX, (int)m_nRacketY, 12, PADDLE_HEIGHT) || a::ball()->GetBallsCount() == 0)
        {
            AddGetReeadyInfo("Your paddle destroyed, no bonus score");
            PlaySound(99); //g_pMainFrame->m_pImix->SamplePlay(m_nSampleGetReady);
            InitLevel(++m_nCurrentLevel);
        }

        // check intercept monsters and ball
        auto balls = a::ball();
        for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
        {
            const auto& desc = balls->getDescription(idx);
            if (desc.bIsCaptured == false)
            {
                if (true == a::monst()->IsAcrossBall(desc))
                {
                    balls->ChangeBallAngle(idx, true);
                }
            }
        }

        if (m_nBonusLevelTicks == 0)
        {
            AddGetReeadyInfo("You are strong, bonus 3000 score!");
            AddScore(3000);
            InitLevel(++m_nCurrentLevel);
        }
    }
}

bool CGame::DrawScreen()
{
    bool bIsExit = false;

    DrawBackground();

    // we should add combos if timeout
    if (m_dwCombosTime + 300 < SDL_GetTicks())
    {
        if (m_nCombosBricks > 5)
        {
            a::coolstr()->Add("combos %d", m_nCombosBricks);
            AddScore(m_nCombosBricks * 20);
        }
        m_nCombosBricks = 0;
    }

    DrawStatistic();
    if (m_nBonusLevelType == -1)
    {
        a::hole()->Draw();
        //a::monst()->Draw2();
        DrawBricks();
        a::expl()->Draw(true);
        DrawBrickBullets();
        a::bullet()->Draw();
        DrawPaddle();
        a::ball()->Draw(m_nRacketType);
        a::monst()->DrawMonsters();
        if (m_bBackWall == true)
        {
            SDL_Rect rc;
            static Uint32 dwTime = 0;
            static int nFrame = 0;
            if (dwTime + 50 < SDL_GetTicks())
            {
                dwTime = SDL_GetTicks();
                nFrame++;
                nFrame %= 10;
            }
            rc.x = nFrame * 40;
            rc.y = 0;
            rc.w = 40;
            rc.h = 350;
            render(600, 90, eImage::GameWall, &rc);
        }
        a::bonus()->Draw();
    }
    else if (m_nBonusLevelType == 0)
    {
        a::expl()->Draw(true);
        DrawBricks();
        DrawPaddle();
        a::ball()->Draw(m_nRacketType);
    }
    else if (m_nBonusLevelType == 1)
    {
        a::expl()->Draw(true);
        DrawPaddle();
        a::ball()->Draw(m_nRacketType);
        a::monst()->DrawWreckage();
    }
    a::expl()->Draw(false);
    a::coolstr()->Draw();

    switch (m_nGameState)
    {
    //  case GS_GAME_RESTORE:
    //      DrawRestoreGame();
    //      break;

    case GS_GAME_ACTIVE:
        //if(IsKeyPressed(SDLK_g) && IsKeyStateChanged(SDLK_g)) m_GenLev.Generate(++m_nCurrentLevel, false);
        if (IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE))
        {
            SendEsc();
        }
#ifdef _DEBUG
        if (IsKeyPressed(SDLK_w) && IsKeyStateChanged(SDLK_w))
        {
            ProcessBonus(CBonus::eType::WALL);
        }
        if (IsKeyPressed(SDLK_r) && IsKeyStateChanged(SDLK_r))
        {
            static int type = 0;
            ProcessBonus(CBonus::eType::PADDLE_MAGNET + type);
            type++;
            type %= 7;
        }
        if (g_modState & KMOD_SHIFT && IsKeyPressed(SDLK_s) && IsKeyStateChanged(SDLK_s))
        {
            ProcessBonus(CBonus::eType::BALL_EXPAND);
        }
        if (!(g_modState & KMOD_SHIFT) && IsKeyPressed(SDLK_s) && IsKeyStateChanged(SDLK_s))
        {
            ProcessBonus(CBonus::eType::BALL_SHRINK);
        }
        if (IsKeyPressed(SDLK_b) && IsKeyStateChanged(SDLK_b))
        {
            a::ball()->AddBall(m_nRacketX - 20, (int)m_nRacketY + (PADDLE_HEIGHT - 20) / 2);
        }
        if (IsKeyPressed(SDLK_o) && IsKeyStateChanged(SDLK_o))
        {
            a::ball()->SetAllBallsToBlue();
        }
#endif
        DoGameActive();
        m_bTutorialPause = a::tutDlg()->ShowDialog();
        break;

    case GS_GET_READY:
        if (true == DrawGetReady() || (IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)) || g_bMouseRB == true)
        {
            SendEsc();
        }
        break;

    case GS_GAME_PAUSED:
        DrawPause();
        break;

    case GS_GAME_AREYOUSURE_EXIT:
        DrawAreYouSure();
        break;

    case GS_GAME_OVER:
        bIsExit = DoGameOver();
        break;
    }

    return bIsExit;
}

bool CGame::DoGameOver()
{
#define GO_ITEM_X ((SCREEN_WIDTH - 124) / 2)
#define GO_ITEM_Y ((SCREEN_HEIGHT - 15) / 2)

    dimScreen();

    char achBuf[50];
    a::fnt2()->DrawString(0, (SCREEN_HEIGHT - 15) / 2 - 10, "GAME OVER", CMyString::eAlign::Center);
    sprintf(achBuf, "Your achieve %d level, and gain %d score points", m_nCurrentLevel + 1, m_nScore);
    a::fnt1()->DrawString(0, (SCREEN_HEIGHT - 15) / 2 + 20, achBuf, CMyString::eAlign::Center);

    bool bIsOver = a::menu()->DrawMenuButton(GO_ITEM_X, GO_ITEM_Y + 40, CMainMenu::B_OK);
    if (g_bMouseRB == true || (g_bMouseLB == true && true == bIsOver) || (IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)))
    {
        SendEsc();
        return true;
    }

    return false;
}

void CGame::DrawBricks()
{
    Uint32 dwTime = SDL_GetTicks();
    static Uint32 dwTimeCountToDie = 0;
    bool bCountDown = false;
    if (dwTimeCountToDie + 300 < dwTime)
    {
        dwTimeCountToDie = dwTime;
        bCountDown = true;
    }

    m_nBrickCount = 0;
    static Uint32 dwBrickAnimTime2 = 0;
    bool bBrickAnimate2 = false;
    if (dwBrickAnimTime2 + 50 < dwTime)
    {
        dwBrickAnimTime2 = dwTime;
        bBrickAnimate2 = true;
    }

    static Uint32 dwTimeDirChange = 0;
    bool bChangeDir = false;
    if (dwTimeDirChange + 100 < dwTime)
    {
        dwTimeDirChange = dwTime;
        bChangeDir = true;
    }

    bool bSecondOut = false;
    static Uint32 dwTimeSecond = 0;
    if (dwTimeSecond + 1000 < dwTime)
    {
        dwTimeSecond = dwTime;
        bSecondOut = true;
    }

    for (size_t i = 0, size = m_bricks.size(); i < size;)
    {
        auto& lb = m_bricks[i];

        int nX = lb.x;
        int nY = lb.y;
        DrawBrick(i, bBrickAnimate2);
        // remove brick from list
        if (lb.byType == BOX_NONE)
        {
            m_bricks[i] = m_bricks[--size];
            m_bricks.pop_back();
            continue;
        }

        // movable bricks
        if (lb.byType >= BOX_MOV_0 && lb.byType < BOX_SHOOT_0)
        {
            float fSpeed = g_fSpeedCorrection * 0.5f;
            do
            {
                if (lb.bDir == true)
                {
                    if (lb.y + BRICK_H < WALL_Y2 && IsEmptyBrickPos(i, nX, lb.y + BRICK_H) == true)
                    {
                        lb.y += 1;
                    }
                    else
                    {
                        fSpeed = 0;
                        if (lb.nDirChangeCount == 0)
                        {
                            lb.nDirChangeCount = 10;
                        }
                    }
                }
                else
                {
                    if (lb.y > WALL_Y1 && IsEmptyBrickPos(i, nX, lb.y - BRICK_H) == true)
                    {
                        lb.y -= 1;
                    }
                    else
                    {
                        fSpeed = 0;
                        if (lb.nDirChangeCount == 0)
                        {
                            lb.nDirChangeCount = 10;
                        }
                    }
                }
                fSpeed -= 1;
            } while (fSpeed > 0.0);

            // only 0-type brick can reverse
            if (lb.byType == BOX_MOV_0 && bChangeDir == true && lb.nDirChangeCount > 0)
            {
                lb.nDirChangeCount--;
                if (lb.nDirChangeCount == 0)
                {
                    lb.bDir = !lb.bDir;
                }
            }
        }
        //shooting bricks
        else if (m_nGameState == GS_GAME_ACTIVE && lb.byType >= BOX_SHOOT_0 && lb.byType < BOX_BOMB_0)
        {
            if (true == bSecondOut && lb.nCountToShoot > 0)
            {
                lb.nCountToShoot--;
                if (lb.nCountToShoot == 0)
                {
                    if (m_nBonusLevelType == -1)
                    {
                        sBrickBullet bb;
                        bb.fX = lb.x + (BRICK_W - 20) / 2; // 20 - diameter of bullet
                        bb.fY = lb.y + (BRICK_H - 20) / 2;
                        bb.nFrame = 0;
                        if (lb.byType == BOX_SHOOT_0) // do shoot on paddle
                        {
                            a::tutDlg()->AddDialog(bb.fX + 10, bb.fY + 10, 0, 0);
                            lb.nCountToShoot = 30 + a::rnd().Get(20);
                            bb.nType = 0;
                            bb.nAngle = CalcBrickBulletsAngle(i, m_nRacketX, m_nRacketY + PADDLE_HEIGHT / 2);
                        }
                        else // do shoot on ball
                        {
                            a::tutDlg()->AddDialog(bb.fX + 10, bb.fY + 10, 0, 3);
                            lb.nCountToShoot = 20 + a::rnd().Get(30);

                            bb.nType = 1;
                            bb.nAngle = 90;

                            auto balls = a::ball();
                            for (size_t idx = 0, size = balls->GetBallsCount(); idx < size; idx++)
                            {
                                const auto& desc = balls->getDescription(idx);
                                if (desc.bIsCaptured == false)
                                {
                                    if (desc.type != eBallType::BLUE)
                                    {
                                        const auto radius = desc.diameter * 0.5f;
                                        bb.nAngle = CalcBrickBulletsAngle(i, desc.x + radius, desc.y + radius);
                                        break;
                                    }
                                }
                            }
                        }
                        m_vecBrickBullets.push_back(bb);
                    }
                    else if (m_nBonusLevelType == 0)
                    {
                        lb.nCountToShoot = 5;
                        a::ball()->AddBall(nX + BRICK_W, nY + 5, 25 + a::rnd().Get(130)); // angle from 25 to 155
                    }
                }
            }
        }
        else if (lb.byType >= BOX_BOMB_0)
        {
            if (true == bCountDown && lb.nCountToDie > 0)
            {
                lb.nCountToDie--;
                if (lb.nCountToDie == 0)
                {
                    DoBomb(i);
                }
            }
        }

        if (m_nBonusLevelType == -1)
        {
            // set it to bomb
            if (m_bSelfDestructStarted == true && m_nCountSelfDestruct == 0)
            {
                a::coolstr()->Add("bricks armed");
                lb.byType = BOX_BOMB_0 + a::rnd().Get(BOX_END - BOX_BOMB_0);
                lb.nCountToDie = 5;
            }
            if (m_nGameState == GS_GAME_ACTIVE)
            {
                //CalculateLaserX(nX, nY, BRICK_W, BRICK_H);
                bool bRemove;
                eBulletType type;
                if (a::bullet()->IsAcross(nX, nY, BRICK_W, BRICK_H, bRemove, type) == true)
                {
                    if (type != eBulletType::MISSILE)
                    {
                        DoImpact(i, bRemove);
                    }
                    else
                    {
                        DoBomb(i);
                    }
                }
            }
            // calculate bricks count
            if (lb.byType < BOX_IM_0 || lb.byType >= BOX_BOMB_0)
            {
                m_nBrickCount++;
            }
        }
        else if (m_nBonusLevelType == 0)
        {
            m_nBrickCount++;
        }

        i++;
    }
    // disable self destruct while we already turn all bricks to bombs
    if (m_bSelfDestructStarted == true && m_nCountSelfDestruct == 0)
    {
        m_nCountSelfDestruct = -1;
        m_bSelfDestructStarted = false;
    }
    /*  static int  nBr = -1;
        if(m_nBrickCount != nBr) {
            nBr = m_nBrickCount;
            printf("Bricks %d\n", m_nBrickCount);
        }*/
}

void CGame::DrawBrick(size_t idx, bool bNextFrameAnim)
{
    auto& lb = m_bricks[idx];

    SDL_Rect src;
    int type = lb.byType;
    static int nFrame = 0;
    static Uint32 dwTime = 0;
    static bool bDir = true;
    if (dwTime + 80 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        nFrame++;
        nFrame %= 5;
        if (!nFrame)
        {
            bDir = !bDir;
        }
    }

    if (type < BOX_40)
    {
        src.y = ((type - 1) / 10) * BRICK_H;
    }
    else if (type < BOX_DBL_0)
    {
        if (bDir || type == BOX_49)
        {
            src.y = ((type - 1) / 10 + nFrame) * BRICK_H;
        }
        else
        {
            src.y = ((type - 1) / 10 + 4 - nFrame) * BRICK_H;
        }
    }
    else
    {
        src.y = ((type - 1) / 10 + 4) * BRICK_H;
    }
    src.x = ((type - 1) % 10) * BRICK_W;
    src.w = BRICK_W;
    src.h = BRICK_H;
    render(lb.x, lb.y, eImage::Bricks, &src);
    if (lb.nAnimateType != -1)
    {
        src.x = lb.nAnimateType * 20;
        src.y = lb.nFrame * 30;
        if (bNextFrameAnim == true)
        {
            if (++lb.nFrame == 10)
            {
                lb.nAnimateType = -1;
            }
        }
        render(lb.x, lb.y, eImage::Exploision, &src);
    }
}

void CGame::DoBomb(size_t idx)
{
    const auto& target = m_bricks[idx];

    Uint8 byType = target.byType;
    a::tutDlg()->AddDialog(target.x + BRICK_W / 2, target.y + BRICK_H / 2, 0, 6);

    for (auto& brick : m_bricks)
    {
        if (fabsf(target.x - brick.x) <= BRICK_W && fabsf(target.y - brick.y) <= BRICK_H)
        {
            if (brick.byType >= BOX_BOMB_0 && brick.nCountToDie != 0)
            {
                brick.nCountToDie = 1;
                brick.byType = byType;
            }
            else
            {
                AddScore(5);
                ChangeBrick(brick, BOX_NONE, true);
                a::expl()->AddExploision(brick.x - 12, brick.y - 5, 0);
            }
        }
    }
}

void CGame::InitLevel(int nLevel, bool bRestore)
{
    m_bricks.clear();
    a::coolstr()->Clear();
    //a::tutDlg()->Reset();
    m_nCurrentLevel = nLevel;
    m_nGameState = GS_GET_READY;
    m_bSelfDestructStarted = false;
    m_nCountSelfDestruct = 0;
    EnableCursor(true);

    char buffer[MAX_PATH];
    snprintf(buffer, sizeof(buffer), "%ssave", a::userProfile());

    static bool bInit = true;
    if (bInit == true)
    {
        bInit = false;
        m_GenLev.LoadPatterns();
    }

    if (bRestore == true)
    {
        bool bLoaded = false;
        FILE* file = fopen(buffer, "rb");
        if (file != nullptr)
        {
            sSave str;
            if (sizeof(sSave) == fread(&str, 1, sizeof(sSave), file))
            {
                EncodeDecode(&str, sizeof(sSave));
                if (strncmp(str.mag, "WEGR", 4) == 0)
                {
                    bLoaded = true;
                    m_nCurrentLevel = str.level;
                    m_nScore = str.score;
                    m_nLives = str.lives;
                    m_nScoreToAdditionalBall = str.scoreToAdditionalBall;
                    m_nBonusLevelType = str.bonusLevelType;
                    sBrick brick;
                    for (int i = 0; i < str.bricksCount; i++)
                    {
                        fread(&brick, sizeof(sBrick), 1, file);
                        EncodeDecode(&brick, sizeof(sBrick));
                        m_bricks.push_back(brick);
                    }
                    if (m_options.autoBonusMode == false)
                    {
                        CBonus::eType type;
                        for (int i = 0; i < str.bonusesCount; i++)
                        {
                            fread(&type, sizeof(type), 1, file);
                            EncodeDecode(&type, sizeof(type));
                            a::bonus()->AddToStack(type);
                        }
                    }
                }
            }
            fclose(file);
        }
        if (bLoaded == false)
        {
            InitNewGame(m_bIsCustomLevels);
            return;
        }

        m_nLevelPrev = -2; // inform, that we restore game
        m_bTutorialPause = false;
    }
    else
    {
        // set bonus level each 5 level
        if (m_bIsCustomLevels == false && m_nCurrentLevel % 5 == 4)
        {
            m_nBonusLevelType = a::rnd().Get(2); // at this moment only 2 types available
        }
        else
        {
            m_nBonusLevelType = -1; // disable bonus level
        }
    }

    if (m_nCurrentLevel > 0)
    {
        PlaySound(1);
    }
    ResetAll();

    if (m_nBonusLevelType == -1)
    {
        if (bRestore == false)
        {
            m_GenLev.Generate(m_nCurrentLevel, m_bIsCustomLevels);
        }
        a::hole()->RemoveAll();
        switch (m_nCurrentLevel % 5)
        {
        case 0:
            a::hole()->AddEnergyHole();
            break;
        case 1:
            a::hole()->AddEnergyHole();
            a::hole()->AddEnergyHole();
            break;
        case 3:
            a::hole()->AddEnergyHole();
            if (a::rnd().Get(10) % 2 == 0)
            {
                a::hole()->AddEnergyHole();
            }
            break;
        }
    }
    else if (m_nBonusLevelType == 0)
    {
        // TODO: place bricks without intersect one by other and symmetrically
        sBrick brick;
        m_bricks.clear();
        for (int i = 0; i < 5; i++)
        {
            brick.x = BRICK_X + a::rnd().Get(BRICKS_WIDTH / 4) * BRICK_W * 4;
            brick.y = BRICK_Y + a::rnd().Get(BRICKS_HEIGHT / 4) * BRICK_H * 4;
            brick.byType = BOX_SHOOT_0 + a::rnd().Get(2);
            brick.nCountToDie = 10;
            brick.nFrame = 0;
            brick.nAnimateType = -1;
            brick.nCountToShoot = 2 + i;
            m_bricks.push_back(brick);
        }
        m_nGetReadyBallsLose = 0; // used for counting lost balls
    }
    else if (m_nBonusLevelType == 1)
    {
        m_bricks.clear();
        m_nBonusLevelTicks = 3 * 60; // 3 minutes
        // TODO: add paddle power
    }

    // store current game state for later restoring
    if (bRestore == false)
    {
        if (m_nCurrentLevel > 0)
        {
            FILE* file = fopen(buffer, "wb");
            if (file != nullptr)
            {
                sSave str;
                memcpy(str.mag, "WEGR", 4);
                str.level = m_nCurrentLevel;
                str.score = m_nScore;
                str.lives = m_nLives;
                str.scoreToAdditionalBall = m_nScoreToAdditionalBall;
                str.bonusLevelType = m_nBonusLevelType;
                str.bonusLevelTicks = m_nBonusLevelTicks;
                str.getReadyBallsLose = m_nGetReadyBallsLose;
                str.bricksCount = m_bricks.size();
                str.bonusesCount = a::bonus()->GetCountInStack();
                str.bonusLevelType = m_nBonusLevelType;
                EncodeDecode(&str, sizeof(sSave));
                fwrite(&str, sizeof(sSave), 1, file);
                sBrick brick;
                for (size_t i = 0; i < m_bricks.size(); i++)
                {
                    memcpy(&brick, &m_bricks[i], sizeof(sBrick));
                    EncodeDecode(&brick, sizeof(sBrick));
                    fwrite(&brick, sizeof(sBrick), 1, file);
                }
                for (int i = 0; i < a::bonus()->GetCountInStack(); i++)
                {
                    auto bonusType = a::bonus()->GetTypeInStack(i);
                    EncodeDecode(&bonusType, sizeof(bonusType));
                    fwrite(&bonusType, sizeof(bonusType), 1, file);
                }
                fclose(file);
            }
        }
    }

    printf("level %d, is bonus level %s\n", m_nCurrentLevel + 1, m_nBonusLevelType != -1 ? "yes" : "no");
}

void CGame::DrawPaddle()
{
    SDL_Rect src;

    static Uint32 dwEngine = 0;
    static int nEngine = 0;
    src.x = m_nRacketSize * 12;
    src.y = m_bPaddleIsInvert == true ? 140 : 0;
    src.w = 12;
    src.h = PADDLE_HEIGHT;
    render(m_nRacketX, m_nRacketY, eImage::Paddle, &src);
    a::fnt1()->SetRect(m_nRacketX, 0, 14, SCREEN_HEIGHT);
    src.x = 0;
    src.h = 50;
    switch (m_nRacketType)
    {
    case RT_LASER:
        src.y = 380;
        src.w = 32;
        render(m_nRacketX - 15, m_nRacketY + (PADDLE_HEIGHT - 50) / 2, eImage::Paddle, &src);
        break;
    case RT_MISSILE:
    //a::fnt1()->DrawNumber(m_nPaddleMissileCount, 0, (int)m_nRacketY - 15, 2);
    case RT_PLASMA:
        src.y = 330;
        src.w = 36;
        render(m_nRacketX - 17, m_nRacketY + (PADDLE_HEIGHT - 50) / 2, eImage::Paddle, &src);
        break;
    case RT_MAGNET:
        break;
    case RT_ENGINE:
        src.y = 280;
        src.w = 21;
        render(m_nRacketX, m_nRacketY + (PADDLE_HEIGHT - 50) / 2, eImage::Paddle, &src);
        src.x = 48;
        src.y = 30 * nEngine;
        src.w = 30;
        src.h = 30;
        render(m_nRacketX + 12, m_nRacketY + (PADDLE_HEIGHT - 30) / 2, eImage::Paddle, &src);
        if (dwEngine + 50 < SDL_GetTicks())
        {
            dwEngine = SDL_GetTicks();
            nEngine++;
            nEngine %= 3;
        }
        break;
    }
    if (m_bPaddleIsInvert == true)
    {
        a::fnt1()->DrawNumber(m_nPaddleInvertCount, 0, (int)m_nRacketY + PADDLE_HEIGHT, CMyString::eAlign::Center);
    }
    a::fnt1()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (m_bCanMovePaddle == false)
    {
        src.x = 0;
        src.h = 50;
        src.y = 430;
        src.w = 36;
        render((int)m_nRacketX - 12, (int)m_nRacketY + (PADDLE_HEIGHT - 50) / 2, eImage::Paddle, &src);
        a::fnt1()->DrawNumber(m_nCanMovePaddleCount, (int)m_nRacketX + 18, (int)m_nRacketY + (PADDLE_HEIGHT - 15) / 2);
    }
}

void CGame::DrawStatistic()
{
    char achBuff[20];
    a::fnt2()->SetRect(97, 0, 56, SCREEN_HEIGHT);
    a::fnt2()->DrawNumber(m_nCurrentLevel + 1, 99, 19, CMyString::eAlign::Center);

    a::fnt2()->SetRect(174, 0, 141, SCREEN_HEIGHT);
    a::fnt2()->DrawNumber(m_nScore, 177, 19, CMyString::eAlign::Center);

    a::fnt2()->SetRect(338, 0, 56, SCREEN_HEIGHT);
    a::fnt2()->DrawNumber(m_nLives >= 0 ? m_nLives : 0, 342, 19, CMyString::eAlign::Center);
    a::fnt2()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    int nX = 598;
    static int nFrame = 0;
    static int nFrame2 = 0;
    static Uint32 dwTime = 0;
    SDL_Rect rc;
    if (dwTime + 60 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        nFrame++;
        nFrame %= 15;
        nFrame2++;
        nFrame2 %= 10;
    }
    rc.x = nFrame * 36;
    rc.w = 36;
    rc.h = 36;
    if (m_bBackWall == true)
    {
        rc.y = 12 * 36;
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(m_nCountBackWall, nX, 31);
        nX -= 36;
    }
    if (m_bSlowDown == true)
    {
        rc.y = 13 * 36;
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(m_nSlowDownCount, nX, 31);
        nX -= 36;
    }
    if (m_nRacketType == RT_PLASMA)
    {
        rc.y = 6 * 36;
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(m_nPaddleMissileCount, nX, 31);
        nX -= 36;
    }
    if (m_nRacketType == RT_MISSILE)
    {
        rc.y = 7 * 36;
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(m_nPaddleMissileCount, nX, 31);
        nX -= 36;
    }
    if (m_nRacketType == RT_LASER)
    {
        rc.y = 8 * 36;
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(m_nPaddleMissileCount, nX, 31);
        nX -= 36;
    }

    const auto type = a::ball()->GetType();
    if (type != eBallType::WHITE)
    {
        if (type == eBallType::RED)
        {
            rc.y = 1 * 36;
        }
        else
        {
            rc.y = 2 * 36;
        }
        render(nX, 10, eImage::Bonuses, &rc);
        a::fnt1()->DrawNumber(a::ball()->GetTypeCount(), nX, 31);
        //nX    -= 36;
    }

    if (m_nBonusLevelType == 0)
    {
        nX = SCREEN_WIDTH - 5 - 24;
        for (int i = 0; i < 5; i++)
        {
            rc.x = 0;
            rc.y = 0;
            rc.w = rc.h = 22;
            render(nX - i * 24 - 1, 18 - 1, eImage::Transp, &rc);
            if (i < m_nGetReadyBallsLose)
            {
                rc.x = nFrame2 * 20;
                rc.y = 128 + 12;
                rc.w = rc.h = 20;
                render(nX - i * 24, 18, eImage::Balls, &rc);
            }
        }
    }
    else if (m_nBonusLevelType == 1)
    {
        sprintf(achBuff, "%d:%02d", m_nBonusLevelTicks / 60, m_nBonusLevelTicks % 60);
        a::fnt1()->DrawString(5, 10, achBuff, CMyString::eAlign::Right);
        //sprintf(achBuff, "power: %d%%", 100 / m_nGetReadyBallsLose);
        //a::fnt1()->DrawString(5, 10, achBuff, CMyString::eAlign::Right);
    }
}

void CGame::DoShoot()
{
    int nLaserY = (int)m_nRacketY + (PADDLE_HEIGHT - 2) / 2;

    if ((m_nRacketType == RT_LASER || m_nRacketType == RT_MISSILE || m_nRacketType == RT_PLASMA))
    {
        //static Uint32 dwFirePressed   = 0;
        //if(dwFirePressed + 250 < SDL_GetTicks()) {
        //  dwFirePressed   = SDL_GetTicks();
        switch (m_nRacketType)
        {
        case RT_LASER:
            for (auto& brick : m_bricks)
            {
                int nY = (int)brick.y;
                if (nY <= nLaserY + 2 && nY + BRICK_H >= nLaserY)
                {
                    //if(abs(nY - nLaserY) <= abs(4 - nH)) {
                    m_nLaserX = std::max<int>(m_nLaserX, brick.x + BRICK_W);
                }
            }
            a::bullet()->AddBullets((int)m_nRacketY + (PADDLE_HEIGHT - 20) / 2, eBulletType::LASER);
            PlaySound(8);
            break;
        case RT_MISSILE:
            a::bullet()->AddBullets((int)m_nRacketY + (PADDLE_HEIGHT - 20) / 2, eBulletType::MISSILE);
            PlaySound(12);
            break;
        case RT_PLASMA:
            a::bullet()->AddBullets((int)m_nRacketY + (PADDLE_HEIGHT - 20) / 2, eBulletType::PLASMA);
            PlaySound(13);
            break;
        }
        if (--m_nPaddleMissileCount == 0)
        {
            m_nRacketType = RT_NORMAL;
        }
        //}
    }

    m_nLaserX = WALL_X1;
}

void CGame::ProcessBonus(CBonus::eType type)
{
    if (type == CBonus::eType::Count)
    {
        return;
    }

    PlaySound(9);
    m_nGetReadyBonuses++;

    switch (type)
    {
    case CBonus::eType::Count: // do nothing
        break;

    case CBonus::eType::BALL_SPLIT:
        a::ball()->SplitBalls();
        a::coolstr()->Add("split balls");
        break;
    case CBonus::eType::BALL_FIRE:
        a::ball()->SetAllBallsToFire();
        a::coolstr()->Add("fireball");
        break;
    case CBonus::eType::BALL_BLUE:
        a::ball()->SetAllBallsToBlue();
        a::coolstr()->Add("shadow ball");
        break;
    case CBonus::eType::BALL_SHRINK:
        a::ball()->ChangeBallSize(-1);
        a::coolstr()->Add("shrink balls");
        break;
    case CBonus::eType::BALL_EXPAND:
        a::ball()->ChangeBallSize(1);
        a::coolstr()->Add("expand balls");
        break;
    case CBonus::eType::PADDLE_LASER:
        while (true == a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT))
            ;
        if (RT_LASER != m_nRacketType)
        {
            m_nPaddleMissileCount = 0;
        }
        m_nPaddleMissileCount += 40;
        m_nRacketType = RT_LASER;
        a::coolstr()->Add("laser");
        break;
    case CBonus::eType::PADDLE_FIRE:
        while (true == a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT))
            ;
        if (RT_PLASMA != m_nRacketType)
        {
            m_nPaddleMissileCount = 0;
        }
        m_nPaddleMissileCount += 1;
        m_nRacketType = RT_PLASMA;
        a::coolstr()->Add("plasmagun");
        break;
    case CBonus::eType::PADDLE_MISSILE:
        while (true == a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT))
            ;
        if (RT_MISSILE != m_nRacketType)
        {
            m_nPaddleMissileCount = 0;
        }
        m_nPaddleMissileCount += 10;
        m_nRacketType = RT_MISSILE;
        a::coolstr()->Add("missile");
        break;
    case CBonus::eType::PADDLE_SHRINK:
        if (m_nRacketSize > 0)
        {
            m_nRacketSize--;
        }
        a::coolstr()->Add("shrink paddle");
        break;
    case CBonus::eType::PADDLE_EXPAND:
        if (m_nRacketSize < 3)
        {
            m_nRacketSize++;
        }
        a::coolstr()->Add("expand paddle");
        break;
    case CBonus::eType::PADDLE_ENGINE:
        while (true == a::ball()->StartBall((int)m_nRacketY, (int)m_nRacketY + PADDLE_HEIGHT))
            ;
        m_nPaddleMissileCount = 0;
        m_nRacketType = RT_ENGINE;
        a::coolstr()->Add("flying paddle");
        break;
    case CBonus::eType::PADDLE_MAGNET:
        m_nPaddleMissileCount = 0;
        m_nRacketType = RT_MAGNET;
        a::coolstr()->Add("magnetic paddle");
        break;
    case CBonus::eType::WALL:
        m_bBackWall = true;
        m_nCountBackWall += 30;
        a::coolstr()->Add("force field");
        if (m_nSndWall == -1)
        {
            m_nSndWall = PlaySound(0, -1);
        }
        break;
    case CBonus::eType::DESTROY:
        for (auto& brick : m_bricks)
        {
            const auto type = brick.byType;
            if (type < BOX_BOMB_0)
            {
                if (type >= BOX_IM_0)
                {
                    ChangeBrick(brick, BOX_0 + (type - BOX_IM_0));
                }
                else if (type >= BOX_FOU_0)
                {
                    ChangeBrick(brick, BOX_0 + (type - BOX_FOU_0));
                }
                else if (type >= BOX_TRD_0)
                {
                    ChangeBrick(brick, BOX_0 + (type - BOX_TRD_0));
                }
                else if (type >= BOX_DBL_0)
                {
                    ChangeBrick(brick, BOX_0 + (type - BOX_DBL_0));
                }
            }
        }
        a::monst()->IsAcross(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, true);
        a::coolstr()->Add("bricks destructor");
        break;
    case CBonus::eType::FREEZE:
        m_bSlowDown = true;
        m_nSlowDownCount = 20;
        a::coolstr()->Add("slow down");
        break;
    }
}

void CGame::DoImpact(int nIndex, bool bRemoveAll)
{
    m_nCombosBricks++;
    m_dwCombosTime = SDL_GetTicks();

    auto& brick = m_bricks[nIndex];
    auto byType = brick.byType;

    if (byType >= BOX_BOMB_0)
    {
        AddScore(5);
        DoBomb(nIndex);
    }
    else if (byType >= BOX_IM_0) // and BOX_SHOOT_ and BOX_MOV_ too
    {
        if (byType < BOX_MOV_0)
        {
            a::tutDlg()->AddDialog((int)brick.x + BRICK_W / 2, (int)brick.y + BRICK_H / 2, 0, 8);
        }
        brick.nCountToDie--;
        if (bRemoveAll == true || brick.nCountToDie == 0)
        {
            AddScore(30);
            ChangeBrick(brick, BOX_NONE, bRemoveAll);
        }
        else
        {
            brick.nAnimateType = 0;
            brick.nFrame = 0;
        }
    }
    else if (byType >= BOX_FOU_0)
    {
        AddScore(5);
        ChangeBrick(brick, BOX_TRD_0 + (byType - BOX_FOU_0), bRemoveAll);
    }
    else if (byType >= BOX_TRD_0)
    {
        AddScore(5);
        ChangeBrick(brick, BOX_DBL_0 + (byType - BOX_TRD_0), bRemoveAll);
    }
    else if (byType >= BOX_DBL_0)
    {
        AddScore(5);
        ChangeBrick(brick, BOX_0 + (byType - BOX_DBL_0), bRemoveAll);
    }
    else
    {
        AddScore(m_bSelfDestructStarted == false ? 5 : 100);
        ChangeBrick(brick, BOX_NONE, true);
    }
    if (byType < BOX_BOMB_0)
    {
        PlaySound(7);
    }
}

void CGame::ChangeBrick(sBrick& brick, Uint8 byToBrickType, bool bRemoveAll)
{
    if (bRemoveAll == true)
    {
        byToBrickType = BOX_NONE;
    }
    else
    {
        brick.nAnimateType = 1;
        brick.nFrame = 0;
    }

    // add bonus. only for non bonus level
    if (byToBrickType == BOX_NONE)
    {
        m_nGetReadyBricks++;
        a::expl()->AddExploision(brick.x - 12, brick.y - 5, 3);
        if (m_nBonusLevelType == -1)
        {
            static Uint32 dwTime = 0;
            // brick 47 adds bonus always
            if (brick.byType == BOX_47 || dwTime + 5000 < SDL_GetTicks())
            {
                if (brick.byType == BOX_47)
                {
                    a::tutDlg()->AddDialog(brick.x + BRICK_W / 2, brick.y + BRICK_H / 2, 0, 7);
                }
                dwTime = SDL_GetTicks();
                const auto type = (CBonus::eType)a::rnd().Get((unsigned)CBonus::eType::Count);
                a::bonus()->AddBonus(brick.x, brick.y, type);
                PlaySound(10);
            }
        }
    }

    brick.byType = byToBrickType;
}

void CGame::AddScore(int nScore)
{
    m_nGetReadyScore += nScore;
    m_nScore += nScore;
    m_nScoreToAdditionalBall -= nScore;
    if (m_nScoreToAdditionalBall <= 0)
    {
        m_nScoreToAdditionalBall += SCORE_TO_ADDITIONAL_BALL;
        m_nLives++;
    }
}

void CGame::ResetAll()
{
    m_dwCombosTime = 0;
    m_nCombosBricks = 0;

    m_nCountBackWall = 0;
    m_bBackWall = false;
    StopSound(m_nSndWall);
    //m_bSelfDestructStarted    = false;
    //m_nCountSelfDestruct      = 0;
    m_bCanMovePaddle = true;
    m_nCanMovePaddleCount = 0;
    m_bSlowDown = false;
    m_nSlowDownCount = 0;

    m_nAddNewMonsterCount = 0;

    m_bPaddleIsInvert = false;
    m_nPaddleInvertCount = 0;
    m_nBrickCount = -1;
    switch (m_nBonusLevelType)
    {
    case -1:
        m_nRacketSize = 1;
        break;
    case 0:
        m_nRacketSize = 2;
        break;
    case 1:
        m_nRacketSize = 0;
        break;
    }
    m_nRacketType = RT_NORMAL;
    m_nPaddleMissileCount = 0;
    m_nRacketY = WALL_Y1 + ((WALL_Y2 - WALL_Y1) - PADDLE_HEIGHT) / 2;
    m_nRacketX = RACKET_X;

    m_vecBrickBullets.clear();
    m_vecBrickBullets.reserve(10);
    a::bonus()->RemoveAll(false);
    a::expl()->RemoveAll();
    a::bullet()->RemoveAll();
    a::monst()->RemoveAll();
    a::ball()->RemoveAll();
    if (m_nBonusLevelType == -1 || m_nBonusLevelType == 1)
    {
        a::ball()->AddBall((int)m_nRacketX - 20, (int)m_nRacketY + (PADDLE_HEIGHT - 20) / 2);
    }
}

void CGame::DrawBrickBullets()
{
    SDL_Rect rc;
    static Uint32 dwTime = 0;
    bool bUpdate = false;
    if (dwTime + 50 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        bUpdate = true;
    }
    rc.w = 20;
    rc.h = 20;
    for (auto& bb : m_vecBrickBullets)
    {
        rc.x = bb.nFrame * 20;
        rc.y = bb.nType * 20;
        render(bb.fX, bb.fY, eImage::BricksMovBullets, &rc);
        if (bUpdate == true)
        {
            bb.nFrame++;
            bb.nFrame %= 5;
        }
    }
}

void CGame::MoveBrickBullets()
{
    for (size_t i = 0, size = m_vecBrickBullets.size(); i < size;)
    {
        auto& b = m_vecBrickBullets[i];

        b.fX += g_fSpeedCorrection * g_fSin[b.nAngle];
        b.fY -= g_fSpeedCorrection * g_fCos[b.nAngle];
        if (b.fX < WALL_X1 || b.fX > (m_bBackWall == true ? WALL_X2 - 20 : SCREEN_WIDTH) || b.fY < WALL_Y1 || b.fY + 20 > WALL_Y2)
        {
            if (b.fX < SCREEN_WIDTH)
            {
                a::expl()->AddExploision((int)b.fX - 12, (int)b.fY - 10, 0);
            }
            m_vecBrickBullets[i] = m_vecBrickBullets[--size];
            m_vecBrickBullets.pop_back();
            continue;
        }

        i++;
    }
}

int CGame::CalcBrickBulletsAngle(int nIndex, int nX, int nY)
{
    const auto& brick = m_bricks[nIndex];

    int nAngle = 0;
    int nCatet1 = nX - ((int)brick.x + BRICK_W / 2);
    int nCatet2 = nY - ((int)brick.y + BRICK_H / 2);
    float fDist = sqrt(nCatet1 * nCatet1 + nCatet2 * nCatet2);
    nAngle = (int)(57.3f * asinf(nCatet2 / fDist));
    if (nCatet1 > 0)
    {
        nAngle = 90 + nAngle;
    }
    else
    {
        nAngle = 270 - nAngle;
    }
    return ((360 + nAngle) % 360);
}

bool CGame::IsEmptyBrickPos(int nSkipPos, int nX, int nY)
{
    for (size_t i = 0; i < m_bricks.size(); i++)
    {
        if ((int)i != nSkipPos)
        {
            const auto& brick = m_bricks[i];
            if (abs(nX - (int)brick.x) == 0 && abs(nY - (int)brick.y) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool CGame::DrawGetReady()
{
    char achBuf[50];

    dimScreen();

    if (m_nBonusLevelType == -1)
    {
        sprintf(achBuf, "LEVEL %d\nGET READY!", m_nCurrentLevel + 1);
        if (m_pchGetReeadyInfo != 0 && m_nLevelPrev != -2)
        {
            a::fnt3()->DrawString(0, 218, m_pchGetReeadyInfo, CMyString::eAlign::Center);
        }
    }
    else if (m_nBonusLevelType == 0)
    {
        sprintf(achBuf, "BONUS LEVEL %d\nGET READY!", (m_nCurrentLevel + 1) / 5);
        a::fnt3()->DrawString(0, 218, "You should destroy all cannons, don't lost more than 5 balls!", CMyString::eAlign::Center);
    }
    else if (m_nBonusLevelType == 1)
    {
        sprintf(achBuf, "BONUS LEVEL %d\nGET READY!", (m_nCurrentLevel + 1) / 5);
        a::fnt3()->DrawString(0, 218, "You should destroy all wreckage!", CMyString::eAlign::Center);
    }
    a::fnt2()->DrawString(0, 170, achBuf, CMyString::eAlign::Center);

    // do not show that info while we restore game
    if (m_nLevelPrev != -2 && m_nCurrentLevel > 0)
    {
        a::fnt1()->DrawString(200, 240 + 2, "Bricks destroyed");
        a::fnt2()->DrawNumber(m_nGetReadyBricks, 200, 240, CMyString::eAlign::Right);

        a::fnt1()->DrawString(200, 260 + 2, "Monsters destroyed");
        a::fnt2()->DrawNumber(m_nGetReadyMonsters, 200, 260, CMyString::eAlign::Right);

        a::fnt1()->DrawString(200, 280 + 2, "Gained score");
        a::fnt2()->DrawNumber(m_nGetReadyScore, 200, 280, CMyString::eAlign::Right);

        a::fnt1()->DrawString(200, 300 + 2, "Bonuses collected");
        a::fnt2()->DrawNumber(m_nGetReadyBonuses, 200, 300, CMyString::eAlign::Right);

        a::fnt1()->DrawString(200, 320 + 2, "Balls lost");
        a::fnt2()->DrawNumber(m_nGetReadyBallsLose, 200, 320, CMyString::eAlign::Right);
    }

    bool bIsOver = a::menu()->DrawMenuButton((SCREEN_WIDTH - 124) / 2, 350, CMainMenu::B_OK);
    if (g_bMouseLB == true && true == bIsOver)
    {
        g_bMouseLB = false;
        return true;
    }
    return false;
}

void CGame::AddGetReeadyInfo(const char* pchString)
{
    delete[] m_pchGetReeadyInfo;
    m_pchGetReeadyInfo = new char[strlen(pchString) + 5];
    sprintf(m_pchGetReeadyInfo, "[ %s ]", pchString);
}

void CGame::RemoveOneLives()
{
    m_nLives--;
    EnableCursor(true);
    StopSound(m_nSndWall);
    if (m_nLives >= 0)
    {
        //PlaySound(99);//g_pMainFrame->m_pImix->SamplePlay(m_nSampleGetReady);
        m_nGameState = GS_GET_READY;
    }
    else
    {
        a::coolstr()->Add("game over");
        //PlaySound(99);//g_pMainFrame->m_pImix->SamplePlay(m_nSampleGameOver);
        m_nGameState = GS_GAME_OVER;
    }
}

void CGame::LoadBackground()
{
    if (m_nLevelImage != m_nCurrentLevel)
    {
        m_nLevelImage = m_nCurrentLevel;
        char achBuf[20];
        if (m_nBonusLevelType == -1)
        {
            sprintf(achBuf, "gamebg%d.jpg", m_nCurrentLevel % 5);
        }
        else
        {
            strcpy(achBuf, "gamebg5.jpg");
        }
        SDL_FreeSurface(m_background);
        m_background = a::res()->loadImage(achBuf);
    }
}

void CGame::FreeBackground()
{
    m_nLevelImage = -1;
    SDL_FreeSurface(m_background);
    m_background = nullptr;
}

void CGame::DrawBackground()
{
    // loading new background
    LoadBackground();

    // draw background
    static Uint32 dwBGtime = 0;
    static int nFrame = 0, nPumpX = 0;
    static bool bPumpDir = true;
    render(0, 0, m_background);
    if (dwBGtime + 30 < SDL_GetTicks())
    {
        dwBGtime = SDL_GetTicks();
        nFrame++;
        nFrame %= (240 - 20);
        if (bPumpDir == true)
        {
            nPumpX++;
        }
        else
        {
            nPumpX--;
        }
        if (nPumpX == 0 || nPumpX == 33)
        {
            bPumpDir = !bPumpDir;
        }
    }
    SDL_Rect rc;
    rc.x = nFrame;
    rc.y = 0;
    rc.w = 10;
    rc.h = 14;
    render(5, 128, eImage::GameBGanims, &rc);
    render(5, 408, eImage::GameBGanims, &rc);

    rc.x = int(nFrame * 3);
    rc.y = 14;
    rc.w = 15;
    rc.h = 9;
    render(81, 56, eImage::GameBGanims, &rc);
    rc.h = 10;
    render(544, 465, eImage::GameBGanims, &rc);

    rc.x = 337;
    rc.y = 0;
    rc.w = 23;
    rc.h = 14;
    render(380 + nPumpX, 73, eImage::GameBGanims, &rc);
}

void CGame::DrawAreYouSure()
{
#define MENU_ITEM_X ((SCREEN_WIDTH - 248) / 2)
#define MANU_ITEM_Y ((SCREEN_HEIGHT - 15) / 2)

    dimScreen();
    a::fnt2()->DrawString(0, MANU_ITEM_Y - 10, "ARE YOU SURE TO LEAVE", CMyString::eAlign::Center);
    a::fnt2()->DrawString(0, MANU_ITEM_Y + 10, "THIS BOARD", CMyString::eAlign::Center);

    int nItem = -1;
    if (true == a::menu()->DrawMenuButton(MENU_ITEM_X, MANU_ITEM_Y + 40, CMainMenu::B_OK))
    {
        nItem = 0;
    }
    if (true == a::menu()->DrawMenuButton(MENU_ITEM_X + 124, MANU_ITEM_Y + 40, CMainMenu::B_CANCEL))
    {
        nItem = 1;
    }
    if (true == a::menu()->DrawMenuButton(MENU_ITEM_X, MANU_ITEM_Y + 79, CMainMenu::B_OPTIONS))
    {
        nItem = 2;
    }

    bool bExit = false;
    if (g_bMouseLB == true)
    {
        g_bMouseLB = false;
        switch (nItem)
        {
        case 0:
            EnableCursor(true);
            m_nGameState = GS_GAME_OVER;
            break;
        case 1:
            bExit = true;
            break;
        case 2:
            EnableCursor(true);
            m_nGameState = GS_GAME_PAUSED;
            g_nGameMode = APPS_MAINMENU;
            a::menu()->SetMenuType(CMainMenu::MT_OPTIONS, true);
            break;
        }
    }
    if ((IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)) || bExit == true || g_bMouseRB == true)
    {
        SendEsc();
    }
}

void CGame::DrawPause()
{
#define PAUSE_ITEM_X ((SCREEN_WIDTH - 124) / 2)
#define PAUSE_ITEM_Y ((SCREEN_HEIGHT - 15) / 2)

    dimScreen();

    a::fnt2()->DrawString(0, (SCREEN_HEIGHT - 15) / 2 - 10, "GAME PAUSED", CMyString::eAlign::Center);
    a::fnt1()->DrawString(0, (SCREEN_HEIGHT - 15) / 2 + 20, "Press <P> key to unpause.", CMyString::eAlign::Center);

    bool bIsOver = a::menu()->DrawMenuButton(PAUSE_ITEM_X, PAUSE_ITEM_Y + 40, CMainMenu::B_OK);
    if (g_bMouseRB == true || (g_bMouseLB == true && true == bIsOver) || (IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)) || (IsKeyPressed(SDLK_p) && IsKeyStateChanged(SDLK_p)))
    {
        SendEsc();
    }
}

void CGame::SetPause()
{
    if (m_nGameState == GS_GAME_ACTIVE)
    {
        EnableCursor(true);
        m_nGameState = GS_GAME_PAUSED;
    }
}

void CGame::RestoreGame()
{
    InitLevel(0, true);

    //m_nGameState  = GS_GAME_RESTORE;
}

/*
void CGame::DrawRestoreGame() {
    dimScreen();
// #define MAX_SAVE_NAME   30
//  struct _SAVED_GAME {
//      BOOL    bIsValid;
//      int             nLevel;
//      int             nScore;
//      int             nLives;
//      char    achName[MAX_SAVE_NAME + 1];
//      struct CLevel::sBrick   astrLevel[LEVEL_WIDTH][LEVEL_HEIGHT];
//  } m_astrSavedGames[10];

    int nItem   = -1;
    if(true == a::menu()->DrawMenuButton(SCREEN_WIDTH / 2 - 124 - 10, SCREEN_HEIGHT - 50, CMainMenu::B_OK)) nItem   = 0;
    if(true == a::menu()->DrawMenuButton(SCREEN_WIDTH / 2 + 10, SCREEN_HEIGHT - 50, CMainMenu::B_CANCEL))   nItem   = 1;

    if(g_bMouseRB == true || (g_bMouseLB == true && nItem == 0) || (IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE))) {
        g_bMouseRB  = false;
        g_bMouseLB  = false;
        InitNewGame(m_bIsCustomLevels);
    }
    if(g_bMouseLB == true && nItem == 1) {
        g_bMouseLB  = false;
        a::menu()->SetMenuType(CMainMenu::MT_MAIN);
        g_nGameMode = APPS_MAINMENU;
    }
}
*/

void CGame::SendEsc()
{
    switch (m_nGameState)
    {
    case GS_GAME_ACTIVE:
        EnableCursor(true);
        m_nGameState = GS_GAME_AREYOUSURE_EXIT;
        break;
    case GS_GET_READY:
        if (m_nLevelPrev != m_nCurrentLevel)
        {
            m_nLevelPrev = m_nCurrentLevel;
            m_nGetReadyScore = 0;
            m_nGetReadyMonsters = 0;
            m_nGetReadyBonuses = 0;
            m_nGetReadyBricks = 0;
            m_nGetReadyBallsLose = 0;
        }
        //if(m_nCurrentLevel != 1)
        PlaySound(99); //g_pMainFrame->m_pImix->SamplePlay(m_nSampleGetReady);
        m_nGameState = GS_GAME_ACTIVE;
        g_bMouseRB = false;
        ResetAll();
        //a::coolstr()->Add("LET'S GO!");
        EnableCursor(false);
        break;
    case GS_GAME_OVER:
        m_nBonusLevelType = -1;
        StopSound(m_nSndWall);
        g_bMouseRB = false;
        g_bMouseLB = false;
        break;
    case GS_GAME_AREYOUSURE_EXIT:
        g_bMouseRB = false;
        EnableCursor(false);
        m_nGameState = GS_GAME_ACTIVE;
        break;
    case GS_GAME_PAUSED:
        g_bMouseRB = false;
        g_bMouseLB = false;
        EnableCursor(false);
        m_nGameState = GS_GAME_ACTIVE;
        break;
    }
}
