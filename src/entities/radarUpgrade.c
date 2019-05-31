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

#include "radarUpgrade.h"

static void tick(void);
static void draw(void);
static void loadTextures(void);
static void touch(Entity *other);

static AtlasImage *textures[NUM_TEXTURES] = {NULL};

void initRadarUpgrade(Entity *e)
{
	Item *i;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));

	e->typeName = "radarUpgrade";
	e->type = ET_ITEM;
	e->data = i;
	e->flags = EF_DELETE;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;

	if (textures[0] == NULL)
	{
		loadTextures();
	}

	e->atlasImage = textures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Item *i;

	i = (Item*)self->data;

	if (--i->frameTime <= 0)
	{
		if (++i->frame >= NUM_TEXTURES)
		{
			i->frame = 0;
		}

		self->atlasImage = textures[i->frame];

		i->frameTime = FPS / 3;
	}

	itemHop();
}

static void draw(void)
{
	drawObjectGlow(128, 255, 128, 64);

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	Walter *w;

	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		w = (Walter*)other->data;

		w->hasRadarUpgrade = 1;

		self->alive = ALIVE_DEAD;

		addPowerupParticles(self->cx, self->cy);

		addGameText(self->x, self->y, "Radar Upgraded!");

		playSound(SND_POWERUP, -1);
	}
}

static void loadTextures(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 0 ; i < NUM_TEXTURES ; i++)
	{
		sprintf(filename, "gfx/entities/radarUpgrade%d.png", i + 1);

		textures[i] = getAtlasImage(filename, 1);
	}
}
