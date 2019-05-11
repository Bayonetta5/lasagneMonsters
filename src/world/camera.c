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

#include "camera.h"

void doCamera(void)
{
	world.camera.x = world.player->cx;
	world.camera.y = world.player->cy;

	world.camera.x -= (SCREEN_WIDTH / 2);
	world.camera.y -= (SCREEN_HEIGHT / 2);

	world.camera.x = MIN(MAX(world.camera.x, world.camera.minX), world.camera.maxX - SCREEN_WIDTH + (TILE_SIZE - 64));
	world.camera.y = MIN(MAX(world.camera.y, world.camera.minY), world.camera.maxY - SCREEN_HEIGHT + (TILE_SIZE - 64));
}

void updateCameraBounds(void)
{
	int x, y;

	world.camera.minX = MAP_WIDTH;
	world.camera.maxX = 0;

	world.camera.minY = MAP_HEIGHT;
	world.camera.maxY = 0;

	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			if (stage->map[x][y] != 0)
			{
				world.camera.maxX = MAX(world.camera.maxX, x + 1);
				world.camera.minX = MIN(world.camera.minX, x);

				world.camera.maxY = MAX(world.camera.maxY, y + 1);
				world.camera.minY = MIN(world.camera.minY, y);
			}
		}
	}

	world.camera.minX *= TILE_SIZE;
	world.camera.maxX *= TILE_SIZE;

	world.camera.minY *= TILE_SIZE;
	world.camera.maxY *= TILE_SIZE;
}
