// MainMenu.h: interface for the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////

#ifndef MAINMENU_H_3C101185416525
#define MAINMENU_H_3C101185416525

#define MENU_ITEM_Y 235
#define MENU_ITEM_X ((SCREEN_WIDTH - 248) / 2)

class CMainMenu
{
public:
    CMainMenu();
    virtual ~CMainMenu();

    void SetMenuType(int nType, bool bReturnToGame = false);
    int DrawMenu();
    void SetEnterNameMode();
    bool DrawMenuButton(int nX, int nY, int nButtonId);
    void DrawBrick(int nX, int nY, int nType);
    void SendEsc();

    enum { MT_MAIN, MT_STARTGAME, MT_HIGHSCORE, MT_OPTIONS, MT_RULES, MT_QUIT };
    enum { B_STARTGAME, B_HIGHSCORE, B_OPTIONS, B_HELP, B_EXIT, B_REGISTER, B_OK, B_CANCEL, B_PREV, B_NEXT, B_NEWGAME, B_RESTORE, B_BUYGAME, B_LEVELEDITOR, B_CUSTOMLEVELS };

protected:
    bool m_bGetNameMode;
    int m_nMenuType;
    bool m_bInitHelp;
    bool m_bReturnToGame;
    bool m_bIsSaveAvailable;
    char m_achStoredLevelInfo[100];
    char m_achName[102];
    struct _OPTIONS
    {
        int nBppIndex;
        bool bFullscreen;
        bool bOGL;
    };
    struct _OPTIONS m_strOpt;

private:
    char GetKey();
    void DrawMenuOptions();
    void DrawMenuHighScore();
    void DrawMenuRules();
    void PlaySFX(int nItem);
    int DrawMenuMain();
    int DrawStartGame();
    void DrawBackground();

#if !defined(__linux__) && !defined(FULL_VERSION)
    void DrawEnterReg();
    void DrawHighlight(int nPos);
#endif
};

#endif /* end of include guard: MAINMENU_H_3C101185416525 */

