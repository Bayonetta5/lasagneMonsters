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

#include "snakeBoss.h"

static void init(void);
static void chase(void);
static void tick(void);
static void damage(int amount, int type);
static void touch(Entity *other);
static void initBodyParts(void);
static void updateBodyParts(void);

static Entity *head;
static Entity *bodyPart[MAX_BODY_PARTS];
static SDL_Point origin;

/* This boss is hard-coded to the layout of stage 14. */
void initSnakeBoss(Entity *e)
{
	Boss *b;

	b = malloc(sizeof(Boss));
	memset(b, 0, sizeof(Boss));

	b->health = b->maxHealth = 100;

	e->typeName = "snakeBoss";
	e->background = 1;
	e->type = ET_ITEM;
	e->data = b;
	e->flags = EF_WEIGHTLESS+EF_NO_WORLD_CLIP+EF_NO_ENT_CLIP;
	e->init = init;
	e->tick = tick;
	e->damage = damage;
	e->touch = touch;

	e->atlasImage = getAtlasImage("gfx/entities/snakeBossHead.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	head = e;

	if (!app.dev.editor)
	{
		initBodyParts();
	}

	origin.x = 1104;
	origin.y = 768;
}

static void tick(void)
{
	chase();

	updateBodyParts();
}

static void chase(void)
{
	int distance;

	distance = getDistance(self->x, self->y, world.player->x, world.player->y);

	if (distance > 350)
	{
		calcSlope(world.player->x, world.player->y, self->x, self->y, &self->dx, &self->dy);
	}
	else
	{
		calcSlope(self->x, self->y, world.player->x, world.player->y, &self->dx, &self->dy);
	}

	updateBodyParts();
}

static void damage(int amount, int type)
{
	Boss *b;

	if (type == DT_WATER)
	{
		b = (Boss*)self->data;

		b->health = MAX(b->health - amount, 0);
	}
}

static void touch(Entity *other)
{
	Entity *oldSelf;

	if (other == world.player)
	{
		oldSelf = self;

		self = other;

		world.player->damage(2, DT_SLIME);

		self = oldSelf;
	}
}

static void updateBodyParts(void)
{
	int i, sx, sy, steps;
	float x, y, dx, dy;

	calcSlope(head->x, head->y, origin.x, origin.y, &dx, &dy);

	sx = abs(head->x - origin.x);
	sy = abs(head->y - origin.y);

	steps = MAX(sx, sy);

	steps /= MAX_BODY_PARTS;

	dx *= steps;
	dy *= steps;

	x = origin.x;
	y = origin.y;

	for (i = 0 ; i < MAX_BODY_PARTS ; i++)
	{
		bodyPart[i]->x = x;
		bodyPart[i]->y = y;

		x += dx;
		y += dy;
	}
}

static void damageBody(int amount, int type)
{
	if (type == DT_WATER && rand() % 3 == 0)
	{
		throwCoins(self->x, self->y, 1);
	}
}

static void initBodyParts(void)
{
	int i;
	Entity *e;

	for (i = 0 ; i < MAX_BODY_PARTS ; i++)
	{
		e = spawnEntity();
		e->x = head->x;
		e->y = head->y;
		e->background = 1;
		e->damage = damageBody;
		e->touch = touch;

		e->flags = EF_WEIGHTLESS+EF_NO_WORLD_CLIP+EF_STATIC;

		e->atlasImage = getAtlasImage("gfx/entities/snakeBossBody.png", 1);
		e->w = e->atlasImage->rect.w;
		e->h = e->atlasImage->rect.h;

		bodyPart[i] = e;
	}
}

static void init(void)
{
	world.boss = (Boss*)self->data;

	loadMusic("music/MonsterVania - Ghost Land.mp3");

	playMusic(1);
}
