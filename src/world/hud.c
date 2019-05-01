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

#include "hud.h"

static void drawHealth(void);

static AtlasImage *heartFull;
static AtlasImage *heartEmpty;

void initHud(void)
{
	heartFull = getAtlasImage("gfx/hud/heartFull.png", 1);
	heartEmpty = getAtlasImage("gfx/hud/heartEmpty.png", 1);
}

void drawHud(void)
{
	drawRect(0, 0, SCREEN_WIDTH, 40, 0, 0, 0, 128);
	
	drawHealth();
}

static void drawHealth(void)
{
	int i, x;
	Walter *w;
	
	w = (Walter*)stage.player->data;
	
	x = 10;
	
	for (i = 0 ; i < w->maxHealth ; i++)
	{
		if (w->health > i)
		{
			blitAtlasImage(heartFull, x, 8, 0, SDL_FLIP_NONE);
		}
		else
		{
			blitAtlasImage(heartEmpty, x, 8, 0, SDL_FLIP_NONE);
		}
		
		x += 32;
	}
}
