/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "arkanoidsb.h"
#include "accessor.h"
#include "assets.h"
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

#include <cassert>
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

bool g_quitRequested = false;
bool g_bActive = true;

Uint8* g_keysState = nullptr;
std::vector<Uint8> g_keysStateLast;
Uint32 g_modState = 0;

bool g_bMouseRB = false;
bool g_bMouseLB = false;
bool g_bIsCursorVisible = false;
sVector<float> g_cursorPosition{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

int g_nGameMode = APPS_INTRO;

CLevelEditor m_LevelEditor;

bool g_bIsAudioSupported = false;

const char* SoundNames[] = {
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
std::vector<Mix_Chunk*> g_soundList;

const char* MusicNames[] = {
    "module01.ogg",
    "module02.ogg",
    "module03.s3m",
};
struct MusicProperty
{
    Mix_Music* music;
    unsigned char* data;
};
std::vector<MusicProperty> g_musicList;

int main(int /*argc*/, char** /*argv*/)
{
    printf("%s by Andrey A. Ugolnik. Copyright (c) 2006.\n", APP_Title);
    printf("version %d.%d.%d.\n", APP_VerMajor, APP_VerMinor, APP_VerRelease);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL-video: %s\n", SDL_GetError());
        assert(0);
    }

    const int imgFlags = IMG_INIT_PNG;
    const int imgInitted = IMG_Init(imgFlags);
    if ((imgInitted & imgFlags) != imgFlags)
    {
        printf("Couldn't initialize SDL_Image: %s\n", SDL_GetError());
        assert(0);
    }

    a::initialize();
    assets::initialize();

    if (a::res()->Open(assets::makePath("arkanoidsb.pak")) == false)
    {
        assert(0);
    }

    EnableCursor(false);

#if !defined(__MACOSX__) && !defined(EMSCRIPTEN)
    unsigned size;
    unsigned char* data = a::res()->GetDataAllocMem("icon.bmp", size);
    auto icon = SDL_LoadBMP_RW(SDL_RWFromConstMem(data, size), 1);
    a::res()->FreeMem(data);
    if (icon != nullptr)
    {
        printf(" done.\n");
    }
    else
    {
        printf("  %s\n", SDL_GetError());
    }

    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 0, 0, 0));
    SDL_WM_SetIcon(icon, nullptr);
#endif

    setVideoMode();
    SDL_WarpMouse(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    for (int i = 0; i < 360; i++)
    {
        g_fCos[i] = cosf((M_PI / 180.0f) * i);
        g_fSin[i] = sinf((M_PI / 180.0f) * i);
    }

    int count;
    g_keysState = SDL_GetKeyState(&count);
    g_keysStateLast.resize(count);

    initializeAudio();
    loadAudio();

#if defined(EMSCRIPTEN)
    emscripten_set_main_loop(gameLoop, 0, 1);
#else
    while (g_quitRequested == false)
    {
        gameLoop();
    }
#endif

    deinitializeAudio();

    unsetVideoMode();

    assets::deinitialize();
    a::deinitialize();

    SDL_Quit();

    return 0;
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

void updateKeys()
{
    memcpy(g_keysStateLast.data(), g_keysState, g_keysStateLast.size());
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
            g_cursorPosition = {
                std::min<float>(evt.motion.x, SCREEN_WIDTH),
                std::min<float>(evt.motion.y, SCREEN_HEIGHT)
            };
            break;

        case SDL_KEYDOWN:
            if (
                ((evt.key.keysym.mod & (KMOD_RMETA | KMOD_LMETA)) && evt.key.keysym.sym == SDLK_q)
                || ((evt.key.keysym.mod & (KMOD_LALT | KMOD_RALT)) && (evt.key.keysym.sym == SDLK_x || evt.key.keysym.sym == SDLK_F4)))
            {
                g_quitRequested = true;
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
        //g_cursorPosition.x += nDelta;
        //g_nMouseDX += nDelta;
        //}
        //if(evt.jaxis.axis == 1) {
        //g_cursorPosition.y += nDelta;
        //g_nMouseDY += nDelta;
        //}
        //}
        //break;

        case SDL_QUIT:
            g_quitRequested = true;
            break;
        }
    }
}

void PlayMusic(bool restart)
{
    if (g_bIsAudioSupported)
    {
        static size_t Index = 0;
        if (restart == true)
        {
            Index = 0;
        }

        Mix_HookMusicFinished([]() { PlayMusic(false); });
        Mix_PlayMusic(g_musicList[Index++].music, 0);
        Index %= g_musicList.size() - 1;

        SetVolumeMusic(a::opt().musicVolume);
    }
}

void playMenuMusic()
{
    if (g_bIsAudioSupported)
    {
        Mix_HookMusicFinished([]() { playMenuMusic(); });
        Mix_PlayMusic(g_musicList.back().music, 0);

        SetVolumeMusic(a::opt().musicVolume);
    }
}

int PlaySound(int index, int loopCount)
{
    if (g_bIsAudioSupported)
    {
        if ((size_t)index < g_soundList.size())
        {
            return Mix_PlayChannel(-1, g_soundList[index], loopCount);
        }
    }
    return -1;
}

void StopSound(int& channel)
{
    if (g_bIsAudioSupported)
    {
        if (channel != -1)
        {
            Mix_HaltChannel(channel);
            channel = -1;
        }
    }
}

void SetVolumeMusic(int volume)
{
    if (g_bIsAudioSupported)
    {
        a::opt().musicVolume = std::min(10, std::max(0, volume));
        printf("Music volume %d\n", a::opt().musicVolume);
        const float coeff = MIX_MAX_VOLUME / 10.0f;
        Mix_VolumeMusic(a::opt().musicVolume * coeff);
    }
}

void SetVolumeSound(int volume)
{
    if (g_bIsAudioSupported)
    {
        a::opt().soundVolume = std::min(10, std::max(0, volume));
        printf("Sound volume %d\n", a::opt().soundVolume);
        const float coeff = MIX_MAX_VOLUME / 10.0f;
        Mix_Volume(-1, a::opt().soundVolume * coeff);
    }
}

bool DrawIntro()
{
    static Uint32 lastTime = SDL_GetTicks();
    static Uint32 idleTime = 0;

    const Uint32 currentTime = SDL_GetTicks();
    const Uint32 dt = currentTime - lastTime;
    lastTime = currentTime;
    idleTime += dt;

    clear(0, 0, 0, 255);

    static SDL_Surface* logo = nullptr;
    static SDL_Surface* sdlLogo = nullptr;
    if (logo == nullptr)
    {
        logo = a::res()->loadImage("logo.png");
        sdlLogo = a::res()->loadImage("sdl_minibox.png");
    }

    const Uint32 transitionTime = 500;
    const float ratio = 1.0f - std::min<float>(transitionTime, idleTime) / transitionTime;
    const float offset = ::sinf(M_PI_2 * ratio) * ratio * (SCREEN_HEIGHT * 0.5f);
    render((SCREEN_WIDTH - 128) / 2, (SCREEN_HEIGHT - 128) / 2 - offset, logo, 0);

    render(SCREEN_WIDTH - 88 - 5, SCREEN_HEIGHT - 44 - 5, sdlLogo, 0);

    const Uint32 maxIdleTime = 2000;
    const bool done = idleTime >= maxIdleTime || IsKeyPressed(SDLK_ESCAPE) || g_bMouseRB || g_bMouseLB;
    if (done)
    {
        g_bMouseRB = false;
        g_bMouseLB = false;

        EnableCursor(true);
        SDL_FreeSurface(sdlLogo);
        SDL_FreeSurface(logo);
        logo = nullptr;
    }

    return done;
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

void initializeAudio()
{
    g_bIsAudioSupported = false;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) >= 0)
    {
        const int flags = MIX_INIT_MOD | MIX_INIT_OGG;
        if (Mix_Init(flags) & flags)
        {
            if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) != -1)
            {
                g_bIsAudioSupported = true;
            }
            else
            {
                printf("Mix_OpenAudio: %s\n", Mix_GetError());
            }
        }
        else
        {
            printf("Mix_Init: %s\n", Mix_GetError());
        }
    }
    else
    {
        printf("SDL-audio: %s\n", SDL_GetError());
    }
}

void deinitializeAudio()
{
    if (g_bIsAudioSupported)
    {
        for (auto m : g_musicList)
        {
            Mix_FreeMusic(m.music);
            a::res()->FreeMem(m.data);
        }
        g_musicList.clear();

        for (auto s : g_soundList)
        {
            Mix_FreeChunk(s);
        }
        g_soundList.clear();

        Mix_CloseAudio();
    }
}

void loadAudio()
{
    if (g_bIsAudioSupported)
    {
        // loading sound effects
        for (auto name : SoundNames)
        {
            unsigned size;
            auto data = a::res()->GetDataAllocMem(name, size);
            if (data != nullptr)
            {
                auto rwops = SDL_RWFromConstMem(data, size);
                auto sound = Mix_LoadWAV_RW(rwops, 1);
                a::res()->FreeMem(data);

                if (sound != nullptr)
                {
                    g_soundList.push_back(sound);
                    printf(" done.\n");
                }
                else
                {
                    printf(" %s.\n", SDL_GetError());
                    assert(0);
                }
            }
            else
            {
                printf(" error.\n");
                assert(0);
            }
        }

        for (auto name : MusicNames)
        {
            unsigned size;
            auto data = a::res()->GetDataAllocMem(name, size);
            if (data != nullptr)
            {
                auto rwops = SDL_RWFromConstMem(data, size);
                auto music = Mix_LoadMUS_RW(rwops);
                if (music != nullptr)
                {
                    g_musicList.push_back({ music, data });
                    printf(" done.\n");
                }
                else
                {
                    a::res()->FreeMem(data);
                    printf(" error '%s'\n", SDL_GetError());
                    assert(0);
                }
            }
        }

        playMenuMusic();
    }
}

void gameLoop()
{
    updateKeys();
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
                g_quitRequested = true;
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
                playMenuMusic();
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

        if (IsKeyPressed(SDLK_f) && IsKeyStateChanged(SDLK_f))
        {
            a::opt().showFps = !a::opt().showFps;
            //printf("show fps %s\n", a::opt().showFps == true ? "on" : "off");
        }

#if !defined(EMSCRIPTEN)
        if (g_bIsCursorVisible == true)
        {
            render(g_cursorPosition.x - 8, g_cursorPosition.y, eImage::Cursor);
        }
#endif
        if (a::opt().showFps == true)
        {
            a::fnt1()->DrawNumber(getFps(), 5, 5, CMyString::eAlign::Right);
        }

        endFrame();
    }
}
