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
#include "game.h"
#include "leveleditor.h"
#include "mainmenu.h"
#include "mystring.h"
#include "resource.h"
#include "tutorialdlg.h"
#include "version.h"
#include "videosystem/videosystem.h"

#include "glSDL.h"
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <cmath>
#include <cstdlib>

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#define SDL_GetKeyState SDL_GetKeyboardState
#define SDL_DisplayFormat SDL_DisplayFormatAlpha
#endif

int g_bppList[5] = { 32, 24, 16, 15, 8 };

float g_fSpeedCorrection;
float g_fCos[360];
float g_fSin[360];

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

int g_nGameMode = APPS_INTRO;

CLevelEditor m_LevelEditor;

const char* g_pachSnd[] = {
    "snd_electric.wav", //  0
    "snd_applause.wav", //  1
    "snd_patrol.wav",   //  2
    "snd_turbine.wav",  //  3
    "snd_click.wav",    //  4
    "snd_over.wav",     //  5
    "snd_impact1.wav",  //  6
    "snd_impact2.wav",  //  7
    "snd_laser.wav",    //  8
    "snd_bonusup.wav",  //  9
    "snd_bonusnew.wav", // 10
    "snd_paddle.wav",   // 11
    "snd_missile.wav",  // 12
    "snd_plasma.wav",   // 13
    "snd_lostball.wav", // 14
    "snd_explode.wav",  // 15
};
Mix_Chunk* g_apSnd[sizeof(g_pachSnd) / sizeof(const char*)];
Mix_Music* g_apMod[3];
bool g_bIsAudioSupported = true;

int main(int argc, char* argv[])
{
    printf("%s by 'WE' Group. Copyright (c) 2006.\n", APP_Title);
    printf("version %d.%d.%d.\n", APP_VerMajor, APP_VerMinor, APP_VerRelease);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL-video: %s\n", SDL_GetError());
        exit(-2);
    }
    atexit(CommonQuit);

    const int imgFlags = IMG_INIT_PNG;
    const int imgInitted = IMG_Init(imgFlags);
    if ((imgInitted & imgFlags) != imgFlags)
    {
        printf("Couldn't initialize SDL_Image: %s\n", SDL_GetError());
        exit(-2);
    }

    a::initialize();

    char achPath[MAX_PATH];
    strcpy(achPath, argv[0]);

    const char* resPath = nullptr;
#if defined(__MACOSX__)
    resPath = "arkanoidsb.app/Contents/Resources/arkanoidsb.pak";
#elif defined(EMSCRIPTEN)
    resPath = "arkanoidsb.pak";
#else
    resPath = "res/arkanoidsb.pak";
#endif

    if (a::res()->Open(resPath) == false)
    {
        char* pchEnd;
        if ((pchEnd = strrchr(achPath, '\\')) == 0 && (pchEnd = strrchr(achPath, '/')) == 0)
        {
            exit(-1);
        }
        *(pchEnd + 1) = 0;
#ifdef __MACOSX__
        strcat(achPath, "../Resources/arkanoidsb.pak");
#else
        strcat(achPath, "res/arkanoidsb.pak");
#endif
        if (false == a::res()->Open(achPath))
        {
            exit(-1);
        }
    }

    EnableCursor(false);

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    {
        printf("Couldn't initialize SDL-audio: %s\n", SDL_GetError());
        printf("  continuing without audio support.\n");
        g_bIsAudioSupported = false;
    }
    else
    {
        // open 44.1KHz, signed 16bit, system byte order,
        //      stereo audio, using 1024 byte chunks
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
        {
            printf("Couldn't OpenAudio: %s\n", Mix_GetError());
            printf("  continuing without audio support.\n");
            g_bIsAudioSupported = false;
        }
    }

    unsigned nDataLen;
    unsigned char* pbyData;
#ifndef __MACOSX__
    pbyData = a::res()->GetDataAllocMem("icon.bmp", nDataLen);
    SDL_Surface* pIcon = SDL_LoadBMP_RW(SDL_RWFromConstMem(pbyData, nDataLen), 1);
    a::res()->FreeMem(pbyData);
    if (0 != pIcon)
    {
        printf(" done.\n");
    }
    else
    {
        printf("  %s\n", SDL_GetError());
    }

    SDL_SetColorKey(pIcon, SDL_SRCCOLORKEY, SDL_MapRGB(pIcon->format, 0, 0, 0));
    SDL_WM_SetIcon(pIcon, NULL);
#endif

    setVideoMode();
    SDL_WarpMouse(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    char achTemp[MAX_PATH];

    if (g_bIsAudioSupported == true)
    {
        // loading sound effects
        for (size_t i = 0; i < sizeof(g_pachSnd) / sizeof(g_pachSnd[0]); i++)
        {
            // try to load image from resource from
            pbyData = a::res()->GetDataAllocMem(g_pachSnd[i], nDataLen);
            if (pbyData)
            {
                auto rwops = SDL_RWFromConstMem(pbyData, nDataLen);
                g_apSnd[i] = Mix_LoadWAV_RW(rwops, 1);
                a::res()->FreeMem(pbyData);
                if (g_apSnd[i] != nullptr)
                {
                    printf(" done.\n");
                }
                else
                {
                    printf("  %s\n", SDL_GetError());
                }
            }
        }

//loading modules
#if !defined(EMSCRIPTEN)
        for (size_t i = 0; i < sizeof(g_apMod) / sizeof(g_apMod[0]); i++)
        {
#if defined(__MACOSX__)
            if (i != 2)
            {
                sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module%.2d.ogg", (int)i + 1);
            }
            else
            {
                sprintf(achTemp, "arkanoidsb.app/Contents/Resources/module03.s3m");
            }
#else
            if (i != 2)
            {
                sprintf(achTemp, "res/module%.2d.ogg", (int)i + 1);
            }
            else
            {
                sprintf(achTemp, "res/module03.s3m");
            }
#endif
            printf("Loading module %s", achTemp);
            g_apMod[i] = Mix_LoadMUS(achTemp);
            if (0 != g_apMod[i])
            {
                printf(" done.\n");
            }
            else
            {
                char* pchEnd;
                if ((pchEnd = strrchr(achPath, '\\')) == 0 && (pchEnd = strrchr(achPath, '/')) == 0)
                {
                    achPath[0] = 0;
                }
                *(pchEnd + 1) = 0;
#ifdef __MACOSX__
                if (i != 2)
                {
                    sprintf(achTemp, "../Resources/module%.2d.ogg", (int)i + 1);
                    strcat(achPath, achTemp);
                }
                else
                {
                    strcat(achPath, "../Resources/module03.s3m");
                }
#else
                if (i != 2)
                {
                    sprintf(achTemp, "res/module%.2d.ogg", (int)i + 1);
                    strcat(achPath, achTemp);
                }
                else
                {
                    strcat(achPath, "res/module03.s3m");
                }
#endif
                g_apMod[i] = Mix_LoadMUS(achPath);
                if (0 != g_apMod[i])
                {
                    printf(" done.\n");
                }
                else
                {
                    printf("  %s\n", SDL_GetError());
                }
            }
        }
        SetVolumeSound(a::opt().soundVolume);
        PlayMusic2();
#endif
    }

    for (int i = 0; i < 360; i++)
    {
        g_fCos[i] = cosf((M_PI / 180.0f) * i);
        g_fSin[i] = sinf((M_PI / 180.0f) * i);
    }

    g_keysState = SDL_GetKeyState(&g_keysStateCount);
    g_keysStateLast = new Uint8[g_keysStateCount];
    memset(g_keysStateLast, 0, g_keysStateCount * sizeof(Uint8));

//g_nGameMode = APPS_EDITOR;

#if defined(EMSCRIPTEN)
    emscripten_set_main_loop(gameLoop, 0, 0);
#else
    gameLoop();
#endif

    delete[] g_keysStateLast;

    for (size_t i = 0; i < sizeof(g_apMod) / sizeof(Mix_Music*); i++)
    {
        if (g_apMod[i])
        {
            Mix_FreeMusic(g_apMod[i]);
        }
    }

    for (size_t i = 0; i < sizeof(g_apSnd) / sizeof(Mix_Chunk*); i++)
    {
        if (g_apSnd[i])
        {
            Mix_FreeChunk(g_apSnd[i]);
        }
    }

    unsetVideoMode();

    exit(0);
}

void CommonQuit()
{
    a::deinitialize();

    Mix_CloseAudio();
    SDL_Quit();
}

void SetRect(SDL_Rect* pRc, int nX, int nY, int nW, int nH)
{
    pRc->x = nX;
    pRc->y = nY;
    pRc->w = nW;
    pRc->h = nH;
}

bool IsKeyPressed(int key)
{
    return g_keysState[key] == SDL_PRESSED;
}

bool IsKeyStateChanged(int key)
{
    return g_keysStateLast[key] != g_keysState[key];
}

bool UpdateKeys()
{
    g_nMouseDX = 0;
    g_nMouseDY = 0;

    memcpy(g_keysStateLast, g_keysState, g_keysStateCount * sizeof(Uint8));
    g_modState = SDL_GetModState();

    SDL_Event evt;
    while (SDL_PollEvent(&evt))
    {
        switch (evt.type)
        {
        case SDL_ACTIVEEVENT:
            if (evt.active.state == SDL_APPINPUTFOCUS || evt.active.state == SDL_APPACTIVE)
            {
                if (evt.active.gain == 0)
                {
                    if (g_nGameMode == APPS_GAME)
                    {
                        a::ark()->SetPause();
                    }
                    Mix_PauseMusic();
                    //printf("Arkanoid: Space Ball inactive\n");
                    g_bActive = false;
                }
            }
            else
            {
                if (evt.active.gain)
                {
                    Mix_ResumeMusic();
                    //printf("Arkanoid: Space Ball active\n");
                    g_bActive = true;
                }
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            switch (evt.button.button)
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
            switch (evt.button.button)
            {
            case 1:
                g_bMouseLB = false;
                break;
            case 3:
                g_bMouseRB = false;
                break;
            case 4:
                if (g_nGameMode == APPS_GAME)
                {
                    a::bonus()->SetPosStack(false);
                }
                break;
            case 5:
                if (g_nGameMode == APPS_GAME)
                {
                    a::bonus()->SetPosStack(true);
                }
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
            if (
                ((evt.key.keysym.mod & (KMOD_RMETA | KMOD_LMETA)) && evt.key.keysym.sym == SDLK_q)
                || ((evt.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && (evt.key.keysym.sym == SDLK_x || evt.key.keysym.sym == SDLK_F4)))
            {
                // Alt+X or Command+Q pressed - quit game
                if (g_nGameMode != APPS_INTRO)
                {
                    return true;
                }
            }
            else if ((evt.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && evt.key.keysym.sym == SDLK_RETURN)
            {
                a::opt().fullscreen = !a::opt().fullscreen;
                switchFullscreen();
            }
            break;

        case SDL_JOYBUTTONDOWN: // Handle Joystick Button Presses
            if (evt.jbutton.button == 0)
            {
                g_bMouseLB = true;
            }
            if (evt.jbutton.button == 1)
            {
                g_bMouseRB = true;
            }
            if (evt.jbutton.button == 2)
            {
                if (g_nGameMode == APPS_GAME)
                {
                    a::bonus()->SetPosStack(false);
                }
            }
            else if (evt.jbutton.button == 3)
            {
                if (g_nGameMode == APPS_GAME)
                {
                    a::bonus()->SetPosStack(true);
                }
            }
            if (evt.jbutton.button == 4)
            {
                if (g_nGameMode == APPS_GAME)
                {
                    a::ark()->SendEsc();
                }
                if (g_nGameMode == APPS_MAINMENU)
                {
                    a::menu()->SendEsc();
                }
            }
            break;
        case SDL_JOYBUTTONUP:
            if (evt.jbutton.button == 0)
            {
                g_bMouseLB = false;
            }
            if (evt.jbutton.button == 1)
            {
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

    g_nCursorX = std::max(g_nCursorX, 0);
    g_nCursorX = std::min(g_nCursorX, SCREEN_WIDTH);
    g_nCursorY = std::max(g_nCursorY, 0);
    g_nCursorY = std::min(g_nCursorY, SCREEN_HEIGHT);

    return false;
}

void musicFinished()
{
    PlayMusic(false);
}

void PlayMusic(bool bFromFirst)
{
    if (g_bIsAudioSupported == false)
    {
        return;
    }
    static int nModule = 0;
    if (bFromFirst == true)
    {
        nModule = 0;
    }
    Mix_HookMusicFinished(musicFinished);
    Mix_PlayMusic(g_apMod[nModule++], 0);
    nModule %= (sizeof(g_apMod) / sizeof(Mix_Music*) - 1);
    SetVolumeMusic(a::opt().musicVolume);
}

// menu music
void musicFinished2()
{
    PlayMusic2();
}

void PlayMusic2()
{
    if (g_bIsAudioSupported == false)
    {
        return;
    }
    Mix_HookMusicFinished(musicFinished2);
    Mix_PlayMusic(g_apMod[sizeof(g_apMod) / sizeof(Mix_Music*) - 1], 0);
    SetVolumeMusic(a::opt().musicVolume);
}

int PlaySound(int nSndIndex, int nLoopsCount)
{
    if (g_bIsAudioSupported == false)
    {
        return -1;
    }
    if (nSndIndex > (int)(sizeof(g_pachSnd) / sizeof(const char*)))
    {
        return -1;
    }
    return Mix_PlayChannel(-1, g_apSnd[nSndIndex], nLoopsCount);
}

void StopSound(int& nChannel)
{
    if (g_bIsAudioSupported == false)
    {
        return;
    }
    if (nChannel != -1)
    {
        Mix_HaltChannel(nChannel);
        nChannel = -1;
    }
}

void SetVolumeMusic(int nVolume)
{
    if (g_bIsAudioSupported)
    {
        a::opt().musicVolume = std::min(10, std::max(0, nVolume));
        printf("Music volume %d\n", a::opt().musicVolume);
        const float coeff = MIX_MAX_VOLUME / 10.0f;
        Mix_VolumeMusic(a::opt().musicVolume * coeff);
    }
}

void SetVolumeSound(int nVolume)
{
    if (g_bIsAudioSupported)
    {
        a::opt().soundVolume = std::min(10, std::max(0, nVolume));
        printf("Sound volume %d\n", a::opt().soundVolume);
        const float coeff = MIX_MAX_VOLUME / 10.0f;
        Mix_Volume(-1, a::opt().soundVolume * coeff);
    }
}

bool DrawIntro()
{
    static int nRotates = 0;
    static int nFrame = 0;
    static SDL_Surface* intro = nullptr;
    static SDL_Surface* intro2 = nullptr;
    static SDL_Surface* sdlLogo = nullptr;
    if (intro == nullptr)
    {
        intro = a::res()->loadImage("intro.png");
        intro2 = a::res()->loadImage("intro2.png");
        sdlLogo = a::res()->loadImage("sdl_minibox.png");
    }

    clear(0);
    render((SCREEN_WIDTH - 80) / 2, (SCREEN_HEIGHT - 106) / 2, intro2, 0);
    SDL_Rect rc;
    rc.w = 64;
    rc.h = 64;
    rc.x = (nFrame % 4) * 64;
    rc.y = (nFrame / 4) * 64;
    render((SCREEN_WIDTH - 64) / 2, (SCREEN_HEIGHT - 106) / 2 + 8, intro, &rc);
    render(SCREEN_WIDTH - 88 - 5, SCREEN_HEIGHT - 44 - 5, sdlLogo, 0);

    static Uint32 dwTime = 0;
    if (IsKeyPressed(SDLK_ESCAPE) || g_bMouseRB || g_bMouseLB)
    {
        g_bMouseRB = false;
        g_bMouseLB = false;
        nFrame = 14;
        nRotates = 100;
        dwTime = 0;
    }
    if (dwTime + 60 < SDL_GetTicks())
    {
        dwTime = SDL_GetTicks();
        nFrame++;
        nFrame %= 16;
        if (nFrame == 15)
        {
            nRotates++;
            if (nRotates > 10)
            {
                EnableCursor(true);
                SDL_FreeSurface(sdlLogo);
                SDL_FreeSurface(intro2);
                SDL_FreeSurface(intro);
                intro = nullptr;
                return true;
            }
        }
    }

    return false;
}

void EnableCursor(bool enable)
{
    static bool isEnabled = false;
    if (isEnabled != enable)
    {
        SDL_WM_GrabInput(enable ? SDL_GRAB_OFF : SDL_GRAB_ON);
    }
    isEnabled = enable;
    g_bIsCursorVisible = enable;
}

void gameLoop()
{
#if defined(EMSCRIPTEN)
    UpdateKeys();
#else
    while (UpdateKeys() == false && g_nGameMode != APPS_EXIT)
#endif
    {
        beginFrame();

        if (g_bActive == true)
        {
            // volume manager
            if (g_modState & KMOD_SHIFT)
            {
                if (IsKeyPressed(SDLK_MINUS) && IsKeyStateChanged(SDLK_MINUS))
                {
                    SetVolumeMusic(--a::opt().musicVolume);
                }
                if (IsKeyPressed(SDLK_EQUALS) && IsKeyStateChanged(SDLK_EQUALS))
                {
                    SetVolumeMusic(++a::opt().musicVolume);
                }
            }
            else
            {
                if (IsKeyPressed(SDLK_MINUS) && IsKeyStateChanged(SDLK_MINUS))
                {
                    SetVolumeSound(--a::opt().soundVolume);
                }
                if (IsKeyPressed(SDLK_EQUALS) && IsKeyStateChanged(SDLK_EQUALS))
                {
                    SetVolumeSound(++a::opt().soundVolume);
                }
            }
            if (g_nGameMode == APPS_MAINMENU)
            {
                switch (a::menu()->DrawMenu())
                {
                case 0:
                    g_nGameMode = APPS_EXIT;
                    break;
                case 1:
                    a::tutDlg()->Reset();
                    a::ark()->InitNewGame(false);
                    g_nGameMode = APPS_GAME;
                    PlayMusic(true);
                    break;
                case 2:
                    a::tutDlg()->Reset();
                    a::ark()->RestoreGame();
                    g_nGameMode = APPS_GAME;
                    PlayMusic(true);
                    break;
                case 3:
                    // TODO check for valid custom levels

                    a::tutDlg()->Reset();
                    a::ark()->InitNewGame(true);
                    g_nGameMode = APPS_GAME;
                    PlayMusic(true);
                    break;
                case 4: // enter to level editor mode
                    m_LevelEditor.Load();
                    g_nGameMode = APPS_EDITOR;
                    break;
                }
            }
            else if (g_nGameMode == APPS_GAME)
            {
                if (a::ark()->DrawScreen() == true)
                {
                    PlayMusic2();
                    g_nGameMode = APPS_SHOULDGETNAME;
                }
            }
            else if (g_nGameMode == APPS_SHOULDGETNAME)
            {
                auto& h = a::high();
                h.lastScore = a::ark()->getScore();
                h.lastLevel = a::ark()->getLevel();
                if (h.lastScore > h.entries[9].score)
                {
                    a::menu()->SetEnterNameMode();
                }
                else
                {
                    a::menu()->SetMenuType(CMainMenu::MT_MAIN);
                }
                g_nGameMode = APPS_MAINMENU;
            }
            else if (g_nGameMode == APPS_INTRO)
            {
                if (true == DrawIntro())
                {
                    g_nGameMode = APPS_MAINMENU;
                }
            }
            else if (g_nGameMode == APPS_EDITOR)
            {
                m_LevelEditor.Draw();
                if ((true == a::menu()->DrawMenuButton(BRICKS_WIDTH * BRICK_W + WALL_X1 + 200, WALL_Y2 - 30, CMainMenu::B_OK) && g_bMouseLB == true) || IsKeyPressed(SDLK_ESCAPE))
                {
                    g_bMouseLB = false;
                    m_LevelEditor.Save();
                    g_nGameMode = APPS_MAINMENU;
                }
            }

            /*          // save screenshot to profile dir
                        if((g_modState & KMOD_CTRL) && IsKeyPressed(SDLK_s) && IsKeyStateChanged(SDLK_s)) {
                            char    achPath[MAX_PATH];
                            time_t  osTime  = time(0);
                            struct tm   local   = *localtime(&osTime);
                            sprintf(achPath, "%sarkanoidsb-%4d%2d%2d-%2d%2d.bmp", g_achUserProfile, local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min);
                            SDL_SaveBMP(g_psurfScreen, achPath);
                        }
            */
            if (IsKeyPressed(SDLK_f) && IsKeyStateChanged(SDLK_f))
            {
                a::opt().showFps = !a::opt().showFps;
                //printf("show fps %s\n", a::opt().showFps == true ? "on" : "off");
            }

            if (g_bIsCursorVisible == true)
            {
                render(g_nCursorX - 8, g_nCursorY, eImage::Cursor);
            }
            // if (a::opt().showFps == true)
            {
                a::fnt1()->DrawNumber(getFps(), 5, 5, CMyString::eAlign::Right);
            }

            endFrame();
        }
    }
}
