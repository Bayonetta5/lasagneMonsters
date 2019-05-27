/*
Copyright (C) 2019 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../common.h"

#define MAX_BODY_PARTS 10

extern void addCoinParticles(int x, int y);
extern void addGameText(int x, int y, char *format, ...);
extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void drawObjectGlow(int r, int g, int b, int a);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern void playPositionalSound(int snd, int ch, int srcX, int srcY, int destX, int destY);
extern Entity *spawnEntity(void);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern int getDistance(int x1, int y1, int x2, int y2);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void throwCoins(int x, int y, int n);

extern App app;
extern Entity *self;
extern Game game;
extern World world;
