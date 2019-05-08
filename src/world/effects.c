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

#include "effects.h"

static AtlasImage *pusBallTexture[3];
static AtlasImage *lightTexture;

void initEffects(void)
{
	pusBallTexture[0] = getAtlasImage("gfx/entities/pusBall1.png", 1);
	pusBallTexture[1] = getAtlasImage("gfx/entities/pusBall2.png", 1);
	pusBallTexture[2] = getAtlasImage("gfx/entities/pusBall3.png", 1);

	lightTexture = getAtlasImage("gfx/particles/light.png", 1);
}

void drawObjectGlow(int r, int g, int b, int a)
{
	int x, y;

	x = self->x + (self->w / 2) - world.camera.x;
	y = self->y + (self->h / 2) - world.camera.y;

	SDL_SetTextureColorMod(lightTexture->texture, r, g, b);
	SDL_SetTextureAlphaMod(lightTexture->texture, a);

	blitAtlasImage(lightTexture, x, y, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(lightTexture->texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(lightTexture->texture, 255);
}

void throwPusBalls(int x, int y, int n)
{
	Entity *e;
	int i;

	for (i = 0 ; i < n ; i++)
	{
		e = spawnEntity();

		initPusBall(e);

		e->x = x;
		e->y = y;
		e->dx = (rand() % 500) - (rand() % 500);
		e->dx /= 100;

		e->dy = -(1000 + rand() % 1000);
		e->dy /= 100;

		e->atlasImage = pusBallTexture[rand() % 3];
	}
}
