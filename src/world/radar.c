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

static void logic(void);
static void draw(void);
static void drawRadar(void);
static void drawEntities(void);
static void initMapView(void);
static void updateMapView(void);
static void drawArrows(void);

static void (*returnFromRadar)(void);
static void (*oldDraw)(void);
static SDL_Rect areaRect;
static SDL_Rect offset;
static AtlasImage *arrowTexture;
static int showArrow[4];
static float arrowPulse;

void initRadar(void (*done)(void))
{
	initMapView();

	arrowPulse = 0;

	arrowTexture = getAtlasImage("gfx/main/radarArrow.png", 1);

	returnFromRadar = done;

	oldDraw = app.delegate.draw;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	arrowPulse += 0.1;

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

	if (isControl(CONTROL_RIGHT))
	{
		areaRect.x++;
	}

	if (isControl(CONTROL_LEFT))
	{
		areaRect.x--;
	}

	if (isControl(CONTROL_UP))
	{
		areaRect.y--;
	}

	if (isControl(CONTROL_DOWN))
	{
		areaRect.y++;
	}

	updateMapView();
}

static void draw(void)
{
	oldDraw();

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 64, 0, 192);

	drawText(SCREEN_WIDTH / 2, 8, 96, TEXT_CENTER, app.colors.white, "Zone %03d", stage->id);

	drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 64, 32, TEXT_CENTER, app.colors.white, "[USE] View Area Map");

	drawRadar();

	drawEntities();

	drawArrows();
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
	Walter *w;
	int x, y, ox, oy;

	ox = (SCREEN_WIDTH - (offset.x * CELL_SIZE)) / 2;
	oy = (SCREEN_HEIGHT - (offset.y * CELL_SIZE)) / 2;

	w = (Walter*)world.player->data;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if ((!w->hasRadarUpgrade && e->type != ET_PLAYER) && !app.dev.debug)
		{
			continue;
		}

		x = (e->x / TILE_SIZE) - areaRect.x;
		y = (e->y / TILE_SIZE) - areaRect.y;

		if (x > 0 && y > 0 && x < RADAR_WIDTH && y < RADAR_HEIGHT)
		{
			switch (e->type)
			{
				case ET_PLAYER:
					if (SDL_GetTicks() % 1000 < 500)
					{
						drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 255, 255, 255, 255);
					}
					break;

				case ET_MONSTER:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 255, 0, 0, 255);
					break;

				case ET_BBBBB:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 255, 255, 0, 255);
					break;

				case ET_POWERUP:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 160, 160, 255, 255);
					break;

				case ET_CHEST:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 0, 255, 255, 255);
					break;

				case ET_SAVE_POINT:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 255, 0, 255, 255);
					break;

				case ET_DOOR:
					if (!((Door*)e->data)->open)
					{
						drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 160, 255, 160, 255);
						if (++y < RADAR_HEIGHT)
						{
							drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 160, 255, 160, 255);
						}
					}
					break;

				case ET_HORIZONTAL_DOOR:
					if (!((Door*)e->data)->open)
					{
						drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 160, 255, 160, 255);
						if (++x < RADAR_WIDTH)
						{
							drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 160, 255, 160, 255);
						}
					}
					break;

				case ET_PLATFORM:
					drawRect(ox + (x * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 192, 192, 192, 255);
					drawRect(ox + ((x + 1) * CELL_SIZE), oy + (y * CELL_SIZE), CELL_SIZE - 1, CELL_SIZE - 1, 192, 192, 192, 255);
					break;

				default:
					break;
			}
		}
	}
}

static void drawArrows(void)
{
	float pulse;

	pulse = sin(arrowPulse) * 10;

	if (showArrow[0])
	{
		blitAtlasImageRotated(arrowTexture, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) - 250 + pulse, 0, SDL_FLIP_NONE);
	}

	if (showArrow[1])
	{
		blitAtlasImageRotated(arrowTexture, (SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2) + 250 - pulse, 180, SDL_FLIP_NONE);
	}

	if (showArrow[2])
	{
		blitAtlasImageRotated(arrowTexture, (SCREEN_WIDTH / 2) - 550 + pulse, (SCREEN_HEIGHT / 2), -90, SDL_FLIP_NONE);
	}

	if (showArrow[3])
	{
		blitAtlasImageRotated(arrowTexture, (SCREEN_WIDTH / 2) + 550 - pulse, (SCREEN_HEIGHT / 2), 90, SDL_FLIP_NONE);
	}
}

static void initMapView(void)
{
	areaRect.x = (world.camera.x + (SCREEN_WIDTH / 2)) / TILE_SIZE;
	areaRect.x -= (RADAR_WIDTH / 2);
	areaRect.x = MAX(areaRect.x, stage->bounds.x / TILE_SIZE);

	areaRect.y = (world.camera.y + (SCREEN_HEIGHT / 2)) / TILE_SIZE;
	areaRect.y -= (RADAR_HEIGHT / 2);
	areaRect.y = MAX(areaRect.y, stage->bounds.y / TILE_SIZE);

	areaRect.w = areaRect.x + RADAR_WIDTH;
	areaRect.h = areaRect.y + RADAR_HEIGHT;

	areaRect.w = MIN(areaRect.w, stage->bounds.w / TILE_SIZE);
	areaRect.h = MIN(areaRect.h, stage->bounds.h / TILE_SIZE);

	updateMapView();

	offset.x = MIN(RADAR_WIDTH, (stage->bounds.w - stage->bounds.x) / TILE_SIZE);
	offset.y = MIN(RADAR_HEIGHT, (stage->bounds.h - stage->bounds.y) / TILE_SIZE);
}

static void updateMapView(void)
{
	SDL_Rect limits;
	int w, h;

	limits.x = (stage->bounds.x / TILE_SIZE);
	limits.y = (stage->bounds.y / TILE_SIZE);
	limits.w = (stage->bounds.w / TILE_SIZE);
	limits.h = (stage->bounds.h / TILE_SIZE);

	w = (stage->bounds.w - stage->bounds.x) / TILE_SIZE;
	h = (stage->bounds.h - stage->bounds.y) / TILE_SIZE;

	w = MIN(w, RADAR_WIDTH);
	h = MIN(h, RADAR_HEIGHT);

	areaRect.x = MIN(MAX(areaRect.x, limits.x), limits.w - w);
	areaRect.y = MIN(MAX(areaRect.y, limits.y), limits.h - h);
	areaRect.w = areaRect.x + w;
	areaRect.h = areaRect.y + h;

	showArrow[0] = areaRect.y != limits.y;

	showArrow[1] = areaRect.y != limits.h - h;

	showArrow[2] = areaRect.x != limits.x;

	showArrow[3] = areaRect.x != limits.w - w;
}

