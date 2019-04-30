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

static int hasLOS(int x1, int y1, int x2, int y2);

void chasePlayer(int speed)
{
	if (self->x < stage.player->x)
	{
		self->dx = speed;
	}
	else if (self->x > stage.player->x)
	{
		self->dx = -speed;
	}
}

void haltAtEdge(void)
{
	int mx, my;
	
	if (self->dx != 0)
	{
		my = (self->y + self->h) / TILE_SIZE;
		
		if (self->dx < 0)
		{
			mx = self->x - 1;
		}
		else
		{
			mx = self->x + self->w;
		}
		
		mx /= TILE_SIZE;
		
		if (stage.map[mx][my] == 0)
		{
			self->dx = 0;
			((Monster*)self->data)->thinkTime = FPS / 4;
		}
	}
}

int lookForPlayer(void)
{
	int x1, y1, x2, y2;
	Monster *m;
	
	if (getDistance(self->x, self->y, stage.player->x, stage.player->y) <= SCREEN_HEIGHT)
	{
		if ((self->facing == FACING_LEFT && stage.player->x < self->x) || (self->facing == FACING_RIGHT && stage.player->x > self->x))
		{
			x1 = (self->x + (self->w / 2)) / TILE_SIZE;
			y1 = (self->y + (self->h / 2)) / TILE_SIZE;
			x2 = (stage.player->x + (stage.player->w / 2)) / TILE_SIZE;
			y2 = (stage.player->y + (stage.player->h / 2)) / TILE_SIZE;
			
			if (hasLOS(x1, y1, x2, y2))
			{
				m = (Monster*)self->data;
				
				m->alertTimer = FPS;
				
				return 1;
			}
		}
	}
	
	return 0;
}

static int hasLOS(int x1, int y1, int x2, int y2)
{
	int dx, dy, sx, sy, err, e2;
	
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
		
		if (!isInsideMap(x1, y1) || stage.map[x1][y1] != 0)
		{
			return 0;
		}
	}

	return 0;
}
