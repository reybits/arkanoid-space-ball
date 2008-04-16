// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "arkanoidsb.h"
#include "mainmenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainMenu::CMainMenu() {
	m_bGetNameMode	= false;
	m_bInitHelp		= true;
#if !defined(__linux__) && !defined(FULL_VERSION)
	m_bShowReminder	= false;
#endif
	m_bReturnToGame	= false;
	m_achStoredLevelInfo[0]	= 0;
}

CMainMenu::~CMainMenu() {
}

int CMainMenu::DrawMenu() {
	int	nRetCode	= -1;
	DrawBackground();

	switch(m_nMenuType) {
	case MT_MAIN:
		nRetCode	= DrawMenuMain();
		break;
	case MT_STARTGAME:
		nRetCode	= DrawStartGame();
		break;
	case MT_RULES:
		DrawMenuRules();
		break;
	case MT_HIGHSCORE:
		DrawMenuHighScore();
		break;
#if !defined(__linux__) && !defined(FULL_VERSION)
	case MT_REG_KEY:
		DrawEnterReg();
		break;
#endif
	case MT_OPTIONS:
		DrawMenuOptions();
		break;
	}

	const char achTips[]	=
			"TIP: Try change video mode to increase performance.   "\
			"TIP: Kamikaze and fiery meteorite blows up the paddle.   "\
			"TIP: Ice-meteorite freezes up the paddle for 3 seconds.   "\
			"TIP: To gain additional scores try to destroy very strong bricks.   "\
			"TIP: Nuclear bomb blows up some bricks around.   "\
			"TIP: Cannon fires on you (shots are frozen or invert paddle movement).   "\
			"TIP: Red brick always has a bonus.   "\
			"TIP: Right mouse button - return to main menu.   "\
			" ";
	static bool	bInit	= true;
	if(bInit == true) {
		bInit	= false;
		g_CSinusString.SetString(achTips, SCREEN_WIDTH, 420);
	}
	if(m_nMenuType == MT_MAIN)	g_CSinusString.Draw();

	if(m_nMenuType == MT_MAIN) {
		g_Font.DrawString(5, SCREEN_HEIGHT - 5 - 14, GAME_VERSION);
#if !defined(__linux__) && !defined(FULL_VERSION)
		if(g_bIsRegistered == false) {
			g_Font.DrawString(5, 5, "Unregistered version");
		}
		else {
			char	achBuf[sizeof(g_achRegName) + 20];
			sprintf(achBuf, "Registered to: %s", g_achRegName);
			g_Font.DrawString(5, 5, achBuf);
		}
#else
#	if defined(__linux__)
		g_Font.DrawString(5, 5, "Registered to all Linux users");
#	endif
#endif
	}
	else if(
#if !defined(__linux__) && !defined(FULL_VERSION)
			m_bShowReminder == false &&
#endif
			((IsKeyPressed(SDLK_ESCAPE) && IsKeyStateChanged(SDLK_ESCAPE)) || g_bMouseRB == true)) {
		SendEsc();
	}

	return	nRetCode;
}

void CMainMenu::DrawBackground() {
	if(m_nMenuType == MT_MAIN) {

		Blit(0, 0, m_pBackground, 0);
	}
	else {
		Blit(0, 0, m_pBackground2, 0);
	}
}

int CMainMenu::DrawMenuMain() {
	SDL_Rect	rc;
	SetRect(&rc, 0, 0, 248, 29);

	int nItem	= -1;
#if !defined(__linux__) && !defined(FULL_VERSION)
	int	nCount	= (g_bIsRegistered == true ? 5 : 6);
#else
	int	nCount	= 5;
#endif
	for(int i = 0; i < nCount; i++) {
		if(true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y + i * 29, i)) {
			nItem	= i;
		}
	}

	if(g_bMouseLB == true) {
		g_bMouseLB	= false;
		switch(nItem + 1) {
		case MT_QUIT:	return 0;		// exit
#if !defined(__linux__) && !defined(FULL_VERSION)
		case MT_REG_KEY:	if(g_bIsRegistered == false)	SetMenuType(nItem + 1);	break;
#endif
		default:	SetMenuType(nItem + 1);	break;
		}
	}

	return -1;	// do nothing
}

int CMainMenu::DrawStartGame() {
	//g_Font2.DrawString(0, MENU_ITEM_Y - 60, "SELECT APPROPRIATE ACTION", 2);
	int nItem	= -1;
	if(true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80, B_NEWGAME)) {
		nItem	= 0;
	}
	if(true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 29, B_RESTORE)) {
		if(m_bIsSaveAvailable == true)	nItem	= 1;
	}
	if(true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 58, B_CUSTOMLEVELS)) {
		nItem	= 7;
	}
	if(true == DrawMenuButton(MENU_ITEM_X, MENU_ITEM_Y - 80 + 87, B_LEVELEDITOR)) {
		nItem	= 8;
	}
	if(true == DrawMenuButton(MENU_ITEM_X + 62, MENU_ITEM_Y - 80 + 116, B_CANCEL)) {
		nItem	= 6;
	}
	g_Font2.DrawString(0, MENU_ITEM_Y - 110, m_achStoredLevelInfo, 2);
	g_Font.DrawString(0, MENU_ITEM_Y + 120, "Do you wish to get some help while playing (Tutorial)?", 2);
	g_Font2.DrawString(0, MENU_ITEM_Y + 120 + 20 + 2, (g_bTutorialMode == true ? "YES" : "NO"), 2);
	if(true == DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, MENU_ITEM_Y + 120 + 20, B_PREV)) {
		nItem	= 2;
	}
	if(true == DrawMenuButton(SCREEN_WIDTH / 2 + 40, MENU_ITEM_Y + 120 + 20, B_NEXT)) {
		nItem	= 3;
	}
	g_Font.DrawString(0, MENU_ITEM_Y + 170, "Do you wish to auto use bonuses (like usual arkanoid)?", 2);
	g_Font2.DrawString(0, MENU_ITEM_Y + 170 + 20 + 2, (g_bAutoBonusMode == true ? "YES" : "NO"), 2);
	if(true == DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, MENU_ITEM_Y + 170 + 20, B_PREV)) {
		nItem	= 4;
	}
	if(true == DrawMenuButton(SCREEN_WIDTH / 2 + 40, MENU_ITEM_Y + 170 + 20, B_NEXT)) {
		nItem	= 5;
	}

#if !defined(__linux__) && !defined(FULL_VERSION)
	if(m_bShowReminder == true) {
		switch(g_ReminderDlg.Draw(2)) {
		case 1:
			SetMenuType(MT_REG_KEY);
			g_nGameMode		= APPS_MAINMENU;
			break;
		case 2:
			m_bShowReminder	= false;
			//EnableCursor(true);
			break;
		}
	}
	else
#endif
		if(g_bMouseLB == true) {
		g_bMouseLB	= false;
		switch(nItem) {
		case 0:	return 1;	// start new game
		case 1:
#if !defined(__linux__) && !defined(FULL_VERSION)
			if(g_bIsRegistered == true)
#endif
				return 2;	// restore game
#if !defined(__linux__) && !defined(FULL_VERSION)
			else
				m_bShowReminder	= true;
#endif
			break;
		case 2:
		case 3:	g_bTutorialMode	= !g_bTutorialMode;	break;
		case 4:
		case 5:	g_bAutoBonusMode	= !g_bAutoBonusMode;	break;
		case 6:	m_nMenuType	= MT_MAIN;	break;
		case 7:	return 3;	// custom levels
		case 8:	return 4;	// level editor
		}
	}

	return -1;	// do nothing
}

void CMainMenu::PlaySFX(int nItem) {
	static int	nPrevItem	= -1;
	if(nItem != -1 && nItem != nPrevItem) {
		PlaySound(5);//	g_pMainFrame->m_pImix->SamplePlay(m_nSampleOver);
	}
	nPrevItem	= nItem;
}

void CMainMenu::DrawMenuRules() {
	SDL_Rect	rc;
	int	i	= 120;

	static int	nPage	= 0;

	if(nPage == 0) {
		g_Font2.DrawString(0, 20, "BASIC CONTROLS", CMyString::FONT_ALIGN_CENTER);
		g_Font.SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
		g_Font.DrawString2(0,  50, "To manage your paddle, simply move your mouse in the direction you want the paddle to move. The Left mouse button will launch the sphere into play and shoot your weapons - if you have collected the appropriate power-up. The Esc key, or Pause key will pause the game and bring up menu screens.");

		g_Font2.DrawString(0, 180, "GOALS", CMyString::FONT_ALIGN_CENTER);
		g_Font.DrawString2(0, 210, "There are two main goals of \"Arkanoid: Space Ball\":");
		g_Font.DrawString2(0, 230, "a) Keep your ball in play for as long as possible to advance to the next round.");
		g_Font.DrawString2(0, 270, "b) Accumulate the highest score possible by breaking as many bricks, collecting as many power-ups, and destroying as many monsters as you can.");
		g_Font.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else if(nPage == 1) {
		g_Font2.DrawString(0, 20, "RULES OF PLAY", CMyString::FONT_ALIGN_CENTER);
		g_Font.SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
		g_Font.DrawString2(0,  50, "To start, you are given three lives. The number of reserve lives is displayed on the scoreboard on the top of the screen. The game is over if you have no reserve spheres. Every 5000 points will get you a reserve life. Each level in \"Arkanoid: Space Ball\" is comprised of a varying number of bricks. Most of these bricks are destructible by bouncing your ball off of them (or shooting them) between one and four times. When all such one-to-four-hit bricks have been cleared, you will have successfully completed the level and will automatically advance to the next rlevel. Note that many levels contain 15-time-hit bricks that do not need to be cleared. All levels of the game are grouped into sets of 5. When you will complete a given set of 5 levels you reach a Bonus level. You have to reach as many scores as you can by shaking down enemy gunnery or debris flying on you at a Bonus level. If you have a Bonus level with gunnery you have to pass 5 balls. In case of debris �you have to pass 3 pushing off debris with paddle.\nThere are two modes of the game: the first one is usual when bonuses activate by taking them at once. The second mode is an accumulation of bonuses when you may chose any balls for using.");
		//g_Font.DrawString(0, 350, "�� �� ������ ��������� 3 ������������ �������� � ��������.");
		g_Font.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else if(nPage == 2) {
		g_Font2.DrawString(0, 20, "BRICKS", CMyString::FONT_ALIGN_CENTER);
		g_Font.DrawString(0, 50, "One strike bricks (easy to destroy). Red animated brick always has a bonus.", CMyString::FONT_ALIGN_CENTER);
		for(i = BOX_0; i < BOX_DBL_0; i++) {
			DrawBrick(70 + ((i - 1) % 25) * 20, 80 + ((i - 1) / 25) * 30, i);
		}
		g_Font.DrawString(0, 160, "Strong bricks (4-strike briks)", CMyString::FONT_ALIGN_CENTER);
		for(i = BOX_DBL_0; i < BOX_IM_0; i++) {
			DrawBrick(220 + ((i - BOX_DBL_0) % 10) * 20, 190 + ((i - BOX_DBL_0) / 10) * 30, i);
		}
		g_Font.DrawString(0, 300, "Very strong (3), movable (3), mortiras (2), bombs (2)", CMyString::FONT_ALIGN_CENTER);
		int	x = (SCREEN_WIDTH - (BOX_END - BOX_IM_0) * 20 - 3 * 15) / 2;
		for(i = BOX_IM_0; i < BOX_END; i++) {
			if(i == BOX_MOV_0 || i == BOX_SHOOT_0 || i == BOX_BOMB_0)	x	+= 15;
			DrawBrick(x, 330, i);
			x	+= BRICK_W;
		}
	}
	else if(nPage == 3) {
		g_Font2.DrawString(0, 20, "POWER-UPS", CMyString::FONT_ALIGN_CENTER);
		g_Font.SetRect(100, 0, SCREEN_WIDTH - 200, SCREEN_HEIGHT);
		g_Font.DrawString2(0, 50, "Power-ups are available across all environments. They have the potential to both help and hinder you as you try to complete each round.");
		g_Font.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		static int		anFrameBonus[]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1 };
		static Uint32	dwTimeBonus	= 0;
		bool				bFrameStep	= false;
		if(dwTimeBonus + 30 < SDL_GetTicks()) {
			dwTimeBonus	= SDL_GetTicks();
			bFrameStep	= true;
		}
		const char	*pachDescr[]	= {
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
		for(int nBon = 0; nBon < CBonus::TYPE_LAST_BONUS; nBon++) {
			rc.x	= 36 * anFrameBonus[nBon];
			rc.y	= 36 * nBon;
			rc.w	= 36;	rc.h	= 36;
			int	nX	= 45;
			if(nBon % 2 == 1)	nX	+= SCREEN_WIDTH / 2;
			Blit(nX, i, m_pBonuses, &rc);
// 			if(nBon == CBonus::TYPE_BALL_BLUE || nBon == CBonus::TYPE_BALL_SHRINK || nBon == CBonus::TYPE_PADDLE_SHRINK) {
// 				Blit(nX - 12, i - 12, g_pBonusesAura, 0);
// 			}
			g_Font.DrawString(nX + 45, i + 8, pachDescr[nBon]);
			if(nBon % 2 == 1)	i	+= 36;
			if(bFrameStep == true) {
				anFrameBonus[nBon]++;
				anFrameBonus[nBon]	%= 15;
			}
		}
		g_Font3.DrawString(0, 400, "(*) - available only in registered version", 2);
	}
	else if(nPage == 4) {
		if(m_bInitHelp == true) {
			m_bInitHelp	= false;
			g_Monster.RemoveAll();
			for(int i = CMonster::MONST_PATROL; i < CMonster::MONST_END; i++) {
				g_Monster.AddMonster(40 + (i % 2 == 1 ? SCREEN_WIDTH / 2 : 0), 100 + (i / 2) * 60, i);
			}
		}
		g_Font2.DrawString(0, 20, "OBJECTS", CMyString::FONT_ALIGN_CENTER);
		g_Monster.Draw();
		const char	*pachDescr[]	= {
			"It's a kamikaze - this monster blows up the paddle.", "Follows the nearest ball.",
			"Robotic eye follows your paddle.", "Disturb to ball moving",
			"This monster disturb to ball moving.", "Robotic hand catches your paddle and holds it at the moment.",
			"This monster disturb to ball moving.", "This monster disturb to ball moving.",
			"Fiery meteorite blows up the paddle.", "Ice-meteorite freezes up the paddle for 3 seconds."
		};
		for(int nBon = 0; nBon < 10; nBon++) {
			int	nX	= 120;
			if(nBon % 2 == 1) nX	+= SCREEN_WIDTH / 2;
			g_Font.SetRect(nX, 0, SCREEN_WIDTH / 2 - 120 - 40, SCREEN_HEIGHT);
			g_Font.DrawString2(0, 100 + (nBon / 2) * 60, pachDescr[nBon]);
		}
		g_Font.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	else if(nPage == 5) {
		g_Font2.DrawString(0, 100, "CREDITS", CMyString::FONT_ALIGN_CENTER);
		g_Font3.DrawString(0, 130, "'WE' Group", 2);
#if !defined(REMOVE_URLS)
		g_Font.DrawString(0,  150, "http://www.wegroup.org", 2);
		g_Font.DrawString(0,  170, "Copyright (c) 2006-2007. All Rights Reserved.", 2);
		g_Font2.DrawString(0, 220, "THANKS TO", CMyString::FONT_ALIGN_CENTER);
		g_Font3.DrawString(0, 250, "SDL, SDL-image, SDL-mixer projects", 2);
		g_Font.DrawString(0,  270, "http://www.libsdl.org", 2);
		g_Font3.DrawString(0, 290, "glSDL / David Olofson", 2);
		g_Font.DrawString(0,  310, "http://www.olofson.net", 2);
		g_Font3.DrawString(0, 330, "Music / Sergey Eybog", 2);
		g_Font.DrawString(0,  350, "http://www.lesser-vibes.com", 2);
#else
		g_Font.DrawString(0,  150, "Copyright (c) 2006-2007. All Rights Reserved.", 2);
		g_Font2.DrawString(0, 220, "THANKS TO", CMyString::FONT_ALIGN_CENTER);
		g_Font3.DrawString(0, 250, "SDL, SDL-image, SDL-mixer projects", 2);
		g_Font3.DrawString(0, 270, "glSDL / David Olofson", 2);
		g_Font3.DrawString(0, 290, "Music / Sergey Eybog", 2);
#endif
	}

	if(IsKeyPressed(SDLK_LEFT) && IsKeyStateChanged(SDLK_LEFT) && nPage > 0) {
		nPage--;
	}
	if(IsKeyPressed(SDLK_RIGHT) && IsKeyStateChanged(SDLK_RIGHT) && nPage < 5) {
		nPage++;
	}

	char	achBuf[20];
	sprintf(achBuf, "Page %d of %d", nPage + 1, 6);
	g_Font.DrawString(10, SCREEN_HEIGHT - 22, achBuf, CMyString::FONT_ALIGN_RIGHT);

	int nItem	= -1;
	if(true == DrawMenuButton(MENU_ITEM_X + 62, 430, B_OK)) {
		nItem	= 0;
	}
	if(nPage > 0 && true == DrawMenuButton(SCREEN_WIDTH / 2 - 62 - 77 - 10, 430, B_PREV)) {
		nItem	= 1;
	}
	if(nPage < 5 && true == DrawMenuButton(SCREEN_WIDTH / 2 + 62 + 10, 430, B_NEXT)) {
		nItem	= 2;
	}

	if(g_bMouseLB == true) {
		g_bMouseLB	= false;
		switch(nItem) {
		case 0:	m_nMenuType	= MT_MAIN;	break;
		case 1:	if(nPage > 0)	nPage--;	break;
		case 2:	if(nPage < 5)	nPage++;	break;
		}
	}
}

void CMainMenu::DrawBrick(int nX, int nY, int nType) {
	SDL_Rect	src;
	static int		nFrame	= 0;
	static Uint32	dwTime	= 0;
	static bool		bDir		= true;
	if(dwTime + 100 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		nFrame++;	nFrame	%= 5;
		if(!nFrame)	bDir	= !bDir;
	}

	if(nType < BOX_40) {
		src.y	= ((nType - 1) / 10) * BRICK_H;
	}
	else if(nType < BOX_DBL_0) {
		if(bDir || nType == BOX_49)
			src.y	= ((nType - 1) / 10 + nFrame) * BRICK_H;
		else
			src.y	= ((nType - 1) / 10 + 4 - nFrame) * BRICK_H;
	}
	else {
		src.y	= ((nType - 1) / 10 + 4) * BRICK_H;
	}
	src.x	= ((nType - 1) % 10) * BRICK_W;
	src.w	= BRICK_W;
	src.h	= BRICK_H;
	Blit(nX, nY, m_pBricks, &src);
}

void CMainMenu::DrawMenuHighScore() {
	int	nScoreY			= 130;
	char	achBuf[102];

	g_Font2.DrawString(20, nScoreY - 20, "BEST PLAYERS NAME");
	g_Font2.DrawString(100, nScoreY - 20, "SCORE", 1);
	g_Font2.DrawString(20, nScoreY - 20, "LEV", 1);
	for(int i = 0; i < 10; i++) {
		if(m_nLastHiScorePos == i) {
			g_Font2.DrawNumber(i + 1, 20, (i + 1) * 20 + nScoreY);
			if(m_bGetNameMode == false) {
				g_Font3.DrawString(60, (i + 1) * 20 + nScoreY, g_strHighScore[i].achName);
			}
			else {
				g_Font.DrawString(0, 80, "Enter Your name, please", CMyString::FONT_ALIGN_CENTER);
				int	nLen	= strlen(m_achName);
				if(nLen < 100) {
					m_achName[nLen]		= GetKey();
					m_achName[nLen + 1]	= 0;
				}
				static Uint32	dwTime	= 0;
				static bool	bCursor		= true;
				if(dwTime + 500 < SDL_GetTicks()) {
					dwTime	= SDL_GetTicks();
					bCursor	= !bCursor;
				}
				sprintf(achBuf, "%s%s", m_achName, bCursor == true ? "_" : "");
				g_Font3.DrawString(60, (i + 1) * 20 + nScoreY, achBuf);

				if(IsKeyPressed(SDLK_RETURN) && IsKeyStateChanged(SDLK_RETURN)) {
					if(strlen(m_achName) == 0) {
						strcpy(m_achName, "Anonymous");
					}
					else {
						strcpy(g_strHighScore[m_nLastHiScorePos].achName, m_achName);
					}
					m_bGetNameMode	= false;
				}
				//nLen	= strlen(m_achName);
				if(IsKeyPressed(SDLK_BACKSPACE) && IsKeyStateChanged(SDLK_BACKSPACE) && nLen > 0) {
					m_achName[nLen - 1]	= 0;
				}
			}
			g_Font2.DrawNumber(g_strHighScore[i].nScore, 100, (i + 1) * 20 + nScoreY, 1);
			g_Font2.DrawNumber(g_strHighScore[i].nLevel, 20, (i + 1) * 20 + nScoreY, 1);
		}
		else {
			g_Font2.DrawNumber(i + 1, 20, (i + 1) * 20 + nScoreY);
			g_Font.DrawString(60, (i + 1) * 20 + nScoreY, g_strHighScore[i].achName);
			g_Font2.DrawNumber(g_strHighScore[i].nScore, 100, (i + 1) * 20 + nScoreY, 1);
			g_Font2.DrawNumber(g_strHighScore[i].nLevel, 20, (i + 1) * 20 + nScoreY, 1);
		}
	}

	if(m_bGetNameMode == false) {
		if(true == DrawMenuButton(MENU_ITEM_X + 62, 430, B_OK)) {
			if(g_bMouseLB == true) {
				g_bMouseLB	= false;
				m_nMenuType	= MT_MAIN;
			}
		}
	}
}

void CMainMenu::DrawMenuOptions() {
	bool	bChanged	= false;
	if(m_strOpt.bOGL != g_bOGL || m_strOpt.nBppIndex != g_nBppIndex || m_strOpt.bFullscreen != g_bFullscreen) {
		bChanged	= true;
	}
	int	nPos		= -1;
	int	nButton	= -1;
	for(int i = 0; i < 5; i++) {
		if(true == DrawMenuButton(SCREEN_WIDTH / 2 - 77 - 40, 100 + i * 50, B_PREV)) {
			nPos		= i;
			nButton	= 0;
		}
		if(true == DrawMenuButton(SCREEN_WIDTH / 2 + 40, 100 + i * 50, B_NEXT)) {
			nPos		= i;
			nButton	= 1;
		}
	}
	int nItem	= -1;
	if(bChanged == true) {
		if(true == DrawMenuButton(MENU_ITEM_X, 360, B_OK))	nItem	= 0;
		if(true == DrawMenuButton(MENU_ITEM_X + 124, 360, B_CANCEL))	nItem	= 1;
	}
	else {
		if(true == DrawMenuButton(MENU_ITEM_X + 62, 360, B_OK))	nItem	= 0;
	}

	if(g_bMouseLB == true) {
		g_bMouseLB	= false;

		if(nItem != -1) {
			g_bMouseLB	= false;
			if(bChanged == true && nItem == 0) {
				g_bOGL			= m_strOpt.bOGL;
				g_nBppIndex		= m_strOpt.nBppIndex;
				g_bFullscreen	= m_strOpt.bFullscreen;
				SwitchFullscreen();
			}
			else {
				m_nMenuType	= MT_MAIN;
				if(m_bReturnToGame == true) {
					g_nGameMode	= APPS_GAME;
				}
			}
		}

		switch(nPos) {
		case 0:
			if(nButton == 0)	g_nVolumeM--;
			if(nButton == 1)	g_nVolumeM++;
			SetVolumeMusic(g_nVolumeM);
			break;
		case 1:
			if(nButton == 0)	g_nVolumeS--;
			if(nButton == 1)	g_nVolumeS++;
			SetVolumeSound(g_nVolumeS);
			break;
		case 2:
			if(nButton != -1) {
				m_strOpt.bFullscreen	= !m_strOpt.bFullscreen;
			}
			break;
		case 3:
			if(nButton != -1) {
				if(nButton == 0)	m_strOpt.nBppIndex++;
				else					m_strOpt.nBppIndex--;
				if(m_strOpt.nBppIndex < 0)	m_strOpt.nBppIndex	= sizeof(g_anBpx) / sizeof(int) - 1;
				if(m_strOpt.nBppIndex >= sizeof(g_anBpx) / sizeof(int))	m_strOpt.nBppIndex	= 0;
			}
			break;
		case 4:
			if(nButton == 0 || nButton == 1) {
				m_strOpt.bOGL	= !m_strOpt.bOGL;
			}
			break;
		}
	}

	g_Font.DrawString(0, 100 - 15, "Music volume", 2);
	g_Font.DrawString(0, 150 - 15, "Sound Effects volume", 2);
	g_Font.DrawString(0, 200 - 15, "Fullscreen mode", 2);
	g_Font.DrawString(0, 250 - 15, "Color depth (bits-per-pixel)", 2);
	g_Font.DrawString(0, 300 - 15, "Use OpenGL rendering", 2);
	if(bChanged == true) {
		g_Font3.DrawString(0, 340, "Apply settings", 2);
	}

	char	achBuf[10];
	sprintf(achBuf, "%d%%", g_nVolumeM * 10);	g_Font2.DrawString(0, 100 + 2, achBuf, 2);//	g_Font2.DrawNumber(g_nVolumeM, 82, 180, 2);
	sprintf(achBuf, "%d%%", g_nVolumeS * 10);	g_Font2.DrawString(0, 150 + 2, achBuf, 2);//g_Font2.DrawNumber(g_nVolumeS, 82, 210, 2);
	g_Font2.DrawString(0, 200 + 2, (m_strOpt.bFullscreen == true ? "YES" : "NO"), 2);
	g_Font2.DrawNumber(g_anBpx[m_strOpt.nBppIndex], 0, 250 + 2, 2);
	g_Font2.DrawString(0, 300 + 2, (m_strOpt.bOGL == true ? "YES" : "NO"), 2);

	g_Font.DrawString(0, SCREEN_HEIGHT - 52, "<Shift> + <+> or <Shift> + <-> - increase/decrease music volume.\n"\
			"<+> or <->  - increase/decrease effects volume", CMyString::FONT_ALIGN_CENTER);
	g_Font.DrawString(0, SCREEN_HEIGHT - 20, "<Esc> return to main menu.", CMyString::FONT_ALIGN_CENTER);
}

void CMainMenu::SetMenuType(int nType, bool bReturnToGame) {
	EnableCursor(true);
	m_nMenuType				= nType;
#if !defined(__linux__) && !defined(FULL_VERSION)
	m_bShowReminder			= false;
#endif
	m_bInitHelp				= true;
	m_strOpt.nBppIndex		= g_nBppIndex;
	m_strOpt.bFullscreen	= g_bFullscreen;
	m_strOpt.bOGL			= g_bOGL;
	m_bReturnToGame			= bReturnToGame;
	// get stored state info
	if(nType == MT_STARTGAME) {
		char	achBuf[PATH_MAX];
		FILE	*pFile;
		_SAVE	str;
		sprintf(achBuf, "%ssave", g_achUserProfile);
		m_bIsSaveAvailable	= false;
		if((pFile = fopen(achBuf, "rb"))) {
			if(sizeof(_SAVE) == fread(&str, 1, sizeof(_SAVE), pFile)) {
				EncodeDecode(&str, sizeof(_SAVE));
				if(strncmp(str.achMagic, "WEGR", 4) == 0) {
					m_bIsSaveAvailable	= true;
					sprintf(m_achStoredLevelInfo, "LEVEL: %d, SCORE: %d, LIVES: %d", str.nLevel + 1, str.nScore, str.nLives);
				}
			}
			fclose(pFile);
		}
		if(m_bIsSaveAvailable == false) {
			strcpy(m_achStoredLevelInfo, "NO PREVIOUSLY STORED GAME AVAILABLE");
		}
	}
}

char CMainMenu::GetKey() {
	const Uint8	abyScans[]	= {
		/*		1    2    3    4    5    6    7    8    9    0    -    =		*/
		SDLK_1,SDLK_2,SDLK_3,SDLK_4,SDLK_5,SDLK_6,SDLK_7,SDLK_8,SDLK_9,SDLK_0,SDLK_MINUS,SDLK_EQUALS,
		/*		Q    W    E    R    T    Y    U    I    O    P    [    ]		*/
		SDLK_q,SDLK_w,SDLK_e,SDLK_r,SDLK_t,SDLK_y,SDLK_u,SDLK_i,SDLK_o,SDLK_p,SDLK_LEFTBRACKET,SDLK_RIGHTBRACKET,
		/*		A    S    D    F    G    H    J    K    L    ;    '    \		*/
		SDLK_a,SDLK_s,SDLK_d,SDLK_f,SDLK_g,SDLK_h,SDLK_j,SDLK_k,SDLK_l,SDLK_SEMICOLON,SDLK_QUOTE,SDLK_BACKSLASH,
		/*		Z    X    C    V    B    N    M    ,    .    /    Spc  `		*/
		SDLK_z,SDLK_x,SDLK_c,SDLK_v,SDLK_b,SDLK_n,SDLK_m,SDLK_COMMA,SDLK_PERIOD,SDLK_SLASH,SDLK_SPACE,SDLK_BACKQUOTE
	};
	const Uint8	abyKeysLover[]	= {
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','\\',
		'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ', '`',
	};
	const Uint8	abyKeysUpper[]	= {
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|',
		'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', ' ', '~',
	};

	for(size_t i = 0; i < sizeof(abyScans); i++) {
		if(IsKeyPressed(abyScans[i]) && IsKeyStateChanged(abyScans[i])) {
			if(g_dwModState & KMOD_SHIFT)
				return abyKeysUpper[i];
			else
				return abyKeysLover[i];
		}
	}
	return 0;
}


/*!
    \fn CMainMenu::SetEnterNameMode()
 */
void CMainMenu::SetEnterNameMode() {
	SetMenuType(MT_HIGHSCORE);
	m_bGetNameMode	= true;

	for(int i = 0; i < 10; i++) {
		if(m_nLastGameScore > g_strHighScore[i].nScore) {
			for(int a = 9; a > i; a--) {
				strcpy(g_strHighScore[a].achName, g_strHighScore[a - 1].achName);
				g_strHighScore[a].nLevel	= g_strHighScore[a - 1].nLevel;
				g_strHighScore[a].nScore	= g_strHighScore[a - 1].nScore;
			}
			memset(g_strHighScore[i].achName, 0, 100);
			g_strHighScore[i].nScore	= m_nLastGameScore;
			g_strHighScore[i].nLevel	= m_nLastGameLevel;
			strcpy(g_strHighScore[i].achName, "Anonymous");
			m_nLastHiScorePos	= i;
			return;
		}
	}
}

void SetPixel(int nX, int nY, Uint32 color) {
}

#if !defined(__linux__) && !defined(FULL_VERSION)
/*!
    \fn CMainMenu::DrawEnterReg()
 */
void CMainMenu::DrawEnterReg() {
	SDL_Rect	rc;
	static char		achBuf[sizeof(g_achRegName) + 1];
	static bool		bName		= true;
	static Uint32	dwTime	= 0;
	static bool		bCursor	= true;
	static bool		bShow		= false;
	if(dwTime + 500 < SDL_GetTicks()) {
		dwTime	= SDL_GetTicks();
		bCursor	= !bCursor;
	}

	int	nPos	= (g_nCursorY - 180) / 30;
	if(nPos == 0 || nPos == 1) {
		if(g_bMouseLB == true) {
			bName	= nPos == 0 ? true : false;
			bShow	= false;
		}
	}
	char	byChar	= GetKey();
	if(bName == true) {
		DrawHighlight(0);
		int	nLen	= strlen(g_achRegName);
		if(nLen < sizeof(g_achRegName) - 1) {
			g_achRegName[nLen]		= toupper(byChar);
			g_achRegName[nLen + 1]	= 0;
			//CheckRegistration();
		}
		if(IsKeyPressed(SDLK_BACKSPACE) && IsKeyStateChanged(SDLK_BACKSPACE) && nLen > 0) {
			g_achRegName[nLen - 1]	= 0;
			bShow	= false;
		}
		sprintf(achBuf, "%s%s", g_achRegName, bCursor == true ? "_" : "");
		g_Font2.DrawString(180, 180, achBuf);
		g_Font2.DrawString(180, 210, g_achRegKey);
		SetRect(&rc, 0, 0, 488, 29);
		Blit(80 - 4, 180 - 4, g_pOptions, &rc);
		g_Font.DrawString(0, 260, "Enter registration name, please.\nNote, that registration name in uppercase.", CMyString::FONT_ALIGN_CENTER);
	}
	else {
		DrawHighlight(1);
		int	nLen	= strlen(g_achRegKey);
		if(nLen < sizeof(g_achRegKey) - 1) {
			g_achRegKey[nLen]			= toupper(byChar);
			g_achRegKey[nLen + 1]	= 0;
		}
		if(IsKeyPressed(SDLK_BACKSPACE) && IsKeyStateChanged(SDLK_BACKSPACE) && nLen > 0) {
			g_achRegKey[nLen - 1]	= 0;
			bShow	= false;
		}
		g_Font2.DrawString(180, 180, g_achRegName);
		sprintf(achBuf, "%s%s", g_achRegKey, bCursor == true ? "_" : "");
		g_Font2.DrawString(180, 210, achBuf);
		SetRect(&rc, 0, 0, 488, 29);
		Blit(80 - 4, 210 - 4, g_pOptions, &rc);
		g_Font.DrawString(0, 260, "Enter registration code.\nExample: XXXX-1234-5678-YYYY.", CMyString::FONT_ALIGN_CENTER);
	}
	if(IsKeyPressed(SDLK_TAB) && IsKeyStateChanged(SDLK_TAB)) {
		bName	= !bName;
		bShow	= false;
	}
	if(IsKeyPressed(SDLK_UP) && IsKeyStateChanged(SDLK_UP)) {
		bName	= true;
		bShow	= false;
	}
	if(IsKeyPressed(SDLK_DOWN) && IsKeyStateChanged(SDLK_DOWN)) {
		bName	= false;
		bShow	= false;
	}
	if(IsKeyPressed(SDLK_RETURN) && IsKeyStateChanged(SDLK_RETURN)) {
		if(CheckRegistration() == true) {
			m_nMenuType	= MT_MAIN;
		}
		else	bShow		= true;
	}
	if(bShow == true) {
		if(byChar)	bShow	= false;
		g_Font.DrawString(0, 240, "Invalid registartion key", CMyString::FONT_ALIGN_CENTER);
	}

	g_Font.DrawString(0, SCREEN_HEIGHT - 20, "<Tab> move to next field, <Enter> confirm, <Esc> return to main menu.", CMyString::FONT_ALIGN_CENTER);

	g_Font2.DrawString(82, 180, "NAME:");
	g_Font2.DrawString(82, 210, "KEY:");

	if(true == DrawMenuButton(MENU_ITEM_X, 360, B_OK) && g_bMouseLB == true) {
		g_bMouseLB	= false;
		if(CheckRegistration() == true) {
			m_nMenuType	= MT_MAIN;
		}
		else	bShow	= true;
	}
	if(true == DrawMenuButton(MENU_ITEM_X + 124, 360, B_CANCEL) && g_bMouseLB == true) {
		g_bMouseLB	= false;
		bShow		= false;
		m_nMenuType	= MT_MAIN;
	}
	if(true == DrawMenuButton(MENU_ITEM_X, 390, B_BUYGAME) && g_bMouseLB == true) {
		g_bMouseLB	= false;
		bShow		= false;
		//m_nMenuType	= MT_MAIN;
#if defined(SOFTONIC)
#	ifdef _WIN32
		ShellExecute(NULL, "open", "https://comercio.softonic.com/pv/5000", NULL, NULL, SW_SHOWNORMAL);
#	elif __MACOSX__
		system("open https://comercio.softonic.com/pv/4865/0/1");
#	endif
#else
#	ifdef _WIN32
		ShellExecute(NULL, "open", "arkanoidsbreg.url", NULL, NULL, SW_SHOWNORMAL);
//#	elif __linux__
//		system("konqueror http://www.wegroup.org/games/arkanoid-games/arkanoid-space-ball.html");
#	elif __MACOSX__
		system("open http://www.wegroup.org/games/arkanoid-games/arkanoid-space-ball.html");
#	endif
#endif
	}
}


/*!
    \fn CMainMenu::DrawHighlight(int nPos)
 */
void CMainMenu::DrawHighlight(int nPos) {
	SDL_Rect	rc;
	SetRect(&rc, 0, 0, 160, 22);
	Blit(80, 180 + nPos * 30 - 1, g_pTransp, &rc);
	Blit(80 + 160, 180 + nPos * 30 - 1, g_pTransp, &rc);
	Blit(80 + 320, 180 + nPos * 30 - 1, g_pTransp, &rc);

	Blit(80 - 4, 180 - 4 + nPos * 30, g_pOptions, 0);
}
#endif

/*!
    \fn CMainMenu::DrawMenuButton(int nX, int nY, int nButtonId)
 */
bool CMainMenu::DrawMenuButton(int nX, int nY, int nButtonId) {
	struct _BUTTON {
		int	x, y, w;
	};
	const _BUTTON	aButtons[]	= {
		{ 0,   0, 248 },					// start new game
		{ 0,  29, 248 },					// highscores
		{ 0,  58, 248 },					// options
		{ 0,  87, 248 },					// help
		{ 0, 116, 248 },					// exit
		{ 0, 145, 248 },					// register
		{ 0, 174, 124 }, { 124, 174, 124 },	// OK, CANCEL
		{ 0, 203,  77 }, { 124, 203,  77 },	// <, >
		{ 0, 232, 248 },					// start new game
		{ 0, 261, 248 },					// restore game
		{ 0, 290, 248 },					// buy game
		{ 0, 319, 248 },					// level editor
		{ 0, 348, 248 },					// custom levels
	};
	SDL_Rect	rc;
	bool	bIsOver	= false;
	if(g_nCursorX > nX && g_nCursorX < nX + aButtons[nButtonId].w && g_nCursorY > nY && g_nCursorY < nY + 24) {
		bIsOver	= true;
		PlaySFX(nButtonId);
	}

	SetRect(&rc, aButtons[nButtonId].x + (bIsOver == false ? 0 : 248), aButtons[nButtonId].y, aButtons[nButtonId].w, 29);
	Blit(nX, nY, m_pMainMenuIcons, &rc);

	return bIsOver;
}


/*!
    \fn CArkanoidSBGame::SendEsc()
 */
void CMainMenu::SendEsc() {
	g_bMouseRB		= false;
	m_nMenuType		= MT_MAIN;
	m_bGetNameMode	= false;
	if(m_bReturnToGame == true) {
		g_nGameMode	= APPS_GAME;
	}
}
