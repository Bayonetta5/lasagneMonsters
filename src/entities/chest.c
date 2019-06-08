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

#include "chest.h"

static void tick(void);
static void init(void);
static void touch(Entity *other);
static void loadTextures(void);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *textures[4] = {NULL};

void initChest(Entity *e)
{
	Chest *c;

	c = malloc(sizeof(Chest));
	memset(c, 0, sizeof(Chest));

	e->typeName = "chest";
	e->type = ET_CHEST;
	e->data = c;
	e->tick = tick;
	e->touch = touch;

	if (textures[0] == NULL)
	{
		loadTextures();
	}

	e->atlasImage = textures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	e->init = init;
	e->load = load;
	e->save = save;

	stage->numChests++;
}

static void tick(void)
{
	Chest *c;

	c = (Chest*)self->data;

	if (c->open && c->coins > 0 && --c->delay <= 0)
	{
		if (!c->found)
		{
			game.stats[STAT_CHESTS_OPENED]++;
		}

		c->found = 1;

		c->coins--;
		c->delay = FPS / 10;

		playPositionalSound(SND_CHEST, -1, self->x, self->y, world.player->x, world.player->y);

		throwCoins(self->x, self->y, 1);

		c->nextRefill = game.stats[STAT_TIME] + REFILL_TIME;
	}
}

static void touch(Entity *other)
{
	Chest *c;

	if (other == world.player)
	{
		c = (Chest*)self->data;

		if (!c->open)
		{
			c->open = 1;

			self->atlasImage = !c->found ? textures[c->open] : textures[c->open + 2];
		}
	}
}

void init(void)
{
	Chest *c;

	c = (Chest*)self->data;

	if (c->nextRefill <= game.stats[STAT_TIME])
	{
		c->open = 0;
		c->coins = 3 + rand() % 8;
		c->delay = 0;
	}

	self->atlasImage = !c->found ? textures[c->open] : textures[c->open + 2];
}

static void loadTextures(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 0 ; i < 4 ; i++)
	{
		sprintf(filename, "gfx/entities/chest%d.png", i + 1);

		textures[i] = getAtlasImage(filename, 1);
	}
}

static void load(cJSON *root)
{
	Chest *c;

	c = (Chest*)self->data;

	c->open = cJSON_GetValueInt(root, "open", 0);
	c->coins = cJSON_GetValueInt(root, "coins", 0);
	c->nextRefill = cJSON_GetValueInt(root, "nextRefill", 0);
	c->found = cJSON_GetValueInt(root, "found", 0);

	self->atlasImage = textures[c->open];
}

static void save(cJSON *root)
{
	Chest *c;

	c = (Chest*)self->data;

	cJSON_AddNumberToObject(root, "open", c->open);
	cJSON_AddNumberToObject(root, "coins", c->coins);
	cJSON_AddNumberToObject(root, "nextRefill", c->nextRefill);
	cJSON_AddNumberToObject(root, "found", c->found);
	cJSON_AddNumberToObject(root, "isStatEntity", 1);
}
