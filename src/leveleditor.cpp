/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "leveleditor.h"
#include "accessor.h"
#include "arkanoidsb.h"
#include "game.h"
#include "mainmenu.h"
#include "mystring.h"
#include "videosystem/videosystem.h"

#include <cstdio>

CLevelEditor::CLevelEditor()
    : m_levels(nullptr)
    , m_nCurrentLevel(0)
    , m_nLevelsCount(0)
    , m_bIsSelectBrickMode(false)
    , m_nBrickType(BOX_0)
{
}

CLevelEditor::~CLevelEditor()
{
    Save();
    delete[] m_levels;
}

void CLevelEditor::Draw()
{
    SDL_Rect rc;
    int nX, nY;

    EnableCursor(true);
    a::ark()->DrawBackground();
    for (int x = 0; x < BRICKS_WIDTH; x++)
    {
        for (int y = 0; y < BRICKS_HEIGHT; y++)
        {
            a::menu()->DrawBrick(BRICK_X + x * BRICK_W, BRICK_Y + y * BRICK_H, m_level[x][y]);
        }
    }
    a::fnt2()->SetRect(97, 0, 56, SCREEN_HEIGHT);
    a::fnt2()->DrawNumber(m_nCurrentLevel + 1, 99, 19, CMyString::eAlign::Center);
    a::fnt2()->SetRect(338, 0, 56, SCREEN_HEIGHT);
    a::fnt2()->DrawNumber(m_nLevelsCount, 342, 19, CMyString::eAlign::Center);
    a::fnt2()->SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (IsKeyPressed(SDLK_TAB) && IsKeyStateChanged(SDLK_TAB))
    {
        m_bIsSelectBrickMode = !m_bIsSelectBrickMode;
    }
    if (m_bIsSelectBrickMode == true)
    {
        dimScreen();
#define S_BRICK_X (SCREEN_WIDTH - 10 * BRICK_W) / 2
#define S_BRICK_Y (SCREEN_HEIGHT - 9 * BRICK_H) / 2
        int x = S_BRICK_X;
        int y = S_BRICK_Y;
        for (int i = BOX_0; i < BOX_END; i++)
        {
            a::menu()->DrawBrick(x, y, i);
            x += BRICK_W;
            if (x >= (SCREEN_WIDTH - 10 * BRICK_W) / 2 + 10 * BRICK_W)
            {
                x = (SCREEN_WIDTH - 10 * BRICK_W) / 2;
                y += BRICK_H;
            }
        }
        nX = (g_cursorPosition.x - S_BRICK_X) / BRICK_W;
        nY = (g_cursorPosition.y - S_BRICK_Y) / BRICK_H;
        if (nX >= 0 && nX < 10 && nY >= 0 && nY < 9)
        {
            SetRect(&rc, 0, 0, BRICK_W, BRICK_H);
            render(S_BRICK_X + nX * BRICK_W, S_BRICK_Y + nY * BRICK_H, eImage::Transp, &rc);
            if (g_bMouseLB == true)
            {
                g_bMouseLB = false;
                m_bIsSelectBrickMode = false;
                m_nBrickType = 1 + nX + nY * 10;
            }
        }
    }
    else
    {
        nX = (g_cursorPosition.x - BRICK_X) / BRICK_W;
        nY = (g_cursorPosition.y - BRICK_Y) / BRICK_H;
        SetRect(&rc, 0, 0, BRICK_W, BRICK_H);
        if (nX >= 0 && nX < BRICKS_WIDTH && nY >= 0 && nY < BRICKS_HEIGHT)
        {
            render(BRICK_X + nX * BRICK_W, BRICK_Y + nY * BRICK_H, eImage::Transp, &rc);
            if (g_bMouseLB == true)
            {
                m_level[nX][nY] = m_nBrickType;
            }
            if (g_bMouseRB == true)
            {
                m_level[nX][nY] = BOX_NONE;
            }
        }
        rc.w = 160;
        for (int i = 0; i < BRICKS_HEIGHT; i++)
        {
            render(BRICK_X + BRICKS_WIDTH * BRICK_W + 5, BRICK_Y + i * BRICK_H, eImage::Transp, &rc);
            render(BRICK_X + BRICKS_WIDTH * BRICK_W + 5 + 160, BRICK_Y + i * BRICK_H, eImage::Transp, &rc);
        }
        a::fnt1()->DrawString(BRICK_X + BRICKS_WIDTH * BRICK_W + 15, BRICK_Y + 15,
                              "<Arrows> - roll level.\n"
                              "<Page Up> / <Page Down> - change level.\n"
                              "<Tab> - edit mode / selection mode.\n"
                              "<Delete> - clear level.\n"
                              "<Ctrl> + <Delete> - delete current level.\n"
                              "<Ctrl> + <Insert> - insert empty level.\n"
                              "<Esc> - save and exit to game menu.\n"
                              "NOTE: No undo availabe.");

        if (IsKeyPressed(SDLK_DELETE) && IsKeyStateChanged(SDLK_DELETE))
        {
            memset(m_level, 0, sizeof(m_level));
        }
        if (((true == a::menu()->DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 30, WALL_Y2 - 30, CMainMenu::B_PREV) && g_bMouseLB == true) || (IsKeyPressed(SDLK_PAGEUP) && IsKeyStateChanged(SDLK_PAGEUP))) && m_nCurrentLevel > 0)
        {
            g_bMouseLB = false;
            memcpy(m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_level, sizeof(m_level));
            m_nCurrentLevel--;
            memcpy(m_level, m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_level));
        }
        if ((true == a::menu()->DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 100, WALL_Y2 - 30, CMainMenu::B_NEXT) && g_bMouseLB == true) || (IsKeyPressed(SDLK_PAGEDOWN) && IsKeyStateChanged(SDLK_PAGEDOWN)))
        {
            g_bMouseLB = false;
            memcpy(m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_level, sizeof(m_level));
            if (m_nCurrentLevel == m_nLevelsCount - 1)
            {
                Uint8* pTmp = new Uint8[(m_nLevelsCount + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT)];
                memcpy(pTmp, m_levels, m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT));
                delete[] m_levels;
                m_levels = pTmp;
                m_nLevelsCount++;
                memset(m_levels + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), 0, sizeof(m_level));
            }
            m_nCurrentLevel++;
            memcpy(m_level, m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_level));
        }
        if (IsKeyPressed(SDLK_UP) && IsKeyStateChanged(SDLK_UP))
        {
            for (int x = 0; x < BRICKS_WIDTH; x++)
            {
                Uint8 byTemp = m_level[x][0];
                for (int y = 0; y < BRICKS_HEIGHT - 1; y++)
                {
                    m_level[x][y] = m_level[x][y + 1];
                }
                m_level[x][BRICKS_HEIGHT - 1] = byTemp;
            }
        }
        if (IsKeyPressed(SDLK_DOWN) && IsKeyStateChanged(SDLK_DOWN))
        {
            for (int x = 0; x < BRICKS_WIDTH; x++)
            {
                Uint8 byTemp = m_level[x][BRICKS_HEIGHT - 1];
                for (int y = BRICKS_HEIGHT - 1; y > 0; y--)
                {
                    m_level[x][y] = m_level[x][y - 1];
                }
                m_level[x][0] = byTemp;
            }
        }
        if (IsKeyPressed(SDLK_LEFT) && IsKeyStateChanged(SDLK_LEFT))
        {
            for (int y = 0; y < BRICKS_HEIGHT; y++)
            {
                Uint8 byTemp = m_level[0][y];
                for (int x = 1; x < BRICKS_WIDTH; x++)
                {
                    m_level[x - 1][y] = m_level[x][y];
                }
                m_level[BRICKS_WIDTH - 1][y] = byTemp;
            }
        }
        if (IsKeyPressed(SDLK_RIGHT) && IsKeyStateChanged(SDLK_RIGHT))
        {
            for (int y = 0; y < BRICKS_HEIGHT; y++)
            {
                Uint8 byTemp = m_level[BRICKS_WIDTH - 1][y];
                for (int x = BRICKS_WIDTH - 1; x > 0; x--)
                {
                    m_level[x][y] = m_level[x - 1][y];
                }
                m_level[0][y] = byTemp;
            }
        }
        if (g_modState & KMOD_CTRL && IsKeyPressed(SDLK_DELETE) && IsKeyStateChanged(SDLK_DELETE))
        {
            if (m_nLevelsCount > 1)
            {
                m_nLevelsCount--;
                Uint8* pTmp = new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
                memcpy(pTmp, m_levels, m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT));
                memcpy(pTmp + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_levels + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), (m_nLevelsCount - m_nCurrentLevel) * (BRICKS_WIDTH * BRICKS_HEIGHT));
                delete[] m_levels;
                m_levels = pTmp;
                m_nCurrentLevel = std::min(m_nCurrentLevel, m_nLevelsCount - 1);
                memcpy(m_level, m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_level));
            }
        }
        if (g_modState & KMOD_CTRL && IsKeyPressed(SDLK_INSERT) && IsKeyStateChanged(SDLK_INSERT))
        {
            m_nLevelsCount++;
            Uint8* pTmp = new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
            memcpy(pTmp, m_levels, m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT));
            memcpy(pTmp + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), (m_nLevelsCount - m_nCurrentLevel - 1) * (BRICKS_WIDTH * BRICKS_HEIGHT));
            delete[] m_levels;
            m_levels = pTmp;
            memset(m_level, 0, sizeof(m_level));
        }
    }

    SetRect(&rc, 0, 0, 30, 40);
    render(5, 5, eImage::Transp, &rc);
    a::menu()->DrawBrick(10, 10, m_nBrickType);
}

void CLevelEditor::Load()
{
    delete[] m_levels;

    char buffer[MAX_PATH];
    snprintf(buffer, sizeof(buffer), "%scustomlevels", a::userProfile());

    FILE* file = fopen(buffer, "rb");
    if (file != nullptr)
    {
        fseek(file, 0, SEEK_END);
        m_nLevelsCount = ftell(file) / (BRICKS_WIDTH * BRICKS_HEIGHT);
        if (m_nLevelsCount > 0)
        {
            rewind(file);
            m_levels = new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
            fread(m_levels, (BRICKS_WIDTH * BRICKS_HEIGHT), m_nLevelsCount, file);
        }
        else
        {
            m_nLevelsCount = 1;
            m_levels = new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
            memset(m_level, 0, sizeof(m_level));
        }
        fclose(file);
    }
    else
    {
        m_nLevelsCount = 1;
        m_levels = new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
        memset(m_levels, 0, sizeof(m_level));
    }
    m_nCurrentLevel %= m_nLevelsCount;
    memcpy(m_level, m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_level));
    printf("Custom levels: %d loaded.\n", m_nLevelsCount);
}

void CLevelEditor::Save()
{
    if (m_levels != nullptr)
    {
        char buffer[MAX_PATH];
        snprintf(buffer, sizeof(buffer), "%scustomlevels", a::userProfile());
        FILE* file = fopen(buffer, "wb");
        if (file != nullptr)
        {
            memcpy(m_levels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_level, sizeof(m_level));
            fwrite(m_levels, (BRICKS_WIDTH * BRICKS_HEIGHT), m_nLevelsCount, file);
            fclose(file);
        }
        printf("Custom levels: %d saved.\n", m_nLevelsCount);
    }
}
