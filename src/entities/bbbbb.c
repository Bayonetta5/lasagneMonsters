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
static void damage(int amount, int damageType);
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
	e->flags = EF_SOLID;
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

	stage->numGirls++;
}

static void tick(void)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	b->hitTimer = MAX(b->hitTimer - 16, 0);

	if (b->health > 1 && b->cageType == CT_SWINGING)
	{
		b->swingX += 0.05f;
		b->swingY += 0.1f;

		self->dx = cos(b->swingX) * 0.5;
		self->dy = sin(-b->swingY) * 0.25;
	}
}

static void draw(void)
{
	BBBBB *b;
	float x, y;

	b = (BBBBB*)self->data;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);

	if (b->health > 1)
	{
		x = self->x + (self->atlasImage->rect.w / 2) - (cageTexture->rect.w / 2) - world.camera.x;
		y = (self->y - world.camera.y) - self->h / 2;

		if (b->hitTimer > 0)
		{
			SDL_SetTextureBlendMode(self->atlasImage->texture, SDL_BLENDMODE_ADD);
			SDL_SetTextureColorMod(self->atlasImage->texture, 255, 255 - b->hitTimer, 255 - b->hitTimer);

			blitAtlasImage(cageTexture, x, y, 0, SDL_FLIP_NONE);

			SDL_SetTextureColorMod(self->atlasImage->texture, 255, 255, 255);
			SDL_SetTextureBlendMode(self->atlasImage->texture, SDL_BLENDMODE_BLEND);
		}
		else
		{
			blitAtlasImage(cageTexture, x, y, 0, SDL_FLIP_NONE);
		}

		if (b->cageType != CT_STANDING)
		{
			drawLine(self->x + (self->atlasImage->rect.w / 2) - world.camera.x, self->y - 8 - world.camera.y, b->chainX - world.camera.x, b->chainY - world.camera.y, 192, 192, 192, 255);
		}
	}
}

static void damage(int amount, int damageType)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	if (b->health > 1 && damageType == DT_WATER)
	{
		b->health = MAX(b->health - amount, 1);

		b->hitTimer = 255;

		if (b->health == 1)
		{
			self->flags &= ~EF_WEIGHTLESS;
			self->dx = self->dy = 0;
			self->damage = NULL;

			addCageBreakParticles(self->cx, self->cy);

			playPositionalSound(SND_BREAK, -1, self->x, self->y, world.player->x, world.player->y);
		}
	}
}

static void touch(Entity *other)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	/* hit ground, add static */
	if (other == NULL)
	{
		self->flags |= EF_STATIC;
	}

	if (other == world.player && b->health == 1)
	{
		self->alive = ALIVE_DEAD;
		b->health = 0;
		stage->numGirls--;

		addBBBBBParticles(self->x, self->y);

		playSound(SND_FANFARE, -1);
	}
}

static void attachChain(void)
{
	BBBBB *b;
	int x, y, found;

	b = (BBBBB*)self->data;

	x = (self->x + (self->w / 2)) / TILE_SIZE;
	y = self->y / TILE_SIZE;

	do
	{
		y--;

		found = !isInsideMap(x, y) || stage->map[x][y] != 0;
	}
	while (!found);

	b->chainX = self->x + (self->w / 2);
	b->chainY = (y * TILE_SIZE) + TILE_SIZE;
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
	b->cageType = lookup(cJSON_GetObjectItem(root, "cageType")->valuestring);

	switch (b->cageType)
	{
		case CT_HANGING:
		case CT_SWINGING:
			self->flags |= EF_WEIGHTLESS;
			attachChain();
			break;

		default:
			break;
	}
}

static void save(cJSON *root)
{
	BBBBB *b;

	b = (BBBBB*)self->data;

	cJSON_AddNumberToObject(root, "health", b->health);
	cJSON_AddStringToObject(root, "cageType", getLookupName("CT_", b->cageType));
}
