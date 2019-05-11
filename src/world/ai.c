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

#include "ai.h"

static int hasLOS(void);
static int traceMap(void);
static int traceEntities(void);

void lookForPlayer(void)
{
	if (world.player->alive == ALIVE_ALIVE)
	{
		if (getDistance(self->x, self->y, world.player->x, world.player->y) <= SCREEN_HEIGHT)
		{
			if ((self->facing == FACING_LEFT && world.player->x < self->x) || (self->facing == FACING_RIGHT && world.player->x > self->x))
			{
				if (hasLOS())
				{
					((Monster*)self->data)->alert = 1;

					return;
				}
			}
		}
	}
	else
	{
		((Monster*)self->data)->alert = 0;
	}
}

static int hasLOS(void)
{
	return traceMap() && traceEntities();
}

static int traceMap(void)
{
	int x1, y1, x2, y2, dx, dy, sx, sy, err, e2;

	x1 = (self->x + (self->w / 2)) / TILE_SIZE;
	y1 = (self->y + (self->h / 2)) / TILE_SIZE;
	x2 = (world.player->x + (world.player->w / 2)) / TILE_SIZE;
	y2 = (world.player->y + (world.player->h / 2)) / TILE_SIZE;

	dx = abs(x2 - x1);
	dy = abs(y2 - y1);

	sx = (x1 < x2) ? 1 : -1;
	sy = (y1 < y2) ? 1 : -1;
	err = dx - dy;

	while (1)
	{
		e2 = 2 * err;

		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}

		if (x1 == x2 && y1 == y2)
		{
			return 1;
		}

		if (!isInsideMap(x1, y1) || stage->map[x1][y1] != 0)
		{
			return 0;
		}
	}

	return 0;
}

static int traceEntities(void)
{
	int i, x1, y1, x2, y2, targetDistance;
	Entity *candidates[MAX_QT_CANDIDATES], *e;

	x1 = MIN(world.player->cx, self->cx);
	y1 = MIN(world.player->cy, self->cy);
	x2 = MAX(world.player->cx, self->cx);
	y2 = MAX(world.player->cy, self->cy);

	getAllEntsWithin(x1, y1, x2, y2, candidates, self);

	targetDistance = getDistance(self->cx, self->cy, world.player->cx, world.player->cy);

	for (i = 0, e = candidates[0] ; i < MAX_QT_CANDIDATES && e != NULL ; e = candidates[++i])
	{
		if (lineRectCollision(x1, y1, x2, y2, e->x, e->y, e->w, e->h))
		{
			if ((e->flags & EF_SOLID || e->type == ET_MONSTER) && getDistance(self->cx, self->cy, e->cx, e->cy) < targetDistance)
			{
				return 0;
			}
		}
	}

	return 1;
}
