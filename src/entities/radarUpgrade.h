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
#include "../json/cJSON.h"

#define NUM_TEXTURES 8

extern void addGameText(int x, int y, char *format, ...);
extern void addPowerupParticles(int x, int y);
extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void drawObjectGlow(int r, int g, int b, int a);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern void itemHop(void);
extern void playSound(int snd, int ch);

extern Entity *self;
extern World world;
