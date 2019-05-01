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
static void drawAmmo(void);
static void drawCoins(void);
static void drawMonsterInfo(void);

static AtlasImage *heartFullTexture;
static AtlasImage *heartEmptyTexture;
static AtlasImage *coinTexture;
static AtlasImage *waterTexture;
static AtlasImage *monstersTexture;

void initHud(void)
{
	heartFullTexture = getAtlasImage("gfx/hud/heartFull.png", 1);
	heartEmptyTexture = getAtlasImage("gfx/hud/heartEmpty.png", 1);
	coinTexture = getAtlasImage("gfx/entities/coin.png", 1);
	waterTexture = getAtlasImage("gfx/hud/water.png", 1);
	monstersTexture = getAtlasImage("gfx/hud/monsters.png", 1);
}

void drawHud(void)
{
	drawRect(0, 0, SCREEN_WIDTH, 40, 0, 0, 0, 128);
	
	drawHealth();
	
	drawAmmo();
	
	drawCoins();
	
	drawMonsterInfo();
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
			blitAtlasImage(heartFullTexture, x, 8, 0, SDL_FLIP_NONE);
		}
		else
		{
			blitAtlasImage(heartEmptyTexture, x, 8, 0, SDL_FLIP_NONE);
		}
		
		x += 32;
	}
}

static void drawAmmo(void)
{
	Walter *w;
	int width, maxWidth;
	
	blitAtlasImage(waterTexture, 400, 8, 0, SDL_FLIP_NONE);
	
	w = (Walter*)stage.player->data;
	
	maxWidth = MAX(w->maxAmmo * 25, 0);
	width = MAX(w->ammo * 25, 0);
	
	drawRect(432, 12, maxWidth, 16, 0, 200, 255, 128);
	drawRect(432, 12, width, 16, 0, 200, 255, 255);
	drawOutlineRect(432, 12, maxWidth, 16, 0, 0, 0, 255);
}

static void drawCoins(void)
{
	blitAtlasImage(coinTexture, 800, 8, 0, SDL_FLIP_NONE);
	
	drawText(832, 6, 32, TEXT_LEFT, app.colors.white, "x %03d", game.coins);
}
	
static void drawMonsterInfo(void)
{
	blitAtlasImage(monstersTexture, 1100, 4, 0, SDL_FLIP_NONE);
	
	drawText(1152, 6, 32, TEXT_LEFT, app.colors.white, "%d / %d", stage.totalMonsters - stage.numMonsters, stage.totalMonsters);
}
