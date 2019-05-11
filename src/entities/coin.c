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

#include "coin.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *coinTexture = NULL;

void initCoin(Entity *e)
{
	Item *i;

	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));

	i->health = FPS * 10;
	i->touchDelay = FPS / 4;

	e->typeName = "coin";
	e->type = ET_ITEM;
	e->data = i;
	e->flags = EF_DELETE+EF_FRICTION+EF_TRANSIENT;
	e->tick = tick;
	e->draw = draw;

	if (coinTexture == NULL)
	{
		coinTexture = getAtlasImage("gfx/entities/coin.png", 1);
	}

	e->atlasImage = coinTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Item *i;

	i = (Item*)self->data;

	if (--i->touchDelay <= 0)
	{
		self->touch = touch;
	}

	if (--i->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

static void draw(void)
{
	Item *i;

	i = (Item*)self->data;

	drawObjectGlow(255, 255, 0, 64);

	if (i->health > FPS || (i->health < FPS && i->health % 5 == 0))
	{
		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);
	}
}

static void touch(Entity *other)
{
	if (self->alive == ALIVE_ALIVE && other == world.player)
	{
		self->alive = ALIVE_DEAD;

		playPositionalSound(SND_COIN, CH_ITEM, self->x, self->y, world.player->x, world.player->y);

		addGameText(self->x, self->y, "+1g");

		addCoinParticles(self->cx, self->cy);

		game.coins++;
	}
}
