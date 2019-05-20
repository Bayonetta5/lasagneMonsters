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

#include "door.h"

static void tick(void);
static void draw(void);
static void drawLight(void);
static void activate(int active);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *textures[3] = {NULL};

void initDoor(Entity *e)
{
	Door *d;

	d = malloc(sizeof(Door));
	memset(d, 0, sizeof(Door));

	d->sx = e->x;
	d->sy = e->y;
	d->ex = e->x;
	d->ey = e->y;
	d->speed = 1;

	e->typeName = "door";
	e->type = ET_DOOR;
	e->data = d;
	e->tick = tick;
	e->draw = draw;
	e->drawLight = drawLight;
	e->activate = activate;
	e->touch = touch;

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/entities/door1.png", 1);
		textures[1] = getAtlasImage("gfx/entities/door2.png", 1);
		textures[2] = getAtlasImage("gfx/entities/door3.png", 1);
	}

	e->atlasImage = textures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_PUSH+EF_NO_WORLD_CLIP;
	e->background = 1;

	/* when opened */
	d->ey = e->y - (e->h - 4);

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Door *d;
	int ty;

	d = (Door*)self->data;

	self->dy = 0;
	self->flags |= EF_STATIC;

	ty = self->y;

	if (d->open && self->y != d->ey)
	{
		ty = d->ey;
	}
	else if (!d->open && self->y != d->sy)
	{
		ty = d->sy;
	}

	if (ty != self->y)
	{
		if (abs(self->y - ty) > d->speed)
		{
			self->dy = (self->y < ty) ? d->speed : -d->speed;
			self->flags &= ~EF_STATIC;
		}
		else
		{
			self->y = ty;
			self->dy = 0;

			if (d->requires == DR_NOTHING)
			{
				playPositionalSound(SND_DOOR_DONE, CH_DOOR, self->x, self->y, world.player->x, world.player->y);
			}
		}
	}
}

static void draw(void)
{
	Door *d;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	if (app.dev.editor)
	{
		d = (Door*)self->data;

		drawRect(d->ex - world.camera.x, d->ey - world.camera.y, self->w, self->h, 168, 255, 168, 160);
		drawOutlineRect(d->ex - world.camera.x, d->ey - world.camera.y, self->w, self->h, 255, 255, 255, 160);
		drawLine(self->cx - world.camera.x, self->y - world.camera.y, d->ex + (self->w / 2) - world.camera.x, d->ey - world.camera.y, 255, 255, 255, 255);
	}
}

static void drawLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->cy - world.camera.y, 50, 128, 128, 128, 255);
}

static void activate(int active)
{
	Door *d;

	d = (Door*)self->data;

	d->open = !d->open;

	playPositionalSound(SND_DOOR_MECH, CH_DOOR, self->x, self->y, world.player->x, world.player->y);
}

static void touch(Entity *other)
{
	Door *d;

	if (other == world.player)
	{
		d = (Door*)self->data;

		if (!d->open)
		{
			if (d->requires == DR_KEY && game.keys > 0)
			{
				game.keys--;

				d->open = 1;

				self->flags |= EF_NO_WORLD_CLIP;

				playPositionalSound(SND_DOOR_MECH, CH_DOOR, self->x, self->y, world.player->x, world.player->y);
			}
			else if (d->requires == DR_NOTHING)
			{
				d->open = 1;

				self->flags |= EF_NO_WORLD_CLIP;

				playPositionalSound(SND_DOOR, CH_DOOR, self->x, self->y, world.player->x, world.player->y);
			}
		}
	}
}

static void load(cJSON *root)
{
	Door *d;

	d = (Door*)self->data;

	d->open = cJSON_GetObjectItem(root, "open")->valueint;
	d->requires = lookup(cJSON_GetObjectItem(root, "requires")->valuestring);
	d->speed = cJSON_GetObjectItem(root, "speed")->valueint;
	d->sx = cJSON_GetObjectItem(root, "sx")->valueint;
	d->sy = cJSON_GetObjectItem(root, "sy")->valueint;
	d->ex = cJSON_GetObjectItem(root, "ex")->valueint;
	d->ey = cJSON_GetObjectItem(root, "ey")->valueint;

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

static void save(cJSON *root)
{
	Door *d;

	d = (Door*)self->data;

	cJSON_AddNumberToObject(root, "open", d->open);
	cJSON_AddStringToObject(root, "requires", getLookupName("DR_", d->requires));
	cJSON_AddNumberToObject(root, "speed", d->speed);
	cJSON_AddNumberToObject(root, "sx", d->sx);
	cJSON_AddNumberToObject(root, "sy", d->sy);
	cJSON_AddNumberToObject(root, "ex", d->ex);
	cJSON_AddNumberToObject(root, "ey", d->ey);
}
