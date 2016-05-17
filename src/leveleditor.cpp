/***************************************************************************
 *   Copyright (C) 2006 by Andrey A. Ugolnik                               *
 *   andrey@wegroup.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "arkanoidsb.h"
#include "leveleditor.h"

CLevelEditor::CLevelEditor() {
	m_bIsSelectBrickMode	= false;
	m_nBrickType	= BOX_0;
	m_pbyLevels		= 0;
	m_nCurrentLevel	= 0;
}


CLevelEditor::~CLevelEditor() {
	Save();
	delete[]	m_pbyLevels;
}


/*!
    \fn CLevelEditor::Draw()
 */
void CLevelEditor::Draw() {
	SDL_Rect	rc;
	int	nX, nY;

	EnableCursor(true);
	g_Arkanoid.DrawBackground();
	for(int x = 0; x < BRICKS_WIDTH; x++) {
		for(int y = 0; y < BRICKS_HEIGHT; y++) {
			g_MainMenu.DrawBrick(BRICK_X + x * BRICK_W, BRICK_Y + y * BRICK_H, m_abyLevel[x][y]);
		}
	}
	g_Font2.SetRect(97, 0, 56, SCREEN_HEIGHT);
	g_Font2.DrawNumber(m_nCurrentLevel + 1, 99, 19, CMyString::FONT_ALIGN_CENTER);
	g_Font2.SetRect(338, 0, 56, SCREEN_HEIGHT);
	g_Font2.DrawNumber(m_nLevelsCount, 342, 19, CMyString::FONT_ALIGN_CENTER);
	g_Font2.SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	if(IsKeyPressed(SDLK_TAB) && IsKeyStateChanged(SDLK_TAB)) {
		m_bIsSelectBrickMode	= !m_bIsSelectBrickMode;
	}
	if(m_bIsSelectBrickMode == true) {
		FadeScreen();
#define S_BRICK_X	(SCREEN_WIDTH - 10 * BRICK_W) / 2
#define S_BRICK_Y	(SCREEN_HEIGHT - 9 * BRICK_H) / 2
		int	x	= S_BRICK_X;
		int	y	= S_BRICK_Y;
		for(int i = BOX_0; i < BOX_END; i++) {
			g_MainMenu.DrawBrick(x, y, i);
			x	+= BRICK_W;
			if(x >= (SCREEN_WIDTH - 10 * BRICK_W) / 2 + 10 * BRICK_W) {
				x	= (SCREEN_WIDTH - 10 * BRICK_W) / 2;
				y	+= BRICK_H;
			}
		}
		nX	= (g_nCursorX - S_BRICK_X) / BRICK_W;
		nY	= (g_nCursorY - S_BRICK_Y) / BRICK_H;
		if(nX >= 0 && nX < 10 && nY >= 0 && nY < 9) {
			SetRect(&rc, 0, 0, BRICK_W, BRICK_H);
			Blit(S_BRICK_X + nX * BRICK_W, S_BRICK_Y + nY * BRICK_H, g_pTransp, &rc);
			if(g_bMouseLB == true) {
				g_bMouseLB	= false;
				m_bIsSelectBrickMode	= false;
				m_nBrickType	= 1 + nX + nY * 10;
			}
		}
	}
	else {
		nX	= (g_nCursorX - BRICK_X) / BRICK_W;
		nY	= (g_nCursorY - BRICK_Y) / BRICK_H;
		SetRect(&rc, 0, 0, BRICK_W, BRICK_H);
		if(nX >= 0 && nX < BRICKS_WIDTH && nY >= 0 && nY < BRICKS_HEIGHT) {
			Blit(BRICK_X + nX * BRICK_W, BRICK_Y + nY * BRICK_H, g_pTransp, &rc);
			if(g_bMouseLB == true) {
				m_abyLevel[nX][nY]	= m_nBrickType;
			}
			if(g_bMouseRB == true) {
				m_abyLevel[nX][nY]	= BOX_NONE;
			}
		}
		rc.w	= 160;
		for(int i = 0; i < BRICKS_HEIGHT; i ++) {
			Blit(BRICK_X + BRICKS_WIDTH * BRICK_W + 5, BRICK_Y + i * BRICK_H, g_pTransp, &rc);
			Blit(BRICK_X + BRICKS_WIDTH * BRICK_W + 5 + 160, BRICK_Y + i * BRICK_H, g_pTransp, &rc);
		}
		g_Font.DrawString(BRICK_X + BRICKS_WIDTH * BRICK_W + 15, BRICK_Y + 15,
			"<Arrows> - roll level.\n"\
			"<Page Up> / <Page Down> - change level.\n"\
			"<Tab> - edit mode / selection mode.\n"\
			"<Delete> - clear level.\n"\
			"<Ctrl> + <Delete> - delete current level.\n"\
			"<Ctrl> + <Insert> - insert empty level.\n"\
			"<Esc> - save and exit to game menu.\n"\
			"NOTE: No undo availabe.");

		if(IsKeyPressed(SDLK_DELETE) && IsKeyStateChanged(SDLK_DELETE)) {
			memset(m_abyLevel, 0, sizeof(m_abyLevel));
		}
		if(((true == g_MainMenu.DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 30, WALL_Y2 - 30, CMainMenu::B_PREV) && g_bMouseLB == true) || (IsKeyPressed(SDLK_PAGEUP) && IsKeyStateChanged(SDLK_PAGEUP))) && m_nCurrentLevel > 0) {
			g_bMouseLB	= false;
			memcpy(m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_abyLevel, sizeof(m_abyLevel));
			m_nCurrentLevel--;
			memcpy(m_abyLevel, m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_abyLevel));
		}
		if((true == g_MainMenu.DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 100, WALL_Y2 - 30, CMainMenu::B_NEXT) && g_bMouseLB == true) || (IsKeyPressed(SDLK_PAGEDOWN) && IsKeyStateChanged(SDLK_PAGEDOWN))) {
			g_bMouseLB	= false;
			memcpy(m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_abyLevel, sizeof(m_abyLevel));
			if(m_nCurrentLevel == m_nLevelsCount - 1) {
				Uint8	*pTmp	= new Uint8[(m_nLevelsCount + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT)];
				memcpy(pTmp, m_pbyLevels, m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT));
				delete[]	m_pbyLevels;
				m_pbyLevels	= pTmp;
				m_nLevelsCount++;
				memset(m_pbyLevels + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), 0, sizeof(m_abyLevel));
			}
			m_nCurrentLevel++;
			memcpy(m_abyLevel, m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_abyLevel));
		}
		if(IsKeyPressed(SDLK_UP) && IsKeyStateChanged(SDLK_UP)) {
			for(int x = 0; x < BRICKS_WIDTH; x++) {
				Uint8	byTemp	= m_abyLevel[x][0];
				for(int y = 0; y < BRICKS_HEIGHT - 1; y++) {
					m_abyLevel[x][y]	= m_abyLevel[x][y + 1];
				}
				m_abyLevel[x][BRICKS_HEIGHT - 1]	= byTemp;
			}
		}
		if(IsKeyPressed(SDLK_DOWN) && IsKeyStateChanged(SDLK_DOWN)) {
			for(int x = 0; x < BRICKS_WIDTH; x++) {
				Uint8	byTemp	= m_abyLevel[x][BRICKS_HEIGHT - 1];
				for(int y = BRICKS_HEIGHT - 1; y > 0; y--) {
					m_abyLevel[x][y]	= m_abyLevel[x][y - 1];
				}
				m_abyLevel[x][0]	= byTemp;
			}
		}
		if(IsKeyPressed(SDLK_LEFT) && IsKeyStateChanged(SDLK_LEFT)) {
			for(int y = 0; y < BRICKS_HEIGHT; y++) {
				Uint8	byTemp	= m_abyLevel[0][y];
				for(int x = 1; x < BRICKS_WIDTH; x++) {
					m_abyLevel[x - 1][y]	= m_abyLevel[x][y];
				}
				m_abyLevel[BRICKS_WIDTH - 1][y]	= byTemp;
			}
		}
		if(IsKeyPressed(SDLK_RIGHT) && IsKeyStateChanged(SDLK_RIGHT)) {
			for(int y = 0; y < BRICKS_HEIGHT; y++) {
				Uint8	byTemp	= m_abyLevel[BRICKS_WIDTH - 1][y];
				for(int x = BRICKS_WIDTH - 1; x > 0; x--) {
					m_abyLevel[x][y]	= m_abyLevel[x - 1][y];
				}
				m_abyLevel[0][y]	= byTemp;
			}
		}
		if(g_modState & KMOD_CTRL && IsKeyPressed(SDLK_DELETE) && IsKeyStateChanged(SDLK_DELETE)) {
			if(m_nLevelsCount > 1) {
				m_nLevelsCount--;
				Uint8	*pTmp	= new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
				memcpy(pTmp, m_pbyLevels, m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT));
				memcpy(pTmp + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_pbyLevels + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), (m_nLevelsCount - m_nCurrentLevel) * (BRICKS_WIDTH * BRICKS_HEIGHT));
				delete[]	m_pbyLevels;
				m_pbyLevels	= pTmp;
				m_nCurrentLevel	= min(m_nCurrentLevel, m_nLevelsCount - 1);
				memcpy(m_abyLevel, m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_abyLevel));
			}
		}
		if(g_modState & KMOD_CTRL && IsKeyPressed(SDLK_INSERT) && IsKeyStateChanged(SDLK_INSERT)) {
			m_nLevelsCount++;
			Uint8	*pTmp	= new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
			memcpy(pTmp, m_pbyLevels, m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT));
			memcpy(pTmp + (m_nCurrentLevel + 1) * (BRICKS_WIDTH * BRICKS_HEIGHT), m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), (m_nLevelsCount - m_nCurrentLevel - 1) * (BRICKS_WIDTH * BRICKS_HEIGHT));
			delete[]	m_pbyLevels;
			m_pbyLevels	= pTmp;
			memset(m_abyLevel, 0, sizeof(m_abyLevel));
		}
	}

	SetRect(&rc, 0, 0, 30, 40);
	Blit(5, 5, g_pTransp, &rc);
	g_MainMenu.DrawBrick(10, 10, m_nBrickType);
}


/*!
    \fn CLevelEditor::Load()
 */
void CLevelEditor::Load() {
	delete[]	m_pbyLevels;
	char	achBuf[PATH_MAX];
	FILE	*pFile;
	sprintf(achBuf, "%scustomlevels", g_achUserProfile);
	if((pFile = fopen(achBuf, "rb"))) {
		fseek(pFile, 0, SEEK_END);
		m_nLevelsCount	= ftell(pFile) / (BRICKS_WIDTH * BRICKS_HEIGHT);
		if(m_nLevelsCount > 0) {
			rewind(pFile);
			m_pbyLevels	= new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
			fread(m_pbyLevels, (BRICKS_WIDTH * BRICKS_HEIGHT), m_nLevelsCount, pFile);
		}
		else {
			m_nLevelsCount	= 1;
			m_pbyLevels	= new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
			memset(m_abyLevel, 0, sizeof(m_abyLevel));
		}
		fclose(pFile);
	}
	else {
		m_nLevelsCount	= 1;
		m_pbyLevels	= new Uint8[m_nLevelsCount * (BRICKS_WIDTH * BRICKS_HEIGHT)];
		memset(m_pbyLevels, 0, sizeof(m_abyLevel));
	}
	m_nCurrentLevel	%= m_nLevelsCount;
	memcpy(m_abyLevel, m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), sizeof(m_abyLevel));
	printf("Custom levels: %d loaded.\n", m_nLevelsCount);
}

/*!
    \fn CLevelEditor::Save()
 */
void CLevelEditor::Save() {
	char	achBuf[PATH_MAX];
	FILE	*pFile;
	sprintf(achBuf, "%scustomlevels", g_achUserProfile);
	memcpy(m_pbyLevels + m_nCurrentLevel * (BRICKS_WIDTH * BRICKS_HEIGHT), m_abyLevel, sizeof(m_abyLevel));
	if((pFile = fopen(achBuf, "wb"))) {
		fwrite(m_pbyLevels, (BRICKS_WIDTH * BRICKS_HEIGHT), m_nLevelsCount, pFile);
		fclose(pFile);
	}
	printf("Custom levels: %d saved.\n", m_nLevelsCount);
}
