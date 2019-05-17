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

static void drawTopBar(void);
static void drawBottomBar(void);
static void doGameText(void);
static void drawHealth(void);
static void drawAmmo(void);
static void drawCoins(void);
static void drawMonsterInfo(void);
static void drawGirlsInfo(void);
static void drawKeys(void);
static void drawTimeLimit(void);
static void drawZoneInfo(void);
static void drawGameText(void);

static AtlasImage *heartFullTexture;
static AtlasImage *heartEmptyTexture;
static AtlasImage *coinTexture;
static AtlasImage *waterTexture;
static AtlasImage *monstersTexture;
static AtlasImage *girlsTexture;
static AtlasImage *keyTexture;
static AtlasImage *hourGlassTexture;

void initHud(void)
{
	heartFullTexture = getAtlasImage("gfx/hud/heartFull.png", 1);
	heartEmptyTexture = getAtlasImage("gfx/hud/heartEmpty.png", 1);
	coinTexture = getAtlasImage("gfx/entities/coin.png", 1);
	waterTexture = getAtlasImage("gfx/hud/water.png", 1);
	monstersTexture = getAtlasImage("gfx/hud/monsters.png", 1);
	girlsTexture = getAtlasImage("gfx/hud/girls.png", 1);
	keyTexture = getAtlasImage("gfx/hud/key.png", 1);
	hourGlassTexture = getAtlasImage("gfx/hud/hourGlass.png", 1);
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
	world.gameTextTail->next = g;
	world.gameTextTail = g;

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

	prev = &world.gameTextHead;

	for (g = world.gameTextHead.next ; g != NULL ; g = g->next)
	{
		g->y--;

		if (--g->health <= 0)
		{
			if (g == world.gameTextTail)
			{
				world.gameTextTail = prev;
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
	drawTopBar();

	drawBottomBar();

	drawGameText();
}

static void drawTopBar(void)
{
	drawRect(0, 0, SCREEN_WIDTH, 40, 0, 0, 0, 192);

	drawHealth();

	drawAmmo();

	drawZoneInfo();
}

static void drawHealth(void)
{
	int i, x;
	Walter *w;

	w = (Walter*)world.player->data;

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

	blitAtlasImage(waterTexture, 510, 8, 0, SDL_FLIP_NONE);

	w = (Walter*)world.player->data;

	maxWidth = MAX(w->maxAmmo * 25, 0);
	width = MAX(w->ammo * 25, 0);

	drawRect(542, 14, maxWidth, 16, 0, 64, 128, 192);
	drawRect(542, 14, width, 16, 0, 200, 255, 255);
	drawOutlineRect(542, 14, maxWidth, 16, 0, 0, 0, 255);
}

static void drawZoneInfo(void)
{
	drawText(SCREEN_WIDTH - 10, 5, 32, TEXT_RIGHT, app.colors.white, "Zone %03d", stage->id);
}

static void drawBottomBar(void)
{
	drawRect(0, SCREEN_HEIGHT - 32, SCREEN_WIDTH, 32, 0, 0, 0, 192);

	drawMonsterInfo();

	drawCoins();

	drawKeys();

	drawGirlsInfo();

	drawTimeLimit();
}

static void drawMonsterInfo(void)
{
	blitAtlasImage(monstersTexture, 10, SCREEN_HEIGHT - 28, 0, SDL_FLIP_NONE);

	drawText(10 + monstersTexture->rect.w + 16, SCREEN_HEIGHT - 32, 32, TEXT_LEFT, app.colors.white, "x %03d", stage->numMonsters);
}

static void drawCoins(void)
{
	blitAtlasImage(coinTexture, 200, SCREEN_HEIGHT - 28, 0, SDL_FLIP_NONE);

	drawText(200 + coinTexture->rect.w + 16, SCREEN_HEIGHT - 32, 32, TEXT_LEFT, app.colors.white, "x %03d", game.coins);
}

static void drawKeys(void)
{
	blitAtlasImage(keyTexture, 400, SCREEN_HEIGHT - 26, 0, SDL_FLIP_NONE);

	drawText(400 + keyTexture->rect.w + 16, SCREEN_HEIGHT - 32, 32, TEXT_LEFT, app.colors.white, "x %03d", game.keys);
}

static void drawGirlsInfo(void)
{
	blitAtlasImage(girlsTexture, 600, SCREEN_HEIGHT - 28, 0, SDL_FLIP_NONE);

	drawText(600 + girlsTexture->rect.w + 16, SCREEN_HEIGHT - 32, 32, TEXT_LEFT, app.colors.white, "x %d", stage->numGirls);
}

static void drawTimeLimit(void)
{
	int t, h, m, s;

	t = game.time / FPS;

	h = t / (60 * 60);
	m = (t / 60) % 60;
	s = t % 60;

	blitAtlasImage(hourGlassTexture, 800, SCREEN_HEIGHT - 28, 0, SDL_FLIP_NONE);

	drawText(800 + hourGlassTexture->rect.w + 16, SCREEN_HEIGHT - 32, 32, TEXT_LEFT, app.colors.white, "%02d:%02d:%02d", h, m, s);
}

static void drawGameText(void)
{
	GameText *g;
	int x, y;

	for (g = world.gameTextHead.next ; g != NULL ; g = g->next)
	{
		x = g->x - world.camera.x;
		y = g->y - world.camera.y;

		drawText(x, y, 32, TEXT_CENTER, app.colors.white, g->text);
	}
}
