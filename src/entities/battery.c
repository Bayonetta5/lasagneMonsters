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

#include "battery.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *batteryTexture = NULL;

void initBattery(Entity *e)
{
	Item *h;

	h = malloc(sizeof(Item));
	memset(h, 0, sizeof(Item));

	e->typeName = "battery";
	e->type = ET_ITEM;
	e->data = h;
	e->flags = EF_DELETE;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;

	if (batteryTexture == NULL)
	{
		batteryTexture = getAtlasImage("gfx/entities/battery.png", 1);
	}

	e->atlasImage = batteryTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	itemHop();
}

static void draw(void)
{
	drawObjectGlow(255, 255, 255, 64);

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	Walter *w;

	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		w = (Walter*)other->data;

		w->refillRate += 0.01f;

		self->alive = ALIVE_DEAD;

		addPowerupParticles(self->x + self->w / 2, self->y + self->h / 2);

		addGameText(self->x, self->y, "Refill Rate Up!");

		playSound(SND_POWERUP, -1);
	}
}
