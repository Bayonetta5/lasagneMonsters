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

#include "radar.h"

static void initStageMap(void);
static void logic(void);
static void draw(void);
static void drawRadar(void);
static void drawEntities(void);

static void (*returnFromRadar)(void);
static void (*oldDraw)(void);
static SDL_Rect areaRect;
static SDL_Rect offset;

void initRadar(void (*done)(void))
{
	initStageMap();

	returnFromRadar = done;

	oldDraw = app.delegate.draw;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (isControl(CONTROL_MAP))
	{
		clearControl(CONTROL_MAP);

		returnFromRadar();
	}

	if (isControl(CONTROL_USE))
	{
		playSound(SND_TIP, 0);

		app.delegate.draw = oldDraw;

		clearControl(CONTROL_USE);

		initZoneMap(returnFromRadar);
	}
}

static void draw(void)
{
	oldDraw();

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 64, 0, 192);

	drawText(SCREEN_WIDTH / 2, 8, 96, TEXT_CENTER, app.colors.white, "Zone %03d", stage->id);

	drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 64, 32, TEXT_CENTER, app.colors.white, "[USE] View Area Map");

	drawRadar();

	drawEntities();
}

static void drawRadar(void)
{
	int x, y, mx, my, n, ox, oy;

	ox = (SCREEN_WIDTH - (offset.x * CELL_SIZE)) / 2;
	oy = (SCREEN_HEIGHT - (offset.y * CELL_SIZE)) / 2;

	y = 0;

	drawRect(ox, oy, (areaRect.w - areaRect.x) * CELL_SIZE, (areaRect.h - areaRect.y) * CELL_SIZE, 0, 0, 0, 96);

	for (my = areaRect.y ; my < areaRect.h ; my++)
	{
		x = 0;

		for (mx = areaRect.x ; mx < areaRect.w ; mx++)
		{
			n = getTileAt(mx, my);

			if (n >= TILE_WALL && n < TILE_FOREGROUND)
			{
				drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 0, 192, 0, 255);
			}

			x++;
		}

		y++;
	}
}

static void drawEntities(void)
{
	Entity *e;
	int x, y, ox, oy;

	ox = (SCREEN_WIDTH - (offset.x * CELL_SIZE)) / 2;
	oy = (SCREEN_HEIGHT - (offset.y * CELL_SIZE)) / 2;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		x = ((e->x + (e->w / 2)) / TILE_SIZE) - areaRect.x;
		y = ((e->y + (e->h / 2)) / TILE_SIZE) - areaRect.y;

		switch (e->type)
		{
			case ET_PLAYER:
				if (SDL_GetTicks() % 1000 < 500)
				{
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 255, 255, 255, 255);
				}
				break;

			default:
				break;
		}
	}
}

static void initStageMap(void)
{
	areaRect.x = (world.camera.x + (SCREEN_WIDTH / 2)) / TILE_SIZE;
	areaRect.x -= 32;
	areaRect.x = MAX(areaRect.x, stage->bounds.x / TILE_SIZE);

	areaRect.y = (world.camera.y + (SCREEN_HEIGHT / 2)) / TILE_SIZE;
	areaRect.y -= 16;
	areaRect.y = MAX(areaRect.y, stage->bounds.y / TILE_SIZE);

	areaRect.w = areaRect.x + 64;
	areaRect.h = areaRect.y + 32;

	areaRect.w = MIN(areaRect.w, stage->bounds.w / TILE_SIZE);
	areaRect.h = MIN(areaRect.h, stage->bounds.h / TILE_SIZE);

	offset.x = (areaRect.w - areaRect.x);
	offset.y = (areaRect.h - areaRect.y);
}
