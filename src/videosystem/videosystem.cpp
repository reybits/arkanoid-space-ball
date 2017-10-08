/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "videosystem.h"
#include "../accessor.h"
#include "../defines.h"
#include "../game.h"
#include "../mystring.h"

#include "../glSDL.h"

#include <cstdlib>

namespace
{
    SDL_Surface* ScreenSurface = nullptr;

    int frame = 0;
    int fps = 0;
    unsigned fpsTime = 0;
    unsigned lastTime = 0;
    // g_fSpeedCorrection  = 1.0f;
    // const float fPerFrameMs = 1000.0f / DESIRED_FPS;
}

void setVideoMode()
{
    SDL_WM_SetCaption("Arkanoid: Space Ball", 0);

    char achBuf[50];
    Uint32 dwFlags = (a::opt().opengl ? SDL_GLSDL : 0) | SDL_HWSURFACE | SDL_DOUBLEBUF | (a::opt().fullscreen == true ? SDL_FULLSCREEN : 0);
    printf("Requested video flags:\n");
    printf("  glSDL: %s\n", dwFlags & SDL_GLSDL ? "yes" : "no");
    printf("  Fullscreen: %s\n", dwFlags & SDL_FULLSCREEN ? "yes" : "no");

    const uint32_t bppCount = sizeof(g_bppList) / sizeof(g_bppList[0]);
    for (uint32_t i = 0; i < bppCount; i++)
    {
        const auto bpp = g_bppList[a::opt().bppIdx];
        ScreenSurface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, bpp, dwFlags);
        if (ScreenSurface != nullptr)
        {
            break;
        }

        printf("Couldn't set %d x %d x %d bpx video mode: %s.\n", SCREEN_WIDTH, SCREEN_HEIGHT, bpp, SDL_GetError());
        a::opt().bppIdx = (a::opt().bppIdx + 1) % bppCount;
    }

    //ScreenSurface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, (a::opt().opengl == false ? SDL_GLSDL : 0) | SDL_DOUBLEBUF | (a::opt().fullscreen == true ? SDL_FULLSCREEN : 0));
    if (ScreenSurface == nullptr)
    {
        exit(1);
    }
    printf("Video mode %d x %d x %d bpx created.\n", ScreenSurface->w, ScreenSurface->h, ScreenSurface->format->BitsPerPixel);

    // get driver name and hardware info
    const SDL_VideoInfo* vi = SDL_GetVideoInfo();
    printf("  driver name: %s\n", SDL_VideoDriverName(achBuf, sizeof(achBuf)));
    printf("  hardware surfaces: %s\n", !vi->hw_available ? "no" : "yes");
    printf("  window manager available: %s\n", !vi->wm_available ? "no" : "yes");
    printf("  hardware to hardware blits accelerated: %s\n", !vi->blit_hw ? "no" : "yes");
    printf("  hardware to hardware colorkey blits accelerated: %s\n", !vi->blit_hw_CC ? "no" : "yes");
    printf("  hardware to hardware alpha blits: %s\n", !vi->blit_hw_A ? "no" : "yes");
    printf("  software to hardware blits: %s\n", !vi->blit_sw ? "no" : "yes");
    printf("  software to hardware colorkey blits: %s\n", !vi->blit_sw_CC ? "no" : "yes");
    printf("  software to hardware alpha blits: %s\n", !vi->blit_sw_A ? "no" : "yes");
    printf("  color fills accelerated: %s\n", !vi->blit_fill ? "no" : "yes");
    printf("  total video memory: %d Kb\n", vi->video_mem);

    SDL_ShowCursor(SDL_DISABLE);

    a::fnt1()->loadFont("font_small.png", 5, -1, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
    a::fnt2()->loadFont("font_big.png", 16, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?-%,._:");
    a::fnt3()->loadFont("font_small2.png", 5, -1, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_abcdefghijklmnopqrstuvwxyz{|}~");
    a::fntTut()->loadFont("font_tutorial.png", 5, 1, nullptr);

    a::ark()->LoadBackground();
    a::res()->preloadImages();
}

void unsetVideoMode()
{
    a::res()->purgeImages();
    a::ark()->FreeBackground();

    a::fntTut()->Unload();
    a::fnt3()->Unload();
    a::fnt2()->Unload();
    a::fnt1()->Unload();
}

void switchFullscreen()
{
    //SDL_WM_ToggleFullScreen(ScreenSurface); // currently works only in linux
    unsetVideoMode();
    setVideoMode();
}

void beginFrame()
{
    auto currentTime = SDL_GetTicks();

    frame++;
    const auto delta = currentTime - fpsTime;
    if (delta >= 1000)
    {
        fps = (frame * 1000.0) / delta;
        frame = 0;
        fpsTime = currentTime;
    }

    g_fSpeedCorrection = (float)(currentTime - lastTime) / (DESIRED_FPS / 10.0f);
    lastTime = currentTime;
}

void endFrame()
{
    SDL_Flip(ScreenSurface);
}

int getFps()
{
    return fps;
}

void clear(unsigned color)
{
    SDL_FillRect(ScreenSurface, nullptr, color);
}

void scissor(SDL_Rect* rc)
{
    SDL_SetClipRect(ScreenSurface, rc);
}

void dimScreen()
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            render(x * 160, y * 120, eImage::Transp);
        }
    }
}

void render(int x, int y, SDL_Surface* img, SDL_Rect* src)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    SDL_BlitSurface(img, src, ScreenSurface, &dst);
}

void render(int x, int y, eImage id, SDL_Rect* src)
{
    auto img = a::res()->getImage(id);
    render(x, y, img, src);
}
