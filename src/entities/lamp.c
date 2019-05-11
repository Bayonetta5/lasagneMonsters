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

#include "lamp.h"

static void draw(void);
static void drawLight(void);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *texture = NULL;

void initLamp(Entity *e)
{
	Lamp *l;

	l = malloc(sizeof(Lamp));
	memset(l, 0, sizeof(Lamp));

	l->r = l->g = l->b = l->a = 255;

	e->typeName = "lamp";
	e->data = l;
	e->type = ET_LIGHT;
	e->flags = EF_STATIC+EF_NO_COLS;

	if (texture == NULL)
	{
		texture = getAtlasImage("gfx/entities/streetLamp.png", 1);
	}

	e->draw = draw;
	e->drawLight = drawLight;

	e->atlasImage = texture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	e->load = load;
	e->save = save;
}

static void draw(void)
{
	int x, y;

	x = self->x - world.camera.x;
	y = self->y - world.camera.y;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);

	x += (self->w / 2);

	y += 8;

	drawObjectGlowAt(x, y, 255, 255, 192, 96);
}

static void drawLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->y - world.camera.y, 75, 255, 255, 168, 255);
}

static void load(cJSON *root)
{
	Lamp *l;

	l = (Lamp*)self->data;

	l->r = cJSON_GetValueInt(root, "r", 255);
	l->g = cJSON_GetValueInt(root, "g", 255);
	l->b = cJSON_GetValueInt(root, "b", 255);
	l->a = cJSON_GetValueInt(root, "a", 255);
}

static void save(cJSON *root)
{
	Lamp *l;

	l = (Lamp*)self->data;

	cJSON_AddNumberToObject(root, "r", l->r);
	cJSON_AddNumberToObject(root, "g", l->g);
	cJSON_AddNumberToObject(root, "b", l->b);
	cJSON_AddNumberToObject(root, "a", l->a);
}
