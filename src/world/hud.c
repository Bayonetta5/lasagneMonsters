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

static void doGameText(void);
static void drawHealth(void);
static void drawAmmo(void);
static void drawCoins(void);
static void drawMonsterInfo(void);
static void drawKeys(void);
static void drawGameText(void);

static AtlasImage *heartFullTexture;
static AtlasImage *heartEmptyTexture;
static AtlasImage *coinTexture;
static AtlasImage *waterTexture;
static AtlasImage *monstersTexture;
static AtlasImage *keyTexture;

void initHud(void)
{
	heartFullTexture = getAtlasImage("gfx/hud/heartFull.png", 1);
	heartEmptyTexture = getAtlasImage("gfx/hud/heartEmpty.png", 1);
	coinTexture = getAtlasImage("gfx/entities/coin.png", 1);
	waterTexture = getAtlasImage("gfx/hud/water.png", 1);
	monstersTexture = getAtlasImage("gfx/hud/monsters.png", 1);
	keyTexture = getAtlasImage("gfx/hud/key.png", 1);
}

void addGameText(int x, int y, char *format, ...)
{
	GameText *g;
	char buffer[MAX_DESCRIPTION_LENGTH];
	va_list args;
	
	memset(&buffer, '\0', sizeof(buffer));

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	g = malloc(sizeof(GameText));
	memset(g, 0, sizeof(GameText));
	stage.gameTextTail->next = g;
	stage.gameTextTail = g;
	
	g->x = x;
	g->y = y;
	STRNCPY(g->text, buffer, MAX_NAME_LENGTH);
	g->health = FPS;
}

void doHud(void)
{
	doGameText();
}

static void doGameText(void)
{
	GameText *g, *prev;
	
	prev = &stage.gameTextHead;
	
	for (g = stage.gameTextHead.next ; g != NULL ; g = g->next)
	{
		g->y--;
		
		if (--g->health <= 0)
		{
			if (g == stage.gameTextTail)
			{
				stage.gameTextTail = prev;
			}
			
			prev->next = g->next;
			free(g);
			g = prev;
		}
		
		prev = g;
	}
}

void drawHud(void)
{
	drawHealth();
	
	drawAmmo();
	
	drawMonsterInfo();
	
	drawCoins();
	
	drawKeys();
	
	drawGameText();
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
	int i, width, maxWidth, x;
	
	blitAtlasImage(waterTexture, 10, 48, 0, SDL_FLIP_NONE);
	
	w = (Walter*)stage.player->data;
	
	maxWidth = MAX(w->maxAmmo * 25, 0);
	width = MAX(w->ammo * 25, 0);
	
	drawRect(42, 54, maxWidth, 16, 0, 64, 128, 192);
	drawRect(42, 54, width, 16, 0, 200, 255, 255);
	drawOutlineRect(42, 54, maxWidth, 16, 0, 0, 0, 255);
	
	x = 42;
	
	for (i = 0 ; i < w->maxAmmo ; i++)
	{
		drawLine(x, 54, x, 70, 0, 0, 0, 255);
		
		x += 25;
	}
}

static void drawMonsterInfo(void)
{
	blitAtlasImage(monstersTexture, SCREEN_WIDTH - 112, 8, 0, SDL_FLIP_NONE);
	
	drawText(SCREEN_WIDTH - 10, 6, 32, TEXT_RIGHT, app.colors.white, "%d / %d", stage.totalMonsters - stage.numMonsters, stage.totalMonsters);
}

static void drawCoins(void)
{
	blitAtlasImage(coinTexture, SCREEN_WIDTH - 109, 53, 0, SDL_FLIP_NONE);
	
	drawText(SCREEN_WIDTH - 10, 48, 32, TEXT_RIGHT, app.colors.white, "x %03d", game.coins);
}

static void drawKeys(void)
{
	blitAtlasImage(keyTexture, SCREEN_WIDTH - 90, 96, 0, SDL_FLIP_NONE);
	
	drawText(SCREEN_WIDTH - 10, 90, 32, TEXT_RIGHT, app.colors.white, "x %d", stage.keys);
}

static void drawGameText(void)
{
	GameText *g;
	int x, y;
	
	for (g = stage.gameTextHead.next ; g != NULL ; g = g->next)
	{
		x = g->x - stage.camera.x;
		y = g->y - stage.camera.y;
		
		drawText(x, y, 32, TEXT_CENTER, app.colors.white, g->text);
	}
}
