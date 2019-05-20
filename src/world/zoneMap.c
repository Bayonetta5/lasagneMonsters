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

#include "zoneMap.h"

static void logic(void);
static void draw(void);
static void changeStage(int dx, int dy);
static void doCamera(int jumpTo);
static void drawStages(void);
static void drawStageInfo(void);
static void drawStageExits(void);

static PointF camera;
static Stage *selectedStage;
static AtlasImage *monstersTexture;
static AtlasImage *girlsTexture;
static AtlasImage *savePointTexture;
static AtlasImage *chestTexture;
static void (*returnFromRadar)(void);
static void (*oldDraw)(void);
static float pulse;

void initZoneMap(void (*done)(void))
{
	camera.x = camera.y = 0;

	oldDraw = app.delegate.draw;

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	selectedStage = stage;

	monstersTexture = getAtlasImage("gfx/hud/monsters.png", 1);
	girlsTexture = getAtlasImage("gfx/hud/girls.png", 1);
	savePointTexture = getAtlasImage("gfx/entities/computer1.png", 1);
	chestTexture = getAtlasImage("gfx/entities/chest1.png", 1);

	doCamera(1);

	pulse = 0;

	returnFromRadar = done;
}

static void logic(void)
{
	if (isControl(CONTROL_LEFT))
	{
		clearControl(CONTROL_LEFT);

		changeStage(-1, 0);
	}

	if (isControl(CONTROL_RIGHT))
	{
		clearControl(CONTROL_RIGHT);

		changeStage(1, 0);
	}

	if (isControl(CONTROL_UP))
	{
		clearControl(CONTROL_UP);

		changeStage(0, -1);
	}

	if (isControl(CONTROL_DOWN))
	{
		clearControl(CONTROL_DOWN);

		changeStage(0, 1);
	}

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

		initRadar(returnFromRadar);
	}

	doCamera(0);
}

static void changeStage(int dx, int dy)
{
	Stage *s;
	int x, y;

	x = selectedStage->x + dx;
	y = selectedStage->y + dy;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->visited && s->x == x && s->y == y)
		{
			selectedStage = s;

			playSound(SND_NAV, 0);
		}
	}
}

static void doCamera(int jumpTo)
{
	float dx, dy;
	int x, y;

	x = selectedStage->x * (CELL_SIZE + GRID_SPACING);
	x -= SCREEN_WIDTH / 2;

	y = selectedStage->y * (CELL_SIZE + GRID_SPACING);
	y -= SCREEN_HEIGHT * 0.4;

	if (!jumpTo && getDistance(camera.x, camera.y, x, y) > 16)
	{
		calcSlope(camera.x, camera.y, x, y, &dx, &dy);

		dx *= 16;
		dy *= 16;

		camera.x -= dx;
		camera.y -= dy;
	}
	else
	{
		camera.x = x;
		camera.y = y;
	}
}

static void draw(void)
{
	oldDraw();

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 64, 0, 192);

	drawStageExits();

	drawStages();

	drawStageInfo();

	drawText(SCREEN_WIDTH / 2, 8, 96, TEXT_CENTER, app.colors.white, "AREA MAP");

	drawText(SCREEN_WIDTH / 2, 80, 64, TEXT_CENTER, app.colors.white, "Zone %03d", selectedStage->id);

	drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 64, 32, TEXT_CENTER, app.colors.white, "[USE] View Area Map");
}

static void drawStages(void)
{
	Stage *s;
	int x, y, selectCol;

	pulse += 0.125;
	selectCol = 128 + ((sin(pulse) * 256) / 2);

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->visited)
		{
			x = (s->x * (CELL_SIZE + GRID_SPACING)) - camera.x;
			x -= CELL_SIZE / 2;

			y = (s->y * (CELL_SIZE + GRID_SPACING)) - camera.y;
			y -= CELL_SIZE / 2;

			if (s->numMonsters > 0)
			{
				drawRect(x, y, CELL_SIZE, CELL_SIZE, 64, 0, 0, 255);
				drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 168, 0, 0, 255);
			}
			else if (s->numGirls > 0)
			{
				drawRect(x, y, CELL_SIZE, CELL_SIZE, 64, 64, 0, 255);
				drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 168, 168, 0, 255);
			}
			else
			{
				drawRect(x, y, CELL_SIZE, CELL_SIZE, 0, 64, 0, 255);
				drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 0, 168, 0, 255);
			}

			if (s == stage)
			{
				if (SDL_GetTicks() % 1000 < 500)
				{
					drawRect(x, y, CELL_SIZE, CELL_SIZE, 255, 255, 255, 128);
				}
			}
			else if (s == selectedStage)
			{
				drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 255, 255, 255, selectCol);
			}
		}

		if (app.dev.debug)
		{
			drawText(x + (CELL_SIZE / 2), y + 12, 16, TEXT_CENTER, app.colors.white, "%d", s->id, s->x, s->y);
			drawText(x + (CELL_SIZE / 2), y + 32, 16, TEXT_CENTER, app.colors.white, "(%d,%d)", s->id, s->x, s->y);
		}
	}
}

static void drawStageInfo(void)
{
	int y;

	y = SCREEN_HEIGHT - 50;

	if (selectedStage->numMonsters > 0)
	{
		blitAtlasImage(monstersTexture, 25, y, 0, SDL_FLIP_NONE);

		drawText(25 + monstersTexture->rect.w + 16, y, 32, TEXT_LEFT, app.colors.white, "x %03d", selectedStage->numMonsters);

		y -= 40;
	}

	if (selectedStage->numGirls > 0)
	{
		blitAtlasImage(girlsTexture, 25, y, 0, SDL_FLIP_NONE);

		drawText(25 + girlsTexture->rect.w + 16, y, 32, TEXT_LEFT, app.colors.white, "x %d", selectedStage->numGirls);

		y -= 40;
	}

	if (selectedStage->numSavePoints > 0)
	{
		blitAtlasImage(savePointTexture, 25, y, 0, SDL_FLIP_NONE);

		drawText(25 + savePointTexture->rect.w + 16, y, 32, TEXT_LEFT, app.colors.white, "x 1");

		y -= 40;
	}

	if (selectedStage->numChests > 0)
	{
		blitAtlasImage(chestTexture, 25, y, 0, SDL_FLIP_NONE);

		drawText(25 + chestTexture->rect.w + 16, y, 32, TEXT_LEFT, app.colors.white, "x %d", selectedStage->numChests);

		y -= 40;
	}
}

static void linkStage(Stage *s, int dx, int dy)
{
	Stage *other;
	int sx, sy, ex, ey;

	other = getAdjacentStage(s, dx, dy);

	if (other != NULL)
	{
		sx = (s->x * (CELL_SIZE + GRID_SPACING));
		sy = (s->y * (CELL_SIZE + GRID_SPACING));
		sx -= camera.x;
		sy -= camera.y;

		ex = (other->x * (CELL_SIZE + GRID_SPACING));
		ey = (other->y * (CELL_SIZE + GRID_SPACING));
		ex -= camera.x;
		ey -= camera.y;

		drawLine(sx, sy, ex, ey, 255, 255, 255, 255);
	}
}

static void drawStageExits(void)
{
	Stage *s;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->visited)
		{
			if (s->exits.n)
			{
				linkStage(s, 0, -1);
			}

			if (s->exits.s)
			{
				linkStage(s, 0, 1);
			}

			if (s->exits.e)
			{
				linkStage(s, 1, 0);
			}

			if (s->exits.w)
			{
				linkStage(s, -1, 0);
			}
		}
	}
}
