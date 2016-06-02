// arkanoidsb.cpp : Defines the entry point for the application.
//

#include "arkanoidsb.h"
#include <SDL_syswm.h>
#include <sys/stat.h>
#include "version.h"


void SetVideoMode();
void UnsetVideoMode();
void CommonQuit();
bool UpdateKeys();
void readConfig();
void writeConfig();
void PlayMusic2();
void SwitchFullscreen();
bool DrawIntro();

char g_achUserProfile[PATH_MAX] = { 0 };

SDL_Surface* g_psurfScreen = 0;
bool g_bFullscreen = true;
bool g_bOGL = true;
int g_anBpx[5] = { 32, 24, 16, 15, 8 };
int g_nBppIndex = 0;
bool g_bShowFps = false;
bool g_bTutorialMode = true;
bool g_bAutoBonusMode = true;

float g_fSpeedCorrection;
float g_fCos[360];
float g_fSin[360];

SDL_Joystick* g_pJoystick = 0;
bool g_bIsJoystickSupported	= false;

bool g_bActive = true;

int g_keysStateCount = 0;
Uint8* g_keysState = 0;
Uint8* g_keysStateLast = 0;
Uint32 g_modState = 0;

bool g_bMouseRB = false;
bool g_bMouseLB = false;
bool g_bIsCursorVisible = false;
float g_nMouseDX = 0;
float g_nMouseDY = 0;
int g_nCursorX = SCREEN_WIDTH / 2;
int g_nCursorY = SCREEN_HEIGHT / 2;

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
SDL_Surface* g_pGameBG = 0;
SDL_Surface* m_pGameBGanims;
SDL_Surface* m_pGameWall;
SDL_Surface* m_pRacket;
SDL_Surface* m_pBackground;
SDL_Surface* m_pBackground2;
SDL_Surface* m_pMainMenuIcons;
SDL_Surface* m_pCursor;
SDL_Surface* g_pTransp;
SDL_Surface* g_pTutorialDlg;
SDL_Surface* g_pSinusString;
SDL_Surface* g_pOptions;
//SDL_Surface* g_pBGStars;

const char* g_pachSnd[] =
{
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
Mix_Chunk* g_apSnd[sizeof(g_pachSnd) / sizeof(const char*)];
Mix_Music* g_apMod[3];
bool g_bIsAudioSupported = true;
int g_nVolumeM = 3;
int g_nVolumeS = 4;

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


int main(int argc, char *argv[])
{
	int	i;
	char	achTemp[PATH_MAX];

#ifdef __linux__
	if(getenv("XDG_CONFIG_HOME")) {
		sprintf(g_achUserProfile, "%s/arkanoidsb/", getenv("XDG_CONFIG_HOME"));
	}
	else {
		sprintf(g_achUserProfile, "%s/.config/arkanoidsb/", getenv("HOME") ? getenv("HOME") : ".");
	}
#elif _WIN32
	int		nLen	= sizeof(achTemp);
	//GetUserProfileDirectory(0, achTemp, nLen);
	ExpandEnvironmentStrings("%USERPROFILE%", achTemp, nLen);
	sprintf(g_achUserProfile, "%s\\.arkanoidsb\\", achTemp);
#else
	sprintf(g_achUserProfile, "%s/Library/Application Support/arkanoidsb/", getenv("HOME") ? getenv("HOME") : ".");
#endif
	printf("Arkanoid: Space Ball by 'WE' Group. Copyright (c) 2006-2012.\n");
	printf("version %s.\n", AutoVersion::FULLVERSION_STRING);
	printf("Users config dir: %s\n", g_achUserProfile);
	readConfig();

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
		for(size_t i = 0; i < sizeof(g_pachSnd) / sizeof(const char*); i++) {
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
		for(size_t i = 0; i < sizeof(g_apMod) / sizeof(Mix_Music*); i++) {
#ifdef __MACOSX__
 			if(i != 2)	sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module%.2d.ogg", (int)i + 1);
 			else		sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module03.s3m");
#else
 			if(i != 2)	sprintf(achTemp, "res/module%.2d.ogg", (int)i + 1);
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
				if(i != 2) { sprintf(achTemp, "../Resources/module%.2d.ogg", (int)i + 1); strcat(achPath, achTemp); }
				else		strcat(achPath, "../Resources/module03.s3m");
#else
				if(i != 2) { sprintf(achTemp, "res/module%.2d.ogg", (int)i + 1); strcat(achPath, achTemp); }
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

    for(int i = 0; i < 360; i++)
    {
        g_fCos[i] = cosf((M_PI / 180.0f) * i);
        g_fSin[i] = sinf((M_PI / 180.0f) * i);
    }

    g_keysState = SDL_GetKeyState(&g_keysStateCount);
    g_keysStateLast = new Uint8[g_keysStateCount];
    memset(g_keysStateLast, 0, g_keysStateCount * sizeof(Uint8));

	int		nFrame		= 0;
	Uint32	nFPStime	= 0;
	float	fFPS		= 0;
	Uint32	nPrevTime	= 0;
	g_fSpeedCorrection	= 1.0f;
	const float fPerFrameMs     = 1000.0f / DESIRED_FPS;

	CLevelEditor	m_LevelEditor;
	//g_nGameMode	= APPS_EDITOR;

	while(UpdateKeys() == false && g_nGameMode != APPS_EXIT)
	{
		Uint32 nTimeCurrent = SDL_GetTicks();
		if(g_bActive == true)
		{
			nFrame++;
			Uint32	nFPSelapsed		= nTimeCurrent - nFPStime;
			if(nFPSelapsed >= 1000) {
				fFPS		= (nFrame * 1000.0) / nFPSelapsed;
				nFrame		= 0;
				nFPStime	= nTimeCurrent;
			}

			g_fSpeedCorrection = (float)(nTimeCurrent - nPrevTime) / (DESIRED_FPS / 10.0f);
			nPrevTime				= nTimeCurrent;

			// volume manager
			if(g_modState & KMOD_SHIFT) {
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

/*			// save screenshot to profile dir
			if((g_modState & KMOD_CTRL) && IsKeyPressed(SDLK_s) && IsKeyStateChanged(SDLK_s)) {
				char    achPath[PATH_MAX];
				time_t  osTime	= time(0);
				struct tm	local	= *localtime(&osTime);
				sprintf(achPath, "%sarkanoidsb-%4d%2d%2d-%2d%2d.bmp", g_achUserProfile, local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min);
				SDL_SaveBMP(g_psurfScreen, achPath);
			}
*/			if(IsKeyPressed(SDLK_f) && IsKeyStateChanged(SDLK_f)) {
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
			if(nDelay > 0) {
				SDL_Delay(nDelay);
			}
			else {
				SDL_Delay(0);
			}
		}
		else {
			SDL_FillRect(g_psurfScreen, 0, 0);
			g_Font2.DrawString(0, SCREEN_HEIGHT / 2, "FOCUS LOST - GAME INACTIVE", 2);
			SDL_Flip(g_psurfScreen);
			SDL_Delay(200);
		}
	}

    delete[] g_keysStateLast;

	for(size_t i = 0; i < sizeof(g_apMod) / sizeof(Mix_Music*); i++) {
		if(g_apMod[i])	Mix_FreeMusic(g_apMod[i]);
	}

	for(size_t i = 0; i < sizeof(g_apSnd) / sizeof(Mix_Chunk*); i++) {
		if(g_apSnd[i])	Mix_FreeChunk(g_apSnd[i]);
	}

	writeConfig();
	UnsetVideoMode();
	SDL_JoystickClose(g_pJoystick);
	exit(0);
}

void CommonQuit() {
	Mix_CloseAudio();
	SDL_Quit();
}

SDL_Surface* LoadImage(const char* filename, const Uint32 colorKey)
{
    SDL_Surface* optimizedImage = 0; //The optimized image that will be used

    // try to load image from resource from
    unsigned int nDataLen;
    unsigned char* data = g_Resource.GetDataAllocMem(filename, nDataLen);
    if(data)
    {
        SDL_Surface* tmp = IMG_Load_RW(SDL_RWFromMem(data, nDataLen), 1);
        g_Resource.FreeMem(data);
        //SDL_Surface* tmp = IMG_Load(filename);
        if(tmp)
        {
            if(colorKey != 0xff000000)
            {
                Uint32 key = SDL_MapRGB(g_psurfScreen->format, (colorKey>>16)&0xff, (colorKey>>8)&0xff, colorKey&0xff);
                int flag = SDL_SRCCOLORKEY | (g_bOGL ? 0 : SDL_RLEACCEL);
                SDL_SetColorKey(tmp, flag, key | SDL_ALPHA_TRANSPARENT);
                optimizedImage = SDL_DisplayFormat(tmp); //Create an optimized image
            }
            else
            {
                optimizedImage = SDL_DisplayFormatAlpha(tmp); //Create an optimized image
            }
            SDL_FreeSurface(tmp); //Free the old image
            printf(" done.\n");
        }
        else
        {
            printf("  --%s\n", SDL_GetError());
        }
    }

    return optimizedImage; //Return the optimized image
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

bool IsKeyPressed(int key)
{
    return g_keysState[key] == SDL_PRESSED;
}

bool IsKeyStateChanged(int key)
{
    return g_keysStateLast[key] != g_keysState[key];
}

void FadeScreen()
{
    for(int x = 0; x < 4; x++)
    {
        for(int y = 0; y < 4; y++)
        {
            Blit(x * 160, y * 120, g_pTransp, 0);
        }
    }
}

bool UpdateKeys()
{
    g_nMouseDX = 0;
    g_nMouseDY = 0;

    memcpy(g_keysStateLast, g_keysState, g_keysStateCount * sizeof(Uint8));
    g_modState = SDL_GetModState();

    SDL_Event evt;
    while(SDL_PollEvent(&evt))
    {
        switch(evt.type)
        {
        case SDL_ACTIVEEVENT:
            if(evt.active.state == SDL_APPINPUTFOCUS || evt.active.state == SDL_APPACTIVE)
            {
                if(evt.active.gain == 0)
                {
                    if(g_nGameMode == APPS_GAME)
                    {
                        g_Arkanoid.SetPause();
                    }
                    Mix_PauseMusic();
                    //printf("Arkanoid: Space Ball inactive\n");
                    g_bActive = false;
                }
            }
            else
            {
                if(evt.active.gain)
                {
                    Mix_ResumeMusic();
                    //printf("Arkanoid: Space Ball active\n");
                    g_bActive = true;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            switch(evt.button.button)
            {
            case 1:
                g_bMouseLB = true;
                break;
            case 3:
                g_bMouseRB = true;
                break;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            switch(evt.button.button)
            {
            case 1:
                g_bMouseLB = false;
                break;
            case 3:
                g_bMouseRB = false;
                break;
            case 4:
                if(g_nGameMode == APPS_GAME)
                    g_Bonus.SetPosStack(false);
                break;
            case 5:
                if(g_nGameMode == APPS_GAME)
                    g_Bonus.SetPosStack(true);
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            g_nMouseDX += evt.motion.xrel;
            g_nMouseDY += evt.motion.yrel;
            g_nCursorX = evt.motion.x;
            g_nCursorY = evt.motion.y;
            break;

        case SDL_KEYDOWN:
            if(
               ((evt.key.keysym.mod & (KMOD_RMETA | KMOD_LMETA)) && evt.key.keysym.sym == SDLK_q)
               || ((evt.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && (evt.key.keysym.sym == SDLK_x || evt.key.keysym.sym == SDLK_F4))
               )
            { // Alt+X or Command+Q pressed - quit game
                if(g_nGameMode != APPS_INTRO)
                {
                    return true;
                }
            }
            else if((evt.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && evt.key.keysym.sym == SDLK_RETURN)
            {
                g_bFullscreen = !g_bFullscreen;
                SwitchFullscreen();
            }
            break;

        case SDL_JOYBUTTONDOWN: // Handle Joystick Button Presses
            if(evt.jbutton.button == 0) {
                g_bMouseLB = true;
            }
            if(evt.jbutton.button == 1) {
                g_bMouseRB = true;
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
                g_bMouseLB = false;
            }
            if(evt.jbutton.button == 1) {
                g_bMouseRB = false;
            }
            break;
        //case SDL_JOYAXISMOTION: // Handle Joystick Motion
            //if((evt.jaxis.value < -3200) || (evt.jaxis.value > 3200)) {
                //int nDelta = int(evt.jaxis.value / 32768.0 * g_fSpeedCorrection * 10);
                //if(evt.jaxis.axis == 0) {
                    //g_nCursorX += nDelta;
                    //g_nMouseDX += nDelta;
                //}
                //if(evt.jaxis.axis == 1) {
                    //g_nCursorY += nDelta;
                    //g_nMouseDY += nDelta;
                //}
            //}
            //break;

        case SDL_QUIT:
            return true;
            break;
        }
    }

    if(g_bIsJoystickSupported == true)
    {
        SDL_JoystickUpdate();
        //if(SDL_JoystickGetButton(g_pJoystick, 0)) {
            //g_bMouseLB	= true;
        //}
        //else {
            //g_bMouseLB	= false;
        //}
        //if(SDL_JoystickGetButton(g_pJoystick, 1)) {
            //g_bMouseRB	= true;
        //}
        //else {
            //g_bMouseRB	= false;
        //}
        //if(SDL_JoystickGetButton(g_pJoystick, 2)) {
            //if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(false);
        //}
        //else if(SDL_JoystickGetButton(g_pJoystick, 3)) {
            //if(g_nGameMode == APPS_GAME) g_Bonus.SetPosStack(true);
        //}
        //if(SDL_JoystickGetButton(g_pJoystick, 4)) {
            //g_Arkanoid.SendEsc();
        //}
        int nDelta = SDL_JoystickGetAxis(g_pJoystick, 0);
        if((nDelta < -3200) || (nDelta > 3200)) {
            nDelta = int(nDelta / 32768.0 * g_fSpeedCorrection * 4);
            g_nCursorX += nDelta;
            g_nMouseDX += nDelta;
        }
        nDelta = SDL_JoystickGetAxis(g_pJoystick, 1);
        if((nDelta < -3200) || (nDelta > 3200)) {
            nDelta = int(nDelta / 32768.0 * g_fSpeedCorrection * 4);
            g_nCursorY += nDelta;
            g_nMouseDY += nDelta;
        }
    }

    g_nCursorX = std::max(g_nCursorX, 0);
    g_nCursorX = std::min(g_nCursorX, SCREEN_WIDTH);
    g_nCursorY = std::max(g_nCursorY, 0);
    g_nCursorY = std::min(g_nCursorY, SCREEN_HEIGHT);

    return false;
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
	if(nSndIndex > (int)(sizeof(g_pachSnd) / sizeof(const char*)))
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
	if(g_nVolumeM < 0)	g_nVolumeM = 0;
	if(g_nVolumeM > 10)	g_nVolumeM = 10;
	const float fVolumeCorrection = MIX_MAX_VOLUME / 10.0f;
	Mix_VolumeMusic(int(g_nVolumeM * fVolumeCorrection));
	printf("Music volume %d\n", g_nVolumeM);
}

void SetVolumeSound(int nVolume) {
	if(g_bIsAudioSupported == false)	return;
	g_nVolumeS	= nVolume;
	if(g_nVolumeS < 0)	g_nVolumeS = 0;
	if(g_nVolumeS > 10)	g_nVolumeS = 10;
	const float fVolumeCorrection = MIX_MAX_VOLUME / 10.0f;
	Mix_Volume(-1, int(g_nVolumeS * fVolumeCorrection));
	printf("Sound volume %d\n", g_nVolumeS);
}

void EncodeDecode(void *data, int size)
{
    Uint8* p = (Uint8*)data;
    Uint8 by = 0xaa;
    while(size--)
    {
        *p ^= by;
        *p += 128;
        by += 3;
        p++;
    }
}

void readConfig()
{
    char buffer[PATH_MAX];

    sprintf(buffer, "%sconfig", g_achUserProfile);

    FILE* file = fopen(buffer, "rb");
    if(file)
    {
        fread(&g_nVolumeM, sizeof(g_nVolumeM), 1, file);
        fread(&g_nVolumeS, sizeof(g_nVolumeS), 1, file);
        fread(&g_bFullscreen, sizeof(g_bFullscreen), 1, file);
        fread(&g_bOGL, sizeof(g_bOGL), 1, file);
        fread(&g_nBppIndex, sizeof(g_nBppIndex), 1, file);
        fread(&g_bShowFps, sizeof(g_bShowFps), 1, file);
        fread(&g_bTutorialMode, sizeof(g_bTutorialMode), 1, file);
        fread(&g_bAutoBonusMode, sizeof(g_bAutoBonusMode), 1, file);
        fclose(file);
    }
    else
    {
        printf("error (%d) - %s\n", errno, strerror(errno));
    }

    sprintf(buffer, "%sscores", g_achUserProfile);
    file = fopen(buffer, "rb");
    if(file)
    {
        fread(&g_strHighScore, sizeof(g_strHighScore), 1, file);
        fclose(file);
        EncodeDecode(&g_strHighScore, sizeof(g_strHighScore));
    }
    else
    {
        printf("error (%d) - %s\n", errno, strerror(errno));
    }
}

void writeConfig()
{
    // try to create directory if not exist
#ifdef _WIN32
    CreateDirectory(g_achUserProfile, 0);
#else	//#elseif __linux__	// linux and mac os x
    mkdir(g_achUserProfile, 0700);
#endif

    char buffer[PATH_MAX];

    sprintf(buffer, "%sconfig", g_achUserProfile);
    FILE* file = fopen(buffer, "wb");
    if(file)
    {
        fwrite(&g_nVolumeM, sizeof(g_nVolumeM), 1, file);
        fwrite(&g_nVolumeS, sizeof(g_nVolumeS), 1, file);
        fwrite(&g_bFullscreen, sizeof(g_bFullscreen), 1, file);
        fwrite(&g_bOGL, sizeof(g_bOGL), 1, file);
        fwrite(&g_nBppIndex, sizeof(g_nBppIndex), 1, file);
        fwrite(&g_bShowFps, sizeof(g_bShowFps), 1, file);
        fwrite(&g_bTutorialMode, sizeof(g_bTutorialMode), 1, file);
        fwrite(&g_bAutoBonusMode, sizeof(g_bAutoBonusMode), 1, file);
        fclose(file);
    }
    else
    {
        printf("error (%d) - %s\n", errno, strerror(errno));
    }

    sprintf(buffer, "%sscores", g_achUserProfile);
    file = fopen(buffer, "wb");
    if(file)
    {
        EncodeDecode(&g_strHighScore, sizeof(g_strHighScore));
        fwrite(&g_strHighScore, sizeof(g_strHighScore), 1, file);
        fclose(file);
    }
    else
    {
        printf("error (%d) - %s\n", errno, strerror(errno));
    }
}

void SetVideoMode()
{
    SDL_WM_SetCaption("Arkanoid: Space Ball", 0);

    char achBuf[50];
    Uint32 dwFlags = (g_bOGL == true ? SDL_GLSDL : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF | (g_bFullscreen == true ? SDL_FULLSCREEN : 0);
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
    int nCount = sizeof(g_anBpx) / sizeof(int);
    if(g_nBppIndex < 0)
    {
        g_nBppIndex = nCount - 1;
    }
    do
    {
        g_nBppIndex %= sizeof(g_anBpx) / sizeof(int); // correct it to valid value
        g_psurfScreen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, g_anBpx[g_nBppIndex], dwFlags);
        if(!g_psurfScreen)
        {
            printf("Couldn't set %d x %d x %d bpx video mode: %s.\n", SCREEN_WIDTH, SCREEN_HEIGHT, g_anBpx[g_nBppIndex], SDL_GetError());
            // remove invalid pbx
            //g_vecBpx.erase(g_vecBpx.begin() + g_nBppIndex);
            //nCount	= g_vecBpx.size();
            g_nBppIndex++;
        }
    } while(!g_psurfScreen && --nCount > 0);

    //g_psurfScreen	= SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, (g_bOGL == false ? SDL_GLSDL : 0) | SDL_DOUBLEBUF | (g_bFullscreen == true ? SDL_FULLSCREEN : 0));
    if(!g_psurfScreen)
    {
        exit(1);
    }
    printf("Video mode %d x %d x %d bpx created.\n", g_psurfScreen->w, g_psurfScreen->h, g_psurfScreen->format->BitsPerPixel);

    // get driver name and hardware info
    const SDL_VideoInfo* vi = SDL_GetVideoInfo();
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

    SDL_ShowCursor(SDL_DISABLE);

    // center window
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if(SDL_GetWMInfo(&info) > 0)
    {
#if defined(__linux__)
        if(info.subsystem == SDL_SYSWM_X11)
        {
            info.info.x11.lock_func();
            int w = DisplayWidth(info.info.x11.display, DefaultScreen(info.info.x11.display));
            int h = DisplayHeight(info.info.x11.display, DefaultScreen(info.info.x11.display));
            int x = (w - SCREEN_WIDTH)/2;
            int y = (h - SCREEN_HEIGHT)/2;
            XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
            info.info.x11.unlock_func();
        }
#elif defined(WIN32)
        RECT rc;
        HWND hwnd = info.window;
        int w = GetSystemMetrics(SM_CXSCREEN);
        int h = GetSystemMetrics(SM_CYSCREEN);
        GetWindowRect(hwnd, &rc);
        int x = (w - (rc.right-rc.left))/2;
        int y = (h - (rc.bottom-rc.top))/2;
        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
#endif
    }

    g_Font.LoadFont2("font_small.png", 5, -1, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
    g_Font2.LoadFont2("font_big.png", 16, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?-%,._:");
    g_Font3.LoadFont2("font_small2.png", 5, -1, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
    g_FontTutorial.LoadFont2("font_tutorial.png", 5, 1, nullptr);

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

void UnsetVideoMode()
{
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

void SwitchFullscreen()
{
    //SDL_WM_ToggleFullScreen(g_psurfScreen); // currently works only in linux
    UnsetVideoMode();
    SetVideoMode();
}

bool DrawIntro()
{
    static int nRotates = 0;
    static int nFrame = 0;
    static SDL_Surface* intro = 0;
    static SDL_Surface* intro2 = 0;
    static SDL_Surface* sdlLogo = 0;
    if(intro == 0)
    {
        intro = LoadImage("intro.png");
        intro2 = LoadImage("intro2.png");
        sdlLogo = LoadImage("sdl_minibox.png");
    }

    SDL_FillRect(g_psurfScreen, 0, 0);
    Blit((SCREEN_WIDTH - 80) / 2, (SCREEN_HEIGHT - 106) / 2, intro2, 0);
    SDL_Rect rc;
    rc.w = 64;
    rc.h = 64;
    rc.x = (nFrame % 4) * 64;
    rc.y = (nFrame / 4) * 64;
    Blit((SCREEN_WIDTH - 64) / 2, (SCREEN_HEIGHT - 106) / 2 + 8, intro, &rc);
    Blit(SCREEN_WIDTH - 88 - 5, SCREEN_HEIGHT - 44 - 5, sdlLogo, 0);

    static Uint32 dwTime = 0;
    if(IsKeyPressed(SDLK_ESCAPE) || g_bMouseRB || g_bMouseLB)
    {
        g_bMouseRB = false;
        g_bMouseLB = false;
        nFrame = 14;
        nRotates = 100;
        dwTime = 0;
    }
    if(dwTime + 60 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        nFrame++;
        nFrame %= 16;
        if(nFrame == 15)
        {
            nRotates++;
            if(nRotates > 10)
            {
                EnableCursor(true);
                SDL_FreeSurface(sdlLogo);
                SDL_FreeSurface(intro2);
                SDL_FreeSurface(intro);
                intro = 0;
                return true;
            }
        }
    }

    return false;
}

void EnableCursor(bool enable)
{
    static bool isEnabled = false;
    if(isEnabled != enable)
    {
        SDL_WM_GrabInput(enable ? SDL_GRAB_OFF : SDL_GRAB_ON);
    }
    isEnabled = enable;
    g_bIsCursorVisible = enable;
}

