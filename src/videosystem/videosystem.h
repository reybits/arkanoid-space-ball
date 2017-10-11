/**********************************************\
*
*  Copyright (C) 2006 by Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#pragma once

#include "../resource.h"

struct SDL_Rect;

void setVideoMode();
void unsetVideoMode();
void switchFullscreen();

void beginFrame();
void endFrame();

int getFps();

void clear(unsigned r, unsigned g, unsigned b, unsigned a);

void scissor(SDL_Rect* rc = nullptr);
void dimScreen();

void render(int x, int y, SDL_Surface* img, SDL_Rect* src = nullptr);
void render(int x, int y, eImage id, SDL_Rect* src = nullptr);
