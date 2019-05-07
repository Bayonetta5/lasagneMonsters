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

#include "piggyBank.h"

static void initTextures(void);
static void tick(void);
static void draw(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *piggyBankTexture = NULL;
static AtlasImage *digitTextures[10];
static AtlasImage *arrowTextures[2];

void initPiggyBank(Entity *e)
{
	PiggyBank *p;

	if (piggyBankTexture == NULL)
	{
		initTextures();
	}

	p = malloc(sizeof(PiggyBank));
	memset(p, 0, sizeof(PiggyBank));

	e->typeName = "piggyBank";
	e->type = ET_STRUCTURE;
	e->data = p;
	e->atlasImage = piggyBankTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
	e->flags = EF_NO_ENT_CLIP+EF_STATIC;

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	PiggyBank *p;

	p = (PiggyBank*)self->data;

	p->pauseTime = MAX(p->pauseTime - 1, 0);

	if (--p->frameTime <= 0)
	{
		p->frameTime = rand() % FPS;

		if (++p->frame >= 2)
		{
			p->frame = 0;
		}
	}
}

static void draw(void)
{
	int digits[3], x;
	PiggyBank *p;

	p = (PiggyBank*)self->data;

	digits[0] = (p->requiredCoins / 100);
	digits[1] = (p->requiredCoins / 10) % 10;
	digits[2] = (p->requiredCoins % 10);

	x = (self->x - world.camera.x + self->w);

	blitAtlasImage(digitTextures[digits[0]], x, (self->y - world.camera.y) - 48, 0, SDL_FLIP_NONE);
	blitAtlasImage(digitTextures[digits[1]], x + 24, (self->y - world.camera.y) - 48, 0, SDL_FLIP_NONE);
	blitAtlasImage(digitTextures[digits[2]], x + 48, (self->y - world.camera.y) - 48, 0, SDL_FLIP_NONE);

	blitAtlasImage(arrowTextures[p->frame], x - 40, (self->y - world.camera.y) - 36, 0, SDL_FLIP_NONE);

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	Walter *w;
	PiggyBank *p;

	p = (PiggyBank*)self->data;

	if (other == world.player)
	{
		w = (Walter*)other->data;

		if (w->action)
		{
			if (p->requiredCoins > 0 && game.coins > 0 && p->pauseTime == 0)
			{
				playPositionalSound(SND_CHEST, CH_PLAYER, self->x, self->y, world.player->x, world.player->y);

				game.coins--;

				if (--p->requiredCoins == 0)
				{
					playPositionalSound(SND_SQUEAK, CH_PLAYER, self->x, self->y, world.player->x, world.player->y);

					activeEntities(p->targetName, 1);
				}

				p->pauseTime = FPS / 8;
			}
		}
	}
}

static void initTextures(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	piggyBankTexture = getAtlasImage("gfx/entities/piggyBank.png", 1);

	arrowTextures[0] = getAtlasImage("gfx/entities/neonArrow1.png", 1);
	arrowTextures[1] = getAtlasImage("gfx/entities/neonArrow2.png", 1);

	for (i = 0 ; i < 10 ; i++)
	{
		sprintf(filename, "gfx/entities/digit%d.png", i);

		digitTextures[i] = getAtlasImage(filename, 1);
	}
}

static void load(cJSON *root)
{
	PiggyBank *p;

	p = (PiggyBank*)self->data;

	p->requiredCoins = cJSON_GetObjectItem(root, "requiredCoins")->valueint;
	STRNCPY(p->targetName, cJSON_GetObjectItem(root, "targetName")->valuestring, MAX_NAME_LENGTH);
}

static void save(cJSON *root)
{
	PiggyBank *p;

	p = (PiggyBank*)self->data;

	cJSON_AddNumberToObject(root, "requiredCoins", p->requiredCoins);
	cJSON_AddStringToObject(root, "targetName", p->targetName);
}
