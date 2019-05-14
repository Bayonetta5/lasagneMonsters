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

static SDL_Point camera;

void initRadar(void)
{
	camera.x = camera.y = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (app.keyboard[SDL_SCANCODE_A])
	{
		camera.x++;
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		camera.x--;
	}

	if (app.keyboard[SDL_SCANCODE_W])
	{
		camera.y++;
	}

	if (app.keyboard[SDL_SCANCODE_S])
	{
		camera.y--;
	}
}

static void draw(void)
{
	Stage *s;
	int x, y, mx, my;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		for (mx = 0 ; mx < MAP_WIDTH ; mx++)
		{
			for (my = 0 ; my < MAP_HEIGHT ; my++)
			{
				if (s->map[mx][my] >= TILE_WALL && s->map[mx][my] < TILE_FOREGROUND)
				{
					x = 16 + ((mx + camera.x + s->x) * CELL_SIZE);
					y = 16 + ((my + camera.y + s->y) * CELL_SIZE);

					drawRect(x, y, CELL_SIZE - 1, CELL_SIZE - 1, 0, 192, 0, 255);
				}
			}
		}
	}
}
