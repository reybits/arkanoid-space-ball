// arkanoidsb.cpp : Defines the entry point for the application.
//

#include "arkanoidsb.h"
#include <sys/stat.h>


void SetVideoMode();
void UnsetVideoMode();
void CommonQuit();
bool UpdateKeys();
void ReadWriteConfig(bool bMode);
void PlayMusic2();
void SwitchFullscreen();
bool DrawIntro();

char	g_achUserProfile[PATH_MAX]	= { 0 };

SDL_Surface	*g_psurfScreen;
bool g_bFullscreen	= true;
bool g_bOGL				= false;
int g_anBpx[5]	= { 32, 24, 16, 15, 8 };
int g_nBppIndex	= 0;
bool g_bShowFps	= false;
bool g_bTutorialMode	= true;
bool g_bAutoBonusMode	= true;

double	g_fSpeedCorrection;
double	g_fCos[360];
double	g_fSin[360];

SDL_Joystick	*g_pJoystick;
bool g_bIsJoystickSupported	= false;

bool g_bActive	= true;

int g_nNumKeys		= 0;
Uint8	*g_pnKeys	= 0;
bool *g_pbIsKeyStateChanged	= 0;
Uint32 g_dwModState	= 0;

bool g_bMouseRB			= false;
bool g_bMouseLB			= false;
bool g_bIsCursorVisible	= false;
double g_nMouseDX	= 0;
double g_nMouseDY	= 0;
int g_nCursorX	= SCREEN_WIDTH / 2;
int g_nCursorY	= SCREEN_HEIGHT / 2;

int g_nGameMode	= APPS_INTRO;
int m_nLastHiScorePos	= -1;
int m_nLastGameScore		= 0;
int m_nLastGameLevel		= 0;
_HIGHSCORE	g_strHighScore[10];

SDL_Surface* m_pEnergyHole;
SDL_Surface* m_pMonstPatrol;
SDL_Surface* m_pMonstCopter;
SDL_Surface* m_pMonstEye;
SDL_Surface* m_pMonstBlackHole;
SDL_Surface* m_pMonstFighter;
SDL_Surface* m_pMonstHand;
SDL_Surface* m_pMonstStone1;
SDL_Surface* m_pMonstStone2;
SDL_Surface* m_pMonstTurbine;
SDL_Surface* m_pMonstUfo;
SDL_Surface* m_pMonstWreckage1;
SDL_Surface* m_pMonstWreckage2;
SDL_Surface* m_pBall;
SDL_Surface* m_pVector;
SDL_Surface* m_pFB;
SDL_Surface* m_pBullet;
SDL_Surface* m_pExploision;
SDL_Surface* m_pBricks;
SDL_Surface* m_pBricksMovBullets;
SDL_Surface* m_pBonuses;
SDL_Surface* g_pBonusesSmall;
SDL_Surface* g_pBonusesAura;
SDL_Surface* g_pGameBG	= 0;
SDL_Surface* m_pGameBGanims;
SDL_Surface* m_pGameWall;
SDL_Surface* m_pRacket;
SDL_Surface* m_pUnregistered;
SDL_Surface* m_pBackground;
SDL_Surface* m_pBackground2;
SDL_Surface* m_pMainMenuIcons;
SDL_Surface* m_pCursor;
SDL_Surface* g_pTransp;
SDL_Surface* g_pTutorialDlg;
SDL_Surface* g_pSinusString;
SDL_Surface* g_pOptions;
//SDL_Surface* g_pBGStars;

const char *g_pachSnd[]	= {
	"snd_electric.wav",	//  0
	"snd_applause.wav",	//  1
	"snd_patrol.wav",	//  2
	"snd_turbine.wav",	//  3
	"snd_click.wav",		//  4
	"snd_over.wav",		//  5
	"snd_impact1.wav",	//  6
	"snd_impact2.wav",	//  7
	"snd_laser.wav",		//  8
	"snd_bonusup.wav",	//  9
	"snd_bonusnew.wav",	// 10
	"snd_paddle.wav",		// 11
	"snd_missile.wav",	// 12
	"snd_plasma.wav",		// 13
	"snd_lostball.wav",	// 14
	"snd_explode.wav",	// 15
};
Mix_Chunk	*g_apSnd[sizeof(g_pachSnd) / sizeof(const char*)];
Mix_Music	*g_apMod[3];
bool g_bIsAudioSupported	= true;
int g_nVolumeM	= 3;
int g_nVolumeS	= 4;

CResource		g_Resource;

CMyString		g_Font;
CMyString		g_Font2;
CMyString		g_Font3;
CRandom			g_Rnd;
CMonster		g_Monster;
CEnergyHole		g_EnergyHole;
CBonus			g_Bonus;
CExploision		g_Exploision;
CBullet			g_Bullet;
CBall			g_Ball;
CMainMenu		g_MainMenu;
CArkanoidSBGame	g_Arkanoid;
CCoolString		g_CoolString;
CSinusString	g_CSinusString;
CTutorialDlg	g_TutorialDlg;
CMyString		g_FontTutorial;


#if !defined(__linux__) && !defined(FULL_VERSION)
bool CheckForBlackList(const char *pchKey);
bool CheckRegistration();
bool g_bIsRegistered	= false;
char g_achRegName[100 + 1]	= { 0 };
char g_achRegKey[16 + 3 + 1]	= { 0 };	// 16 symbols + 3 dashes + 1 null
int g_nUnregisterdCount	= 30 * 60;

CReminderDlg	g_ReminderDlg;
#endif


int main(int argc, char *argv[]) {
	int	i;
	char	achTemp[PATH_MAX];

#ifdef __linux__
	sprintf(g_achUserProfile, "%s/.arkanoidsb/", getenv("HOME") ? getenv("HOME") : ".");
#elif _WIN32
	int		nLen	= sizeof(achTemp);
	//GetUserProfileDirectory(0, achTemp, nLen);
	ExpandEnvironmentStrings("%USERPROFILE%", achTemp, nLen);
	sprintf(g_achUserProfile, "%s\\.arkanoidsb\\", achTemp);
#else
	sprintf(g_achUserProfile, "%s/Library/Application Support/arkanoidsb/", getenv("HOME") ? getenv("HOME") : ".");
#endif
	printf("Arkanoid: Space Ball by 'WE' Group. Copyright (c) 2006-2007.\n");
	printf("version %s.\n", GAME_VERSION);
	printf("Users config dir: %s\n", g_achUserProfile);
	ReadWriteConfig(true);

	// parse command line
	for(i = 1; i < argc; i++) {
		if(strncmp(argv[i], "-f", 2)) {
			g_bFullscreen	= true;
		}
		else if(strncmp(argv[i], "-w", 2)) {
			g_bFullscreen	= false;
		}
		else if(strncmp(argv[i], "-s", 2)) {
			g_bOGL	= false;
		}
		else if(strncmp(argv[i], "-g", 2)) {
			g_bOGL	= true;
		}
		else if(strncmp(argv[i], "-h", 2)) {
			printf("%s [-f | -w | -s | -g | -h]\n", argv[0]);
			printf("  -f - fullscreen\n");
			printf("  -w - windowed mode\n");
			printf("  -s - software mode\n");
			printf("  -g - use opengl\n");
			printf("  -h - this screen\n");
			exit(0);
		}
	}

	char	achPath[PATH_MAX], *pchEnd;
	strcpy(achPath, argv[0]);
#ifdef __MACOSX__
	if(false == g_Resource.Open("arkanoidsb.app/Contents/Resources/arkanoidsb.pak")) {
#else
	if(false == g_Resource.Open("res/arkanoidsb.pak")) {
#endif
		if((pchEnd = strrchr(achPath, '\\')) == 0 && (pchEnd = strrchr(achPath, '/')) == 0) {
			exit(-1);
		}
		*(pchEnd + 1)	= 0;
#ifdef __MACOSX__
		strcat(achPath, "../Resources/arkanoidsb.pak");
#else
		strcat(achPath, "res/arkanoidsb.pak");
#endif
		if(false == g_Resource.Open(achPath)) {
			exit(-1);
		}
	}

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Couldn't initialize SDL-video: %s\n", SDL_GetError());
		exit(-2);
	}
	atexit(CommonQuit);

	EnableCursor(false);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		printf("Couldn't initialize SDL-audio: %s\n", SDL_GetError());
		printf("  continuing without audio support.\n");
		g_bIsAudioSupported	= false;
	}
	else {
		// open 44.1KHz, signed 16bit, system byte order,
		//      stereo audio, using 1024 byte chunks
		if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
			printf("Couldn't OpenAudio: %s\n", Mix_GetError());
			printf("  continuing without audio support.\n");
			g_bIsAudioSupported	= false;
		}
	}
	if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
		printf("Couldn't initialize joystick: %s\n", SDL_GetError());
		printf("  continuing without joystick support.\n");
		//g_bIsJoystickSupported	= false;
	}
	else {
		int	nJosticks	= SDL_NumJoysticks();
		printf("%i joysticks were found.\n", nJosticks);
		if(nJosticks > 0) {
			printf("The names of the joysticks are:\n");
			for(int i = 0; i < nJosticks; i++) {
				printf("  %s\n", SDL_JoystickName(i));
				g_pJoystick	= SDL_JoystickOpen(i);
				printf("Axes: %d, Buttons: %d, Balls: %d, Hats %d\n", SDL_JoystickNumAxes(g_pJoystick), SDL_JoystickNumButtons(g_pJoystick), SDL_JoystickNumBalls(g_pJoystick), SDL_JoystickNumHats(g_pJoystick));
				SDL_JoystickClose(g_pJoystick);
			}
			SDL_JoystickEventState(SDL_QUERY);
			g_pJoystick	= SDL_JoystickOpen(0);
			g_bIsJoystickSupported	= true;
		}
	}

	unsigned int	nDataLen;
	unsigned char	*pbyData;
#ifndef __MACOSX__
	pbyData	= g_Resource.GetDataAllocMem("icon.bmp", nDataLen);
	SDL_Surface	*pIcon	= SDL_LoadBMP_RW(SDL_RWFromMem(pbyData, nDataLen), 1);
	g_Resource.FreeMem(pbyData);
	if(0 != pIcon) {
		printf(" done.\n");
	}
	else {
		printf("  %s\n", SDL_GetError());
	}

	SDL_SetColorKey(pIcon, SDL_SRCCOLORKEY, SDL_MapRGB(pIcon->format, 0, 0, 0));
	SDL_WM_SetIcon(pIcon, NULL);
#endif

	SetVideoMode();
	SDL_WarpMouse(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	if(g_bIsAudioSupported == true) {
		// loading sound effects
		for(i = 0; i < sizeof(g_pachSnd) / sizeof(const char*); i++) {
			// try to load image from resource from
			pbyData	= g_Resource.GetDataAllocMem(g_pachSnd[i], nDataLen);
			if(pbyData) {
				g_apSnd[i]	= Mix_LoadWAV_RW(SDL_RWFromMem(pbyData, nDataLen), 1);
				g_Resource.FreeMem(pbyData);
				if(0 != g_apSnd[i]) {
					printf(" done.\n");
				}
				else {
					printf("  %s\n", SDL_GetError());
				}
			}
		}
		//loading modules
		for(i = 0; i < sizeof(g_apMod) / sizeof(Mix_Music*); i++) {
#ifdef __MACOSX__
 			if(i != 2)	sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module%.2d.ogg", i + 1);
 			else		sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module03.s3m");
#else
 			if(i != 2)	sprintf(achTemp, "res/module%.2d.ogg", i + 1);
 			else		sprintf(achTemp, "res/module03.s3m");
#endif
			printf("Loading module %s", achTemp);
			g_apMod[i]	= Mix_LoadMUS(achTemp);
			if(0 != g_apMod[i])	printf(" done.\n");
			else {
				if((pchEnd = strrchr(achPath, '\\')) == 0 && (pchEnd = strrchr(achPath, '/')) == 0) {
					achPath[0]	= 0;
				}
				*(pchEnd + 1)	= 0;
#ifdef __MACOSX__
				if(i != 2) { sprintf(achTemp, "../Resources/module%.2d.ogg", i + 1); strcat(achPath, achTemp); }
				else		strcat(achPath, "../Resources/module03.s3m");
#else
				if(i != 2) { sprintf(achTemp, "res/module%.2d.ogg", i + 1); strcat(achPath, achTemp); }
				else		strcat(achPath, "res/module03.s3m");
#endif
				g_apMod[i]	= Mix_LoadMUS(achPath);
				if(0 != g_apMod[i])	printf(" done.\n");
				else	printf("  %s\n", SDL_GetError());
			}
		}
		SetVolumeSound(g_nVolumeS);
		PlayMusic2();
	}

	const double	pi	= 3.1415926535;
	for(i = 0; i < 360; i++) {
		g_fCos[i]	= (double)cos((pi / 180) * i);
		g_fSin[i]	= (double)sin((pi / 180) * i);
	}

	g_pnKeys						= SDL_GetKeyState(&g_nNumKeys);
	g_pnKeys						= new Uint8[g_nNumKeys];
	g_pbIsKeyStateChanged	= new bool[g_nNumKeys];
	memset(g_pnKeys, 0, g_nNumKeys * sizeof(Uint8));
	memset(g_pbIsKeyStateChanged, 0, g_nNumKeys * sizeof(bool));

	int		nFrame		= 0;
	Uint32	nFPStime	= 0;
	double	fFPS		= 0;
	Uint32	nPrevTime	= 0;
	g_fSpeedCorrection	= 1.0;
	const double    fPerFrameMs     = 1000.0 / DESIRED_FPS;
	
	CLevelEditor	m_LevelEditor;
	//g_nGameMode	= APPS_EDITOR;

	while(UpdateKeys() == false && g_nGameMode != APPS_EXIT) {
		Uint32	nTimeCurrent	= SDL_GetTicks();
		if(g_bActive == true) {
			nFrame++;
			Uint32	nFPSelapsed		= nTimeCurrent - nFPStime;
			if(nFPSelapsed >= 1000) {
				fFPS		= (nFrame * 1000.0) / nFPSelapsed;
				nFrame		= 0;
				nFPStime	= nTimeCurrent;
			}

			g_fSpeedCorrection	= double(nTimeCurrent - nPrevTime) / (DESIRED_FPS / 10.0);
			nPrevTime				= nTimeCurrent;

			// volume manager
			if(g_dwModState & KMOD_SHIFT) {
				if(IsKeyPressed(SDLK_MINUS) && IsKeyStateChanged(SDLK_MINUS)) {
					SetVolumeMusic(--g_nVolumeM);
				}
				if(IsKeyPressed(SDLK_EQUALS) && IsKeyStateChanged(SDLK_EQUALS)) {
					SetVolumeMusic(++g_nVolumeM);
				}
			}
			else {
				if(IsKeyPressed(SDLK_MINUS) && IsKeyStateChanged(SDLK_MINUS)) {
					SetVolumeSound(--g_nVolumeS);
				}
				if(IsKeyPressed(SDLK_EQUALS) && IsKeyStateChanged(SDLK_EQUALS)) {
					SetVolumeSound(++g_nVolumeS);
				}
			}
			if(g_nGameMode == APPS_MAINMENU) {
				switch(g_MainMenu.DrawMenu()) {
				case 0:
					g_nGameMode	= APPS_EXIT;
					break;
				case 1:
					g_TutorialDlg.Reset();
					g_Arkanoid.InitNewGame(false);
					g_nGameMode	= APPS_GAME;
					PlayMusic(true);
					break;
				case 2:
					g_TutorialDlg.Reset();
					g_Arkanoid.RestoreGame();
					g_nGameMode	= APPS_GAME;
					PlayMusic(true);
					break;
				case 3:
					// TODO check for valid custom levels
					
					g_TutorialDlg.Reset();
					g_Arkanoid.InitNewGame(true);
					g_nGameMode	= APPS_GAME;
					PlayMusic(true);
					break;
				case 4:	// enter to level editor mode
					m_LevelEditor.Load();
					g_nGameMode	= APPS_EDITOR;
					break;
				}
			}
			else if(g_nGameMode == APPS_GAME) {
				if(g_Arkanoid.DrawScreen() == true) {
					PlayMusic2();
					g_nGameMode	= APPS_SHOULDGETNAME;
				}
			}
			else if(g_nGameMode == APPS_SHOULDGETNAME) {
				g_Arkanoid.GetGameData(m_nLastGameScore, m_nLastGameLevel);
				if(m_nLastGameScore > g_strHighScore[9].nScore) {
					g_MainMenu.SetEnterNameMode();
				}
				else {
					g_MainMenu.SetMenuType(CMainMenu::MT_MAIN);
				}
				g_nGameMode	= APPS_MAINMENU;
			}
			else if(g_nGameMode == APPS_INTRO) {
				if(true == DrawIntro())	g_nGameMode	= APPS_MAINMENU;
			}
			else if(g_nGameMode == APPS_EDITOR) {
				m_LevelEditor.Draw();
				if((true == g_MainMenu.DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 200, WALL_Y2 - 30, CMainMenu::B_OK) && g_bMouseLB == true) || IsKeyPressed(SDLK_ESCAPE)) {
					g_bMouseLB	= false;
					m_LevelEditor.Save();
					g_nGameMode	= APPS_MAINMENU;
				}
			}

			// save screenshot to profile dir
			if((g_dwModState & KMOD_CTRL) && IsKeyPressed(SDLK_s) && IsKeyStateChanged(SDLK_s)) {
/*				char    achPath[PATH_MAX];
				time_t  osTime	= time(0);
				struct tm	local	= *localtime(&osTime);
				sprintf(achPath, "%sarkanoidsb-%4d%2d%2d-%2d%2d.bmp", g_achUserProfile, local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min);
				SDL_SaveBMP(g_psurfScreen, achPath);
*/			}
			if(IsKeyPressed(SDLK_f) && IsKeyStateChanged(SDLK_f)) {
				g_bShowFps	= !g_bShowFps;
				//printf("show fps %s\n", g_bShowFps == true ? "on" : "off");
			}

			if(g_bIsCursorVisible == true) {
				Blit(g_nCursorX - 8, g_nCursorY, m_pCursor, 0);
			}
			if(g_bShowFps == true) {
				g_Font.DrawNumber((int)fFPS, 5, 5, 1);
			}

			SDL_Flip(g_psurfScreen);
			
			Sint32  nDelay  = (Sint32)(fPerFrameMs - (SDL_GetTicks() - nTimeCurrent));
			if(nDelay > 0)  SDL_Delay(nDelay);
			else    SDL_Delay(0);
		}
		else {
			SDL_FillRect(g_psurfScreen, 0, 0);
			g_Font2.DrawString(0, SCREEN_HEIGHT / 2, "FOCUS LOST - GAME INACTIVE", 2);
			SDL_Flip(g_psurfScreen);
			SDL_Delay(200);
		}
	}

	delete[]	g_pnKeys;
	delete[]	g_pbIsKeyStateChanged;

	for(i = 0; i < sizeof(g_apMod) / sizeof(Mix_Music*); i++) {
		if(g_apMod[i])	Mix_FreeMusic(g_apMod[i]);
	}

	for(i = 0; i < sizeof(g_apSnd) / sizeof(Mix_Chunk*); i++) {
		if(g_apSnd[i])	Mix_FreeChunk(g_apSnd[i]);
	}

	ReadWriteConfig(false);
	UnsetVideoMode();
	SDL_JoystickClose(g_pJoystick);
	exit(0);
}

void CommonQuit() {
	Mix_CloseAudio();
	SDL_Quit();
}

SDL_Surface *LoadImage(const char *pchFileName, const Uint32 nColorKey) {
	SDL_Surface	*pOptimizedImage	= 0;	//The optimized image that will be used

	// try to load image from resource from
	unsigned int	nDataLen;
	unsigned char	*pbyData	= g_Resource.GetDataAllocMem(pchFileName, nDataLen);
	if(pbyData) {
		SDL_Surface	*pTmp	= IMG_Load_RW(SDL_RWFromMem(pbyData, nDataLen), 1);
		g_Resource.FreeMem(pbyData);
		//SDL_Surface	*pTmp	= IMG_Load(pchFileName);
		if(pTmp != 0) {
			if(nColorKey != 0xff000000) {
				SDL_SetColorKey(pTmp, SDL_SRCCOLORKEY | (g_bOGL == true ? 0 : SDL_RLEACCEL), SDL_MapRGB(g_psurfScreen->format, (nColorKey>>16)&0xff, (nColorKey>>8)&0xff, nColorKey&0xff) | SDL_ALPHA_TRANSPARENT);
				pOptimizedImage	= SDL_DisplayFormat(pTmp);	//Create an optimized image
			}
			else {
				pOptimizedImage	= SDL_DisplayFormatAlpha(pTmp);	//Create an optimized image
			}
			SDL_FreeSurface(pTmp);	//Free the old image
			printf(" done.\n");
		}
		else {
			printf("  --%s\n", SDL_GetError());
		}
	}

	return	pOptimizedImage;	//Return the optimized image
}

void Blit(const int nX, const int nY, SDL_Surface *pImg, SDL_Rect *pSrc) {
	SDL_Rect	dst;
	dst.x	= nX;
	dst.y	= nY;
	SDL_BlitSurface(pImg, pSrc, g_psurfScreen, &dst);
}

// void BlitStretch(int nX, int nY, SDL_Surface *pImg, SDL_Rect *pSrc, double fScale) {
// 	SDL_Rect	pdst;
// 	int	w	= (int)(fScale * pSrc->w);
// 	int	h	= (int)(fScale * pSrc->h);
// 	int	dx	= (pSrc->w - w) / 2;
// 	int	dy	= (pSrc->h - h) / 2;
// 	pSrc->x	+= dx;
// 	pSrc->y	+= dy;
// 	pSrc->w	= w;
// 	pSrc->h	= h;
//
// 	pdst.x	= nX + dx;
// 	pdst.y	= nY + dy;
//
// 	SDL_BlitSurface(pImg, pSrc, g_psurfScreen, &pdst);
// }

void SetRect(SDL_Rect *pRc, int nX, int nY, int nW, int nH) {
	pRc->x	= nX;
	pRc->y	= nY;
	pRc->w	= nW;
	pRc->h	= nH;
}

bool IsKeyPressed(int nKey) {
	if(g_pnKeys[nKey] == SDL_PRESSED)
		return true;
	return false;
}

bool IsKeyStateChanged(int nKey) {
	return g_pbIsKeyStateChanged[nKey];
}

void FadeScreen() {
	for(int x = 0; x < 4; x++) {
		for(int y = 0; y < 4; y++) {
			Blit(x * 160, y * 120, g_pTransp, 0);
		}
	}
}

bool UpdateKeys() {
	SDL_Event	evt;
	Uint8		*pnKeys;

	g_dwModState	= SDL_GetModState();
	pnKeys	= SDL_GetKeyState(NULL);
	for(int i = 0; i < g_nNumKeys; i++) {
		g_pnKeys[i] != pnKeys[i] ? g_pbIsKeyStateChanged[i] = true : g_pbIsKeyStateChanged[i] = false;
	}
	memcpy(g_pnKeys, pnKeys, g_nNumKeys * sizeof(Uint8));

	g_nMouseDX	= g_nMouseDY	= 0;
	while(SDL_PollEvent(&evt)) {
		switch(evt.type) {
		case SDL_ACTIVEEVENT:
			if(evt.active.state == SDL_APPINPUTFOCUS || evt.active.state == SDL_APPACTIVE) {
				if(evt.active.gain == 0) {
					if(g_nGameMode == APPS_GAME) {
						g_Arkanoid.SetPause();
					}
					Mix_PauseMusic();
					//printf("Arkanoid: Space Ball inactive\n");
					g_bActive	= false;
				}
			}
			else {
				if(evt.active.gain) {
					Mix_ResumeMusic();
					//printf("Arkanoid: Space Ball active\n");
					g_bActive	= true;
				}
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch(evt.button.button) {
			case 1:	g_bMouseLB	= true;	break;
			case 3:	g_bMouseRB	= true;	break;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch(evt.button.button) {
			case 1:	g_bMouseLB	= false;	break;
			case 3:	g_bMouseRB	= false;	break;
			case 4:	if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(false);	break;
			case 5:	if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(true);	break;
			}
			break;

		case SDL_MOUSEMOTION:
			g_nMouseDX	+= evt.motion.xrel;
			g_nMouseDY	+= evt.motion.yrel;
			g_nCursorX	= evt.motion.x;
			g_nCursorY	= evt.motion.y;
			break;

		case SDL_KEYDOWN:
			if(((g_dwModState & (SDLK_LCTRL | SDLK_RCTRL)) && evt.key.keysym.sym == SDLK_q) ||
				((g_dwModState & (SDLK_LALT | SDLK_RALT)) && (evt.key.keysym.sym == SDLK_x || evt.key.keysym.sym == SDLK_F4))) {	// if ALT + X pressed quit game
				if(g_nGameMode != APPS_INTRO)	return true;
			}
			else if((g_dwModState & (SDLK_LALT | SDLK_RALT)) && evt.key.keysym.sym == SDLK_RETURN) {
				g_bFullscreen	= !g_bFullscreen;
				SwitchFullscreen();
			}
			break;

		case SDL_JOYBUTTONDOWN:	// Handle Joystick Button Presses
			if(evt.jbutton.button == 0) {
				g_bMouseLB	= true;
			}
			if(evt.jbutton.button == 1) {
				g_bMouseRB	= true;
			}
			if(evt.jbutton.button == 2) {
				if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(false);
			}
			else if(evt.jbutton.button == 3) {
				if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(true);
			}
			if(evt.jbutton.button == 4) {
				if(g_nGameMode == APPS_GAME) g_Arkanoid.SendEsc();
				if(g_nGameMode == APPS_MAINMENU) g_MainMenu.SendEsc();
			}
			break;
		case SDL_JOYBUTTONUP:
			if(evt.jbutton.button == 0) {
				g_bMouseLB	= false;
			}
			if(evt.jbutton.button == 1) {
				g_bMouseRB	= false;
			}
			break;
/*		case SDL_JOYAXISMOTION:	// Handle Joystick Motion
			if((evt.jaxis.value < -3200) || (evt.jaxis.value > 3200)) {
				int	nDelta	= int(evt.jaxis.value / 32768.0 * g_fSpeedCorrection * 10);
				if(evt.jaxis.axis == 0) {
					g_nCursorX	+= nDelta;
					g_nMouseDX	+= nDelta;
				}
				if(evt.jaxis.axis == 1) {
					g_nCursorY	+= nDelta;
					g_nMouseDY	+= nDelta;
				}
			}
			break;*/

		case SDL_QUIT:
			return true;
			break;
		}
	}

	if(g_bIsJoystickSupported == true) {
		SDL_JoystickUpdate();
		/*if(SDL_JoystickGetButton(g_pJoystick, 0)) {
			g_bMouseLB	= true;
		}
		else {
			g_bMouseLB	= false;
		}
		if(SDL_JoystickGetButton(g_pJoystick, 1)) {
			g_bMouseRB	= true;
		}
		else {
			g_bMouseRB	= false;
		}
		if(SDL_JoystickGetButton(g_pJoystick, 2)) {
			if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(false);
		}
		else if(SDL_JoystickGetButton(g_pJoystick, 3)) {
			if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(true);
		}
		if(SDL_JoystickGetButton(g_pJoystick, 4)) {
			g_Arkanoid.SendEsc();
		}*/
		int	nDelta	= SDL_JoystickGetAxis(g_pJoystick, 0);
		if((nDelta < -3200) || (nDelta > 3200)) {
			nDelta	= int(nDelta / 32768.0 * g_fSpeedCorrection * 4);
			g_nCursorX	+= nDelta;
			g_nMouseDX	+= nDelta;
		}
		nDelta	= SDL_JoystickGetAxis(g_pJoystick, 1);
		if((nDelta < -3200) || (nDelta > 3200)) {
			nDelta	= int(nDelta / 32768.0 * g_fSpeedCorrection * 4);
			g_nCursorY	+= nDelta;
			g_nMouseDY	+= nDelta;
		}
	}

	g_nCursorX	= max(g_nCursorX, 0);
	g_nCursorX	= min(g_nCursorX, SCREEN_WIDTH);
	g_nCursorY	= max(g_nCursorY, 0);
	g_nCursorY	= min(g_nCursorY, SCREEN_HEIGHT);

	return	false;
}

void musicFinished() {
	PlayMusic(false);
}

void PlayMusic(bool bFromFirst) {
	if(g_bIsAudioSupported == false)	return;
	static int	nModule	= 0;
	if(bFromFirst == true)	nModule	= 0;
	Mix_HookMusicFinished(musicFinished);
	Mix_PlayMusic(g_apMod[nModule++], 0);
	nModule	%= (sizeof(g_apMod) / sizeof(Mix_Music*) - 1);
	SetVolumeMusic(g_nVolumeM);
}

// menu music
void musicFinished2() {
	PlayMusic2();
}

void PlayMusic2() {
	if(g_bIsAudioSupported == false)	return;
	Mix_HookMusicFinished(musicFinished2);
	Mix_PlayMusic(g_apMod[sizeof(g_apMod) / sizeof(Mix_Music*) - 1], 0);
	SetVolumeMusic(g_nVolumeM);
}

int PlaySound(int nSndIndex, int nLoopsCount) {
	if(g_bIsAudioSupported == false)	return -1;
	if(nSndIndex > sizeof(g_pachSnd) / sizeof(const char*))
		return	-1;
	return	Mix_PlayChannel(-1, g_apSnd[nSndIndex], nLoopsCount);
}

void StopSound(int &nChannel) {
	if(g_bIsAudioSupported == false)	return;
	if(nChannel != -1) {
		Mix_HaltChannel(nChannel);
		nChannel	= -1;
	}
}

void SetVolumeMusic(int nVolume) {
	if(g_bIsAudioSupported == false)	return;
	g_nVolumeM	= nVolume;
	if(g_nVolumeM < 0)	g_nVolumeM	= 0;
	if(g_nVolumeM > 10)	g_nVolumeM	= 10;
	const double	fVolumeCorrection	= MIX_MAX_VOLUME / 10.0;
	Mix_VolumeMusic(int(g_nVolumeM * fVolumeCorrection));
	printf("Music volume %d\n", g_nVolumeM);
}

void SetVolumeSound(int nVolume) {
	if(g_bIsAudioSupported == false)	return;
	g_nVolumeS	= nVolume;
	if(g_nVolumeS < 0)	g_nVolumeS	= 0;
	if(g_nVolumeS > 10)	g_nVolumeS	= 10;
	const double	fVolumeCorrection	= MIX_MAX_VOLUME / 10.0;
	Mix_Volume(-1, int(g_nVolumeS * fVolumeCorrection));
	printf("Sound volume %d\n", g_nVolumeS);
}

void EncodeDecode(void *pData, int nLen) {
	Uint8	*p	= (Uint8*)pData;
	Uint8	by	= 0xaa;
	while(nLen--) {
		*p		^= by;
		*p		+= 128;
		by		+= 3;
		p++;
	}
}

//	bMode: true - read, false - write
void ReadWriteConfig(bool bMode) {
	char	achBuf[PATH_MAX];
	FILE	*pFile;

	// try to create directory if not exist
#ifdef _WIN32
	CreateDirectory(g_achUserProfile, 0);
#else	//#elseif __linux__	// linux and mac os x
	mkdir(g_achUserProfile, 0700);
#endif

	sprintf(achBuf, "%sconfig", g_achUserProfile);
	if(bMode == true) {
		if((pFile = fopen(achBuf, "rb"))) {
			fread(&g_nVolumeM, sizeof(g_nVolumeM), 1, pFile);
			fread(&g_nVolumeS, sizeof(g_nVolumeS), 1, pFile);
			fread(&g_bFullscreen, sizeof(g_bFullscreen), 1, pFile);
			fread(&g_bOGL, sizeof(g_bOGL), 1, pFile);
			fread(&g_nBppIndex, sizeof(g_nBppIndex), 1, pFile);
			fread(&g_bShowFps, sizeof(g_bShowFps), 1, pFile);
#if !defined(__linux__) && !defined(FULL_VERSION)
			fread(&g_nUnregisterdCount, sizeof(g_nUnregisterdCount), 1, pFile);
			EncodeDecode(&g_nUnregisterdCount, sizeof(g_nUnregisterdCount));
#endif
			fread(&g_bTutorialMode, sizeof(g_bTutorialMode), 1, pFile);
			fread(&g_bAutoBonusMode, sizeof(g_bAutoBonusMode), 1, pFile);
			fclose(pFile);
			// correct variables on linux, where arkanoidsb 1.1.6 wil be installed before
			if(g_bTutorialMode < 0 || g_bTutorialMode > 1)	g_bTutorialMode	= 1;
			if(g_bAutoBonusMode < 0 || g_bAutoBonusMode > 1)	g_bAutoBonusMode	= 0;
		}
		else {
			printf("error (%d) - %s\n", errno, strerror(errno));
		}
		sprintf(achBuf, "%sscores", g_achUserProfile);
		if((pFile = fopen(achBuf, "rb"))) {
			fread(&g_strHighScore, sizeof(g_strHighScore), 1, pFile);
			fclose(pFile);
			EncodeDecode(&g_strHighScore, sizeof(g_strHighScore));
		}
		else {
			printf("error (%d) - %s\n", errno, strerror(errno));
		}
#if !defined(__linux__) && !defined(FULL_VERSION)
		sprintf(achBuf, "%skey", g_achUserProfile);
		if((pFile = fopen(achBuf, "rb"))) {
			fread(&g_achRegName, sizeof(g_achRegName), 1, pFile);
			fread(&g_achRegKey, sizeof(g_achRegKey), 1, pFile);
			fclose(pFile);
			EncodeDecode(&g_achRegName, sizeof(g_achRegName));
			EncodeDecode(&g_achRegKey, sizeof(g_achRegKey));
		}
		else {
			g_achRegName[0]	= 0;
			g_achRegKey[0]		= 0;
		}
		CheckRegistration();
#endif
	}
	else {
		if((pFile = fopen(achBuf, "wb"))) {
			fwrite(&g_nVolumeM, sizeof(g_nVolumeM), 1, pFile);
			fwrite(&g_nVolumeS, sizeof(g_nVolumeS), 1, pFile);
			fwrite(&g_bFullscreen, sizeof(g_bFullscreen), 1, pFile);
			fwrite(&g_bOGL, sizeof(g_bOGL), 1, pFile);
			fwrite(&g_nBppIndex, sizeof(g_nBppIndex), 1, pFile);
			fwrite(&g_bShowFps, sizeof(g_bShowFps), 1, pFile);
#if !defined(__linux__) && !defined(FULL_VERSION)
			EncodeDecode(&g_nUnregisterdCount, sizeof(g_nUnregisterdCount));
			fwrite(&g_nUnregisterdCount, sizeof(g_nUnregisterdCount), 1, pFile);
#endif
			fwrite(&g_bTutorialMode, sizeof(g_bTutorialMode), 1, pFile);
			fwrite(&g_bAutoBonusMode, sizeof(g_bAutoBonusMode), 1, pFile);
			fclose(pFile);
		}
		else {
			printf("error (%d) - %s\n", errno, strerror(errno));
		}
		sprintf(achBuf, "%sscores", g_achUserProfile);
		if((pFile = fopen(achBuf, "wb"))) {
			EncodeDecode(&g_strHighScore, sizeof(g_strHighScore));
			fwrite(&g_strHighScore, sizeof(g_strHighScore), 1, pFile);
			fclose(pFile);
		}
		else {
			printf("error (%d) - %s\n", errno, strerror(errno));
		}
#if !defined(__linux__) && !defined(FULL_VERSION)
		sprintf(achBuf, "%skey", g_achUserProfile);
		if((pFile = fopen(achBuf, "wb"))) {
			if(g_bIsRegistered == false) {
				g_achRegName[0]	= 0;
				g_achRegKey[0]		= 0;
			}
			EncodeDecode(&g_achRegName, sizeof(g_achRegName));
			EncodeDecode(&g_achRegKey, sizeof(g_achRegKey));
			fwrite(&g_achRegName, sizeof(g_achRegName), 1, pFile);
			fwrite(&g_achRegKey, sizeof(g_achRegKey), 1, pFile);
			fclose(pFile);
		}
#endif
	}
}

void SetVideoMode() {
	char	achBuf[50];
	Uint32	dwFlags	= (g_bOGL == true ? SDL_GLSDL : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF | (g_bFullscreen == true ? SDL_FULLSCREEN : 0);
	printf("Requested video flags:\n");
	printf("  glSDL: %s\n", dwFlags & SDL_GLSDL ? "yes" : "no");
	printf("  Fullscreen: %s\n", dwFlags & SDL_FULLSCREEN ? "yes" : "no");

	/*static int	nFlags	= -1;
	if(dwFlags != nFlags) {
		nFlags	= dwFlags;
		const int anBpp[]	= { 32, 24, 16, 15, 8 };
		for(int i = 0; i < sizeof(anBpp) / sizeof(int); i++) {
			g_vecBpx.push_back(anBpp[i]);
		}
	}
	int	nCount	= g_vecBpx.size();*/
	int	nCount	= sizeof(g_anBpx) / sizeof(int);
	if(g_nBppIndex < 0)	g_nBppIndex	= nCount - 1;
	do {
		g_nBppIndex		%= sizeof(g_anBpx) / sizeof(int);	// correct it to valid value
		g_psurfScreen	= SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, g_anBpx[g_nBppIndex], dwFlags);
		if(!g_psurfScreen) {
			printf("Couldn't set %d x %d x %d bpx video mode: %s.\n", SCREEN_WIDTH, SCREEN_HEIGHT, g_anBpx[g_nBppIndex], SDL_GetError());
			// remove invalid pbx
			//g_vecBpx.erase(g_vecBpx.begin() + g_nBppIndex);
			//nCount	= g_vecBpx.size();
			g_nBppIndex++;
		}
	} while(!g_psurfScreen && --nCount > 0);

	//g_psurfScreen	= SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, (g_bOGL == false ? SDL_GLSDL : 0) | SDL_DOUBLEBUF | (g_bFullscreen == true ? SDL_FULLSCREEN : 0));
	if(!g_psurfScreen) {
		exit(1);
	}
	printf("Video mode %d x %d x %d bpx created.\n", g_psurfScreen->w, g_psurfScreen->h, g_psurfScreen->format->BitsPerPixel);

	// get driver name and hardware info
	const SDL_VideoInfo	*vi	= SDL_GetVideoInfo();
	printf("  driver name: %s\n", SDL_VideoDriverName(achBuf, sizeof(achBuf)));
	printf("  hardware surfaces: %s\n", !vi->hw_available ? "no" : "yes");
	printf("  window manager available: %s\n", !vi->wm_available ? "no" : "yes");
	printf("  hardware to hardware blits accelerated: %s\n", !vi->blit_hw ? "no" : "yes");
	printf("  hardware to hardware colorkey blits accelerated: %s\n", !vi->blit_hw_CC ? "no" : "yes");
	printf("  hardware to hardware alpha blits: %s\n", !vi->blit_hw_A ? "no" : "yes");
	printf("  software to hardware blits: %s\n",  !vi->blit_sw ? "no" : "yes");
	printf("  software to hardware colorkey blits: %s\n",  !vi->blit_sw_CC ? "no" : "yes");
	printf("  software to hardware alpha blits: %s\n",  !vi->blit_sw_A ? "no" : "yes");
	printf("  color fills accelerated: %s\n",  !vi->blit_fill ? "no" : "yes");
	printf("  total video memory: %d Kb\n", vi->video_mem);

	SDL_WM_SetCaption("Arkanoid: Space Ball", 0);
	SDL_ShowCursor(SDL_DISABLE);

	g_Font.LoadFont2("font_small.png", 5, -1, "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
	g_Font2.LoadFont2("font_big.png", 16, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?-%,._:");
	g_Font3.LoadFont2("font_small2.png", 5, -1, "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
	g_FontTutorial.LoadFont2("font_tutorial.png", 5, 1);

	m_pEnergyHole		= LoadImage("energyhole.png");
	m_pMonstPatrol		= LoadImage("monster_patrol.png");
	m_pMonstCopter		= LoadImage("monster_copter.png");
	m_pMonstEye			= LoadImage("monster_eye.png");
	m_pMonstBlackHole	= LoadImage("monster_blackhole.png");
	m_pMonstFighter	= LoadImage("monster_fighter.png");
	m_pMonstHand		= LoadImage("monster_hand.png");
	m_pMonstStone1		= LoadImage("monster_stone1.png");
	m_pMonstStone2		= LoadImage("monster_stone2.png");
	m_pMonstTurbine	= LoadImage("monster_turbine.png");
	m_pMonstUfo			= LoadImage("monster_ufo.png");
	m_pMonstWreckage1	= LoadImage("monster_wreckage1.png");
	m_pMonstWreckage2	= LoadImage("monster_wreckage2.png");
	m_pBall				= LoadImage("balls.png");
	m_pVector			= LoadImage("vector.png");
	m_pFB					= LoadImage("fb.png");
	m_pBullet			= LoadImage("bullets.png");
	m_pExploision		= LoadImage("exploision.png");
	m_pBricks			= LoadImage("bricks.png");
	m_pBricksMovBullets	= LoadImage("bricksmovbullets.png");
	m_pBonuses			= LoadImage("bonuses.png");
	g_pBonusesSmall	= LoadImage("bonuses_small.png");
	g_pBonusesAura		= LoadImage("bonuses_aura.png");
	m_pGameBGanims		= LoadImage("gamebganims.png");
	m_pGameWall			= LoadImage("gamewall.png");
	m_pRacket			= LoadImage("paddle.png");
	m_pUnregistered	= LoadImage("reminderdlg.png");
	m_pBackground		= LoadImage("mainmenubg.jpg");
	m_pBackground2		= LoadImage("mainmenubg2.jpg");
	m_pMainMenuIcons	= LoadImage("mainmenuicons.png");
	m_pCursor			= LoadImage("cursor.png");
	g_pTransp			= LoadImage("transp.png");
	g_pTutorialDlg		= LoadImage("tutorial_dlg.png");
	g_pSinusString		= LoadImage("sinusstring.png");
	g_pOptions			= LoadImage("options.png");
	//g_pBGStars		= LoadImage("stars.png");
	g_Arkanoid.LoadBackground();
}

void UnsetVideoMode() {
	g_Arkanoid.FreeBackground();
	//SDL_FreeSurface(g_pBGStars);
	SDL_FreeSurface(g_pOptions);
	SDL_FreeSurface(g_pSinusString);
	SDL_FreeSurface(g_pTutorialDlg);
	SDL_FreeSurface(g_pTransp);
	SDL_FreeSurface(m_pCursor);
	SDL_FreeSurface(m_pMainMenuIcons);
	SDL_FreeSurface(m_pBackground2);
	SDL_FreeSurface(m_pBackground);
	SDL_FreeSurface(m_pUnregistered);
	SDL_FreeSurface(m_pRacket);
	SDL_FreeSurface(m_pGameWall);
	SDL_FreeSurface(m_pGameBGanims);
	SDL_FreeSurface(g_pBonusesAura);
	SDL_FreeSurface(g_pBonusesSmall);
	SDL_FreeSurface(m_pBonuses);
	SDL_FreeSurface(m_pBricksMovBullets);
	SDL_FreeSurface(m_pBricks);
	SDL_FreeSurface(m_pExploision);
	SDL_FreeSurface(m_pBullet);
	SDL_FreeSurface(m_pFB);
	SDL_FreeSurface(m_pVector);
	SDL_FreeSurface(m_pBall);
	SDL_FreeSurface(m_pMonstWreckage2);
	SDL_FreeSurface(m_pMonstWreckage1);
	SDL_FreeSurface(m_pMonstUfo);
	SDL_FreeSurface(m_pMonstTurbine);
	SDL_FreeSurface(m_pMonstStone2);
	SDL_FreeSurface(m_pMonstStone1);
	SDL_FreeSurface(m_pMonstHand);
	SDL_FreeSurface(m_pMonstFighter);
	SDL_FreeSurface(m_pMonstBlackHole);
	SDL_FreeSurface(m_pMonstEye);
	SDL_FreeSurface(m_pMonstCopter);
	SDL_FreeSurface(m_pMonstPatrol);
	SDL_FreeSurface(m_pEnergyHole);

	g_FontTutorial.Unload();
	g_Font3.Unload();
	g_Font2.Unload();
	g_Font.Unload();
}

#if !defined(__linux__) && !defined(FULL_VERSION)
bool CheckRegistration() {
	const char	achKeyWord[16]     = "oPZmAtk73v4eNjE";
	char    		achTmp[10];
	char			achGenerated[16 + 1];
	char			achKey[16 + 1];
	achGenerated[16]	= 0;
	int	i, a;

	// remove dashes
	int	nLen	= strlen(g_achRegKey);
	for(a = 0, i = 0; i < nLen; i++) {
		if(g_achRegKey[i] != '-')
			achKey[a++]	= g_achRegKey[i];
	}
	achKey[16]	= 0;

	//strcpy(g_achRegName, "Andrey Ugolnik");
	g_bIsRegistered	= false;
	strcpy(achGenerated, achKeyWord);
	nLen	= strlen(g_achRegName);
	for(i = 1; i < 16; i++) {
		for(unsigned int a = 0; a < nLen; a++) {
			sprintf(achTmp, "%2X", achKeyWord[i] ^ g_achRegName[a]);
			achGenerated[i] = achTmp[a % 2];
		}
		sprintf(achTmp, "%2X", achGenerated[i - 1] ^ achKeyWord[i] ^ nLen);
		switch(achTmp[1]) {
			case '0': achTmp[1]	= 'H';	break;
			case '1': achTmp[1]	= 'Z';	break;
			case '8': achTmp[1]	= 'X';	break;
			case 'A': achTmp[1]	= 'N';	break;
			case 'B': achTmp[1]	= 'K';	break;
			case 'C': achTmp[1]	= 'W';	break;
			case 'D': achTmp[1]	= 'Y';	break;
			case 'G': achTmp[1]	= 'P';	break;
		}
		achGenerated[i - 1]     = achTmp[1];
	}
	//g_Font2.DrawString(240, 300, achGenerated);
	//strcpy(g_achRegKey, achGenerated);
	if(CheckForBlackList(achGenerated) == false && strcmp(achGenerated, achKey) == 0)
		g_bIsRegistered	= true;

	return  g_bIsRegistered;
}

bool CheckForBlackList(const char *pchKey) {
	const char *pchBlacklistedKeys[]	= {
		"EKW3-4X54-Z72W-N7H5",
	};
	for(int i = 0; i < sizeof(pchBlacklistedKeys) / sizeof(const char**); i++) {
		//printf("blacklisted %s\n", pchBlacklistedKeys[i]);
		if(strcmp(pchBlacklistedKeys[i], pchKey) == 0)
			return true;
	}
	return false;
}
#endif

void SwitchFullscreen() {
//#ifdef __linux__
//	SDL_WM_ToggleFullScreen(g_psurfScreen);	// currently works only in linux
//#else
	UnsetVideoMode();
	SetVideoMode();
//#endif
}

bool DrawIntro() {
	static int		nRotates	= 0;
	static int		nFrame	= 0;
	static SDL_Surface	*pIntro, *pIntro2, *pSDL;
	static bool	bLoaded	= false;
	if(bLoaded == false) {
		bLoaded	= true;
		pIntro	= LoadImage("intro.png");
		pIntro2	= LoadImage("intro2.png");
		pSDL		= LoadImage("sdl_minibox.png");
	}
	SDL_FillRect(g_psurfScreen, 0, 0);
	SDL_Rect	rc;
	Blit((SCREEN_WIDTH - 80) / 2, (SCREEN_HEIGHT - 106) / 2, pIntro2, 0);
	rc.w	= rc.h	= 64;
	rc.x	= (nFrame % 4) * 64;
	rc.y	= (nFrame / 4) * 64;
	Blit((SCREEN_WIDTH - 64) / 2, (SCREEN_HEIGHT - 106) / 2 + 8, pIntro, &rc);
	Blit(SCREEN_WIDTH - 88 - 5, SCREEN_HEIGHT - 44 - 5, pSDL, 0);

	static Uint32	dwTime	= 0;
	if(IsKeyPressed(SDLK_ESCAPE) || g_bMouseRB == true || g_bMouseLB == true) {
		g_bMouseRB	= g_bMouseLB	= false;
		nFrame	= 14;
		nRotates	= 100;
		dwTime	= 0;
	}
	if(dwTime + 60 < SDL_GetTicks()) {
		dwTime  = SDL_GetTicks();
		nFrame++;
		nFrame  %= 16;
		if(nFrame == 15) {
			nRotates++;
			if(nRotates > 10) {
				SDL_FreeSurface(pSDL);
				SDL_FreeSurface(pIntro2);
				SDL_FreeSurface(pIntro);
				EnableCursor(true);
				return true;
			}
		}
	}

	return false;
}

void EnableCursor(bool bEnable) {
	static bool	bAlreadyEnabled	= false;
	if(bAlreadyEnabled == false && bEnable == true) {
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		g_bIsCursorVisible	= true;
	}
	else if(bAlreadyEnabled == true && bEnable == false) {
		SDL_WM_GrabInput(SDL_GRAB_ON);
		g_bIsCursorVisible	= false;
	}
	bAlreadyEnabled	= bEnable;
}
