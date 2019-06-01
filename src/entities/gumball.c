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

#include "gumball.h"

static void initGumballTextures(void);
static void tick(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *gumballTexture[NUM_GUMBALLS] = {NULL};

void initGumball(Entity *e)
{
	Item *i;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));

	i->touchDelay = FPS / 3;
	i->value = rand() % NUM_GUMBALLS;

	e->typeName = "gumball";
	e->type = ET_POWERUP;
	e->data = i;
	e->flags = EF_FRICTION;
	e->tick = tick;

	if (gumballTexture[0] == NULL)
	{
		initGumballTextures();
	}

	e->atlasImage = gumballTexture[i->value];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Item *i;

	i = (Item*)self->data;

	if (--i->touchDelay <= 0)
	{
		self->touch = touch;
	}
}

static void touch(Entity *other)
{
	Walter *w;

	if (self->alive == ALIVE_ALIVE && other == world.player)
	{
		w = (Walter*)other->data;

		w->health = MIN(w->health + 1, w->maxHealth);

		addGameText(self->x, self->y, "+1HP");

		self->alive = ALIVE_DEAD;

		playSound(SND_CRUNCH, CH_ITEM);
	}
}

static void initGumballTextures(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 0 ; i < NUM_GUMBALLS ; i++)
	{
		sprintf(filename, "gfx/entities/gumball%d.png", i + 1);

		gumballTexture[i] = getAtlasImage(filename, 1);
	}
}

static void load(cJSON *root)
{
	Item *i;

	i = (Item*)self->data;

	i->value = cJSON_GetObjectItem(root, "value")->valueint;

	self->atlasImage = gumballTexture[i->value];
}

static void save(cJSON *root)
{
	Item *i;

	i = (Item*)self->data;

	cJSON_AddNumberToObject(root, "value", i->value);
}
