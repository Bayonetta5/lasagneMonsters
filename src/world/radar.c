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
static void changeStage(int dx, int dy);
static void updateCamera(void);

static SDL_Point camera;
static Stage *selectedStage;

void initRadar(void)
{
	camera.x = camera.y = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	selectedStage = world.stagesHead.next;

	updateCamera();
}

static void logic(void)
{
	if (app.keyboard[SDL_SCANCODE_A])
	{
		app.keyboard[SDL_SCANCODE_A] = 0;

		changeStage(-1, 0);
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		app.keyboard[SDL_SCANCODE_D] = 0;

		changeStage(1, 0);
	}

	if (app.keyboard[SDL_SCANCODE_W])
	{
		app.keyboard[SDL_SCANCODE_W] = 0;

		changeStage(0, -1);
	}

	if (app.keyboard[SDL_SCANCODE_S])
	{
		app.keyboard[SDL_SCANCODE_S] = 0;

		changeStage(0, 1);
	}
}

static void changeStage(int dx, int dy)
{
	Stage *s;
	int x, y;

	x = selectedStage->x + (GRID_SPACING * 2 * dx);
	y = selectedStage->y + (GRID_SPACING * 2 * dy);

	printf("%d %d\n", x, y);

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->x == x && s->y == y)
		{
			selectedStage = s;
		}
	}

	updateCamera();
}

static void updateCamera(void)
{
	camera.x = selectedStage->x - ((SCREEN_WIDTH - CELL_SIZE) / 2);
	camera.y = selectedStage->y - ((SCREEN_HEIGHT - CELL_SIZE) / 2);
}

static void draw(void)
{
	Stage *s;
	int x, y;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		x = s->x - camera.x;
		y = s->y - camera.y;

		if (s == selectedStage)
		{
			drawRect(x, y, CELL_SIZE, CELL_SIZE, 128, 128, 255, 255);
			drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 192, 192, 255, 255);
		}
		else
		{
			drawRect(x, y, CELL_SIZE, CELL_SIZE, 0, 128, 0, 255);
			drawOutlineRect(x, y, CELL_SIZE, CELL_SIZE, 0, 255, 0, 255);
		}
	}
}
