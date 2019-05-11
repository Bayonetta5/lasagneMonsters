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

#include "trafficLight.h"

static void draw(void);
static void drawLight(void);
static void toggle(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *goTexture = NULL;
static AtlasImage *stopTexture = NULL;
static AtlasImage *lightTexture = NULL;

void initTrafficLight(Entity *e)
{
	TrafficLight *t;

	if (goTexture == NULL)
	{
		goTexture = getAtlasImage("gfx/entities/trafficLightGo.png", 1);
		stopTexture = getAtlasImage("gfx/entities/trafficLightStop.png", 1);
		lightTexture = getAtlasImage("gfx/effects/light.png", 1);
	}

	t = malloc(sizeof(TrafficLight));
	memset(t, 0, sizeof(TrafficLight));

	e->typeName = "trafficLight";
	e->type = ET_SWITCH;
	e->data = t;
	e->atlasImage = stopTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->draw = draw;
	e->drawLight = drawLight;
	e->touch = touch;
	e->flags = EF_NO_ENT_CLIP+EF_STATIC;

	e->load = load;
	e->save = save;
}

static void draw(void)
{
	TrafficLight *t;
	int x, y;

	t = (TrafficLight*)self->data;

	if (t->on)
	{
		self->atlasImage = goTexture;
	}
	else
	{
		self->atlasImage = stopTexture;
	}

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);

	x = self->cx - world.camera.x;
	y = self->cy - world.camera.y;

	if (t->on)
	{
		SDL_SetTextureColorMod(lightTexture->texture, 128, 255, 64);
	}
	else
	{
		SDL_SetTextureColorMod(lightTexture->texture, 255, 128, 64);
		y -= 16;
	}

	SDL_SetTextureAlphaMod(lightTexture->texture, 64);

	blitAtlasImage(lightTexture, x, y, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(lightTexture->texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(lightTexture->texture, 255);

	if (app.dev.editor)
	{
		drawText(x, self->y - world.camera.y - 32, 32, TEXT_CENTER, app.colors.white, t->targetName);

		drawEntityLinkLines(self, t->targetName);
	}
}

static void drawLight(void)
{
	TrafficLight *t;
	int x, y;

	t = (TrafficLight*)self->data;

	x = self->cx - world.camera.x;
	y = self->cy - world.camera.y;

	if (t->on)
	{
		drawLightEffect(x, y, 16, 0, 255, 0, 255);
	}
	else
	{
		drawLightEffect(x, y - 16, 16, 255, 0, 0, 255);
	}
}

static void touch(Entity *other)
{
	if (other == world.player)
	{
		if (isControl(CONTROL_USE))
		{
			clearControl(CONTROL_USE);

			toggle();
		}
	}
}

static void toggle(void)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	t->on = !t->on;

	activeEntities(t->targetName, t->on);

	playPositionalSound(SND_TRAFFIC_LIGHT, -1, self->x, self->y, world.player->x, world.player->y);
}

static void load(cJSON *root)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	t->on = cJSON_GetObjectItem(root, "on")->valueint;
	STRNCPY(t->targetName, cJSON_GetObjectItem(root, "targetName")->valuestring, MAX_NAME_LENGTH);
}

static void save(cJSON *root)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	cJSON_AddStringToObject(root, "targetName", t->targetName);
	cJSON_AddNumberToObject(root, "on", t->on);
}
