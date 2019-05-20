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

#include "horizontalDoor.h"

static void tick(void);
static void load(cJSON *root);

static AtlasImage *textures[3] = {NULL};
static void (*superLoad)(cJSON *root);

void initHorizontalDoor(Entity *e)
{
	Door *d;

	initDoor(e);

	d = (Door*)e->data;

	e->typeName = "horizontalDoor";
	e->tick = tick;

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/entities/horizontalDoor1.png", 1);
		textures[1] = getAtlasImage("gfx/entities/horizontalDoor2.png", 1);
		textures[2] = getAtlasImage("gfx/entities/horizontalDoor3.png", 1);
	}

	superLoad = e->load;

	e->load = load;

	/* when opened */
	d->ey = e->y;
	d->ex = e->x - (e->w - 4);
}

static void tick(void)
{
	Door *d;
	int tx;

	d = (Door*)self->data;

	self->dx = 0;
	self->flags |= EF_STATIC;

	tx = self->x;

	if (d->open && self->x != d->ex)
	{
		tx = d->ex;
	}
	else if (!d->open && self->x != d->sx)
	{
		tx = d->sx;
	}

	if (tx != self->x)
	{
		if (abs(self->x - tx) > d->speed)
		{
			self->dx = (self->x < tx) ? d->speed : -d->speed;
			self->flags &= ~EF_STATIC;
		}
		else
		{
			self->x = tx;
			self->dx = 0;

			playPositionalSound(SND_DOOR_DONE, CH_DOOR, self->x, self->y, world.player->x, world.player->y);
		}
	}
}

static void load(cJSON *root)
{
	Door *d;

	superLoad(root);

	d = (Door*)self->data;

	switch (d->requires)
	{
		case DR_KEY:
			self->atlasImage = textures[1];
			break;

		case DR_REMOTE:
			self->atlasImage = textures[2];
			break;

		default:
			self->atlasImage = textures[0];
			break;
	}

	self->w = self->atlasImage->rect.w;
	self->h = self->atlasImage->rect.h;
}
