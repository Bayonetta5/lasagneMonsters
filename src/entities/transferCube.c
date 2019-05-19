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

#include "transferCube.h"

static void draw(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

void initTransferCube(Entity *e)
{
	TransferCube *t;

	t = malloc(sizeof(TransferCube));
	memset(t, 0, sizeof(TransferCube));

	t->w = 1;
	t->h = 1;

	e->typeName = "transferCube";
	e->type = ET_TRANSFER_CUBE;
	e->data = t;
	e->draw = draw;
	e->touch = touch;
	e->atlasImage = getAtlasImage("gfx/entities/transferCube.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_WORLD_CLIP+EF_STATIC;

	e->load = load;
	e->save = save;
}

static void draw(void)
{
	SDL_Rect dest;
	TransferCube *t;

	t = (TransferCube*)self->data;

	dest.x = self->x - world.camera.x;
	dest.y = self->y - world.camera.y;
	dest.w = self->w;
	dest.h = self->h;

	SDL_RenderCopyEx(app.renderer, self->atlasImage->texture, &self->atlasImage->rect, &dest, 0, NULL, SDL_FLIP_NONE);

	if (app.dev.editor)
	{
		drawText(self->x - world.camera.x + (self->w / 2), self->y - world.camera.y - 32, 32, TEXT_CENTER, app.colors.white, "%d (%s)", t->targetStage, t->targetFlag);
	}
}

static void touch(Entity *other)
{
	if (other != NULL)
	{
		if (other == world.player)
		{
			world.transferCube = (TransferCube*)self->data;
		}
		else
		{
			other->alive = ALIVE_DEAD;
			other->flags |= EF_TRANSIENT;
		}
	}
}

static void load(cJSON *root)
{
	TransferCube *t;

	t = (TransferCube*)self->data;

	t->targetStage = cJSON_GetObjectItem(root, "targetStage")->valueint;
	STRNCPY(t->targetFlag, cJSON_GetObjectItem(root, "targetFlag")->valuestring, MAX_NAME_LENGTH);
	t->w = cJSON_GetObjectItem(root, "w")->valueint;
	t->h = cJSON_GetObjectItem(root, "h")->valueint;

	self->w *= t->w;
	self->h *= t->h;
}

static void save(cJSON *root)
{
	TransferCube *t;

	t = (TransferCube*)self->data;

	cJSON_AddNumberToObject(root, "targetStage", t->targetStage);
	cJSON_AddStringToObject(root, "targetFlag",t->targetFlag);
	cJSON_AddNumberToObject(root, "w", t->w);
	cJSON_AddNumberToObject(root, "h", t->h);
}
