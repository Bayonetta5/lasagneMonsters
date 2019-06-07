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

#include "waterBottle.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);
static void save(cJSON *root);

static AtlasImage *waterBottleTexture = NULL;

void initWaterBottle(Entity *e)
{
	Item *h;

	h = malloc(sizeof(Item));
	memset(h, 0, sizeof(Item));

	e->typeName = "waterBottle";
	e->type = ET_POWERUP;
	e->data = h;
	e->flags = EF_DELETE;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
	e->save = save;

	if (waterBottleTexture == NULL)
	{
		waterBottleTexture = getAtlasImage("gfx/entities/waterBottle.png", 1);
	}

	e->atlasImage = waterBottleTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	stage->numWaterBottles++;
}

static void tick(void)
{
	itemHop();
}

static void draw(void)
{
	drawObjectGlow(224, 224, 255, 64);

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	Walter *w;

	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		w = (Walter*)other->data;

		w->ammo = ++w->maxAmmo;

		self->alive = ALIVE_DEAD;

		addPowerupParticles(self->cx, self->cy);

		addGameText(self->x, self->y, "Capacity Up!");

		playSound(SND_POWERUP, -1);
	}
}

static void save(cJSON *root)
{
	cJSON_AddNumberToObject(root, "isStatEntity", 1);
}
