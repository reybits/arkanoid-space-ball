// MainMenu.h: interface for the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINMENU_H__F6055D1D_17C9_4442_A253_222767E6A181__INCLUDED_)
#define AFX_MAINMENU_H__F6055D1D_17C9_4442_A253_222767E6A181__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainMenu {
public:
	enum { MT_MAIN, MT_STARTGAME, MT_HIGHSCORE, MT_OPTIONS, MT_RULES, MT_QUIT, MT_REG_KEY };
	enum { B_STARTGAME, B_HIGHSCORE, B_OPTIONS, B_HELP, B_EXIT, B_REGISTER, B_OK, B_CANCEL, B_PREV, B_NEXT, B_NEWGAME, B_RESTORE, B_BUYGAME, B_LEVELEDITOR, B_CUSTOMLEVELS };
	void SetMenuType(int nType, bool bReturnToGame = false);
	int DrawMenu();
	CMainMenu();
	virtual ~CMainMenu();
	void SetEnterNameMode();
	bool DrawMenuButton(int nX, int nY, int nButtonId);
	void DrawBrick(int nX, int nY, int nType);
	void SendEsc();

protected:
#define	MENU_ITEM_Y	235
#define	MENU_ITEM_X	((SCREEN_WIDTH - 248) / 2)
	struct _OPTIONS {
		int nBppIndex;
		bool bFullscreen;
		bool bOGL;
	} m_strOpt;
	char m_achName[102];
	bool m_bGetNameMode;
	char GetKey();
	void DrawMenuOptions();
	void DrawMenuHighScore();
	void DrawMenuRules();
	void PlaySFX(int nItem);
	int DrawMenuMain();
	int DrawStartGame();
	int m_nMenuType;
	void DrawBackground();
#if !defined(__linux__) && !defined(FULL_VERSION)
	void DrawEnterReg();
	void DrawHighlight(int nPos);
	bool m_bShowReminder;
#endif
	bool m_bInitHelp;
	bool m_bReturnToGame;
	char m_achStoredLevelInfo[100];
	bool m_bIsSaveAvailable;
};

#endif // !defined(AFX_MAINMENU_H__F6055D1D_17C9_4442_A253_222767E6A181__INCLUDED_)
