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

#include "heart.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *heartTextures[2] = {NULL};
static AtlasImage *sparkleTexture;

void initHeart(Entity *e)
{
	Item *h;

	h = malloc(sizeof(Item));
	memset(h, 0, sizeof(Item));

	e->typeName = "heart";
	e->type = ET_ITEM;
	e->data = h;
	e->flags = EF_DELETE;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;

	if (heartTextures[0] == NULL)
	{
		heartTextures[0] = getAtlasImage("gfx/entities/healthUp1.png", 1);
		heartTextures[1] = getAtlasImage("gfx/entities/healthUp2.png", 1);
		sparkleTexture = getAtlasImage("gfx/particles/light.png", 1);
	}

	e->atlasImage = heartTextures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Item *h;

	h = (Item*)self->data;

	if (self->isOnGround && --h->thinkTime <= 0)
	{
		self->dy = -12;

		h->thinkTime = FPS * (1 + rand() % 8);
	}

	if (--h->frameTime <= 0)
	{
		if (++h->frame > 1)
		{
			h->frame = 0;
		}

		self->atlasImage = heartTextures[h->frame];

		h->frameTime = FPS / 3;
	}
}

static void draw(void)
{
	int x, y;

	x = self->x + (self->w / 2) - world.camera.x;
	y = self->y + (self->h / 2) - world.camera.y;

	SDL_SetTextureColorMod(sparkleTexture->texture, 255, 0, 0);
	SDL_SetTextureAlphaMod(sparkleTexture->texture, 64);

	blitAtlasImage(sparkleTexture, x, y, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(sparkleTexture->texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(sparkleTexture->texture, 255);

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	Walter *w;

	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		w = (Walter*)other->data;

		w->health = ++w->maxHealth;

		self->alive = ALIVE_DEAD;

		addPowerupParticles(self->x + self->w / 2, self->y + self->h / 2);

		addGameText(self->x, self->y, "Health Up!");

		playSound(SND_POWERUP, -1);
	}
}
