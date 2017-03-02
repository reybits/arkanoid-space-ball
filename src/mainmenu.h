/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#define MENU_ITEM_Y 235
#define MENU_ITEM_X ((SCREEN_WIDTH - 248) / 2)

#include <cstdint>

struct sOptions;
struct sHighscores;

class CMainMenu final
{
public:
    CMainMenu(sOptions& options, sHighscores& highscores);
    ~CMainMenu();

    void SetMenuType(int nType, bool bReturnToGame = false);
    int DrawMenu();
    void SetEnterNameMode();
    bool DrawMenuButton(int nX, int nY, int nButtonId);
    void DrawBrick(int nX, int nY, int nType);
    void SendEsc();

    enum { MT_MAIN, MT_STARTGAME, MT_HIGHSCORE, MT_OPTIONS, MT_RULES, MT_QUIT };
    enum { B_STARTGAME, B_HIGHSCORE, B_OPTIONS, B_HELP, B_EXIT, B_REGISTER, B_OK, B_CANCEL, B_PREV, B_NEXT, B_NEWGAME, B_RESTORE, B_BUYGAME, B_LEVELEDITOR, B_CUSTOMLEVELS };

private:
    char GetKey();
    void DrawMenuOptions();
    void DrawMenuHighScore();
    void DrawMenuRules();
    void PlaySFX(int nItem);
    int DrawMenuMain();
    int DrawStartGame();
    void DrawBackground();

protected:
    sOptions& m_options;
    sHighscores& m_highscores;
    bool m_optionsDirty;

    bool m_bGetNameMode;
    int m_nMenuType;
    bool m_bInitHelp;
    bool m_bReturnToGame;
    bool m_bIsSaveAvailable;
    char m_achStoredLevelInfo[100];
    char m_achName[102];
};
