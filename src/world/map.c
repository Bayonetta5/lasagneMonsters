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

#include "map.h"

static void loadTiles(void);
static void loadMap(cJSON *root);
int isInsideMap(int x, int y);
static void randomizeTiles(void);

void initMap(cJSON *root)
{
	memset(&stage->map, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);

	loadTiles();

	loadMap(root);
}

void drawMap(void)
{
	int x, y, n, x1, x2, y1, y2, mx, my;

	x1 = (world.camera.x % TILE_SIZE) * -1;
	x2 = x1 + MAP_RENDER_WIDTH * TILE_SIZE + (x1 == 0 ? 0 : TILE_SIZE);

	y1 = (world.camera.y % TILE_SIZE) * -1;
	y2 = y1 + MAP_RENDER_HEIGHT * TILE_SIZE + (y1 == 0 ? 0 : TILE_SIZE);

	mx = world.camera.x / TILE_SIZE;
	my = world.camera.y / TILE_SIZE;

	for (y = y1 ; y < y2 ; y += TILE_SIZE)
	{
		for (x = x1 ; x < x2 ; x += TILE_SIZE)
		{
			if (isInsideMap(mx, my))
			{
				n = stage->map[mx][my];

				if (n > 0)
				{
					blitAtlasImage(world.tiles[n], x, y, 0, SDL_FLIP_NONE);
				}
			}

			mx++;
		}

		mx = world.camera.x / TILE_SIZE;

		my++;
	}
}

static void loadTiles(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 1 ; i <= MAX_TILES ; i++)
	{
		sprintf(filename, "gfx/tiles/%d.png", i);

		world.tiles[i] = getAtlasImage(filename, 0);
	}
}

static void loadMap(cJSON *root)
{
	char *data, *cData, *p;
	int x, y;
	unsigned long eLen, cLen, dLen;

	cData = cJSON_GetObjectItem(root, "data")->valuestring;
	eLen = cJSON_GetObjectItem(root, "eLen")->valueint;
	cLen = cJSON_GetObjectItem(root, "cLen")->valueint;
	dLen = cJSON_GetObjectItem(root, "dLen")->valueint;

	data = decompressData(cData, eLen, cLen, dLen);

	if (data)
	{
		p = data;

		for (y = 0 ; y < MAP_HEIGHT ; y++)
		{
			for (x = 0 ; x < MAP_WIDTH ; x++)
			{
				stage->map[x][y] = atoi(p);

				do {p++;} while (*p != ' ');
			}
		}
	}

	if (!app.dev.editor)
	{
		randomizeTiles();
	}

	free(data);
}

static void randomizeTiles(void)
{
	int x, y;

	if (!app.dev.editor)
	{
		for (y = 0 ; y < MAP_HEIGHT ; y++)
		{
			for (x = 0 ; x < MAP_WIDTH ; x++)
			{
				if (stage->map[x][y] == 1 && rand() % 2 == 0)
				{
					stage->map[x][y] += rand() % 4;
				}
			}
		}
	}
}

int isInsideMap(int x, int y)
{
	return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}
