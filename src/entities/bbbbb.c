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

#include "bbbbb.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);
static void attachChain(void);
static void damage(int amount);
static void load(cJSON *root);
static void save(cJSON *root);
static void initTextures(void);

static AtlasImage *bbbbbTextures[NUM_TEXTURES] = {NULL};
static AtlasImage *cageTexture;
static AtlasImage *chainLinkTexture;

void initBBBBB(Entity *e)
{
	BBBBB *b;

	b = malloc(sizeof(BBBBB));
	memset(b, 0, sizeof(BBBBB));

	b->health = 5;

	e->typeName = "bbbbb";
	e->type = ET_STRUCTURE;
	e->data = b;
	e->flags = EF_WEIGHTLESS+EF_SOLID;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
	e->damage = damage;

	if (bbbbbTextures[0] == NULL)
	{
		initTextures();
	}

	e->atlasImage = bbbbbTextures[rand() % NUM_TEXTURES];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	e->load = load;
	e->save = save;

	attachChain();
}

static void tick(void)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	if (b->health > 1)
	{
		b->swingX += 0.025f;
		b->swingY += 0.05f;

		self->dx = cos(b->swingX) * 2;
		self->dy = sin(-b->swingY);
	}
}

static void draw(void)
{
	BBBBB *b;
	float x, y;

	b = (BBBBB*)self->data;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);

	if (b->health > 0)
	{
		x = self->x + (self->atlasImage->rect.w / 2) - (cageTexture->rect.w / 2) - world.camera.x;
		y = self->y;

		blitAtlasImage(cageTexture, x, y, 0, SDL_FLIP_NONE);

		if (b->health > 1)
		{
			drawLine(self->x + (self->atlasImage->rect.w / 2) - world.camera.x, self->y - world.camera.y, b->chainX - world.camera.x, b->chainY - world.camera.y, 192, 192, 192, 255);
		}
	}
}

static void damage(int amount)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	if (b->health > 1)
	{
		if (--b->health == 1)
		{
			self->flags &= ~EF_WEIGHTLESS;
			self->dx = self->dy = 0;
			self->damage = NULL;
		}
	}
}

static void touch(Entity *other)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	/* hit ground, remove solid */
	if (other == NULL)
	{
		self->flags &= ~EF_SOLID;
	}

	if (other == world.player && b->health == 1)
	{
		b->health = 0;
	}
}

static void attachChain(void)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	b->chainX = self->x + (self->w / 2);
	b->chainY = self->y - 100;
}

static void initTextures(void)
{
	char filename[MAX_FILENAME_LENGTH];
	int i;

	for (i = 0 ; i < NUM_TEXTURES ; i++)
	{
		sprintf(filename, "gfx/entities/bbbbb%d.png", i + 1);

		bbbbbTextures[i] = getAtlasImage(filename, 1);
	}

	cageTexture = getAtlasImage("gfx/entities/cage.png", 1);

	chainLinkTexture = getAtlasImage("gfx/entities/cageChainLink.png", 1);
}

static void load(cJSON *root)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	b->health = cJSON_GetObjectItem(root, "health")->valueint;
}

static void save(cJSON *root)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	cJSON_AddNumberToObject(root, "health", b->health);
}
