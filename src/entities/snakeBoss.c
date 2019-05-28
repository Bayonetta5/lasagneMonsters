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
static void tick(void);
static void chase(void);
static void retract(void);
static void emerge(void);
static void killBoss(void);
static void die(void);
static void damage(int amount, int type);
static void touch(Entity *other);
static void initBodyParts(void);
static void updateBodyParts(void);

static Entity *head;
static Entity *bodyPart[MAX_BODY_PARTS];
static Entity *leftFlag, *rightFlag, *originFlag;
static int attackDistance;
static int deathCounter;
static int killPart;

/* This boss is hard-coded to the layout of stage 14. */
void initSnakeBoss(Entity *e)
{
	Boss *b;

	b = malloc(sizeof(Boss));
	memset(b, 0, sizeof(Boss));

	b->health = b->maxHealth = 1;

	e->typeName = "snakeBoss";
	e->background = 1;
	e->type = ET_MONSTER;
	e->data = b;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP;
	e->init = init;
	e->tick = tick;
	e->damage = damage;
	e->touch = touch;

	e->atlasImage = getAtlasImage("gfx/entities/snakeBossHead.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	head = e;

	deathCounter = 0;
	killPart = MAX_BODY_PARTS;

	if (!app.dev.editor)
	{
		initBodyParts();
	}
}

static void tick(void)
{
	Boss *b;

	b = (Boss*)self->data;

	if (rand() % 4 == 0)
	{
		self->flags |= EF_NO_WORLD_CLIP;

		self->tick = retract;
	}
	else
	{
		b->thinkTime = FPS * rrnd(1, 3);

		self->tick = chase;
	}

	updateBodyParts();
}

static void chase(void)
{
	int distance;
	Boss *b;

	b = (Boss*)self->data;

	distance = getDistance(self->x, self->y, world.player->x, world.player->y);

	if (distance > attackDistance)
	{
		calcSlope(world.player->x, world.player->y, self->x, self->y, &self->dx, &self->dy);
	}
	else
	{
		calcSlope(self->x, self->y, world.player->x, world.player->y, &self->dx, &self->dy);
	}

	updateBodyParts();

	if (--b->thinkTime <= 0)
	{
		self->tick = tick;
	}
}

static void retract(void)
{
	Boss *b;

	b = (Boss*)self->data;

	calcSlope(originFlag->x, originFlag->y, self->x, self->y, &self->dx, &self->dy);

	self->dx *= 5;
	self->dy *= 5;

	updateBodyParts();

	/* swap side */
	if (getDistance(self->x, self->y, originFlag->x, originFlag->y) <= 5)
	{
		if (originFlag == leftFlag)
		{
			originFlag = rightFlag;
		}
		else
		{
			originFlag = leftFlag;
		}

		self->x = originFlag->x;
		self->y = originFlag->y;

		b->thinkTime = rrnd(FPS * 1.5, FPS * 2.5);

		self->tick = emerge;
	}
}

static void emerge(void)
{
	Boss *b;

	b = (Boss*)self->data;

	self->dy = -5;

	updateBodyParts();

	if (--b->thinkTime <= 0)
	{
		self->flags &= ~EF_NO_WORLD_CLIP;

		attackDistance = rrnd(250, 500);

		self->tick = tick;
	}
}

static void damage(int amount, int type)
{
	Boss *b;

	b = (Boss*)self->data;

	if (b->health > 0 && type == DT_WATER)
	{
		b->health = MAX(b->health - amount, 0);

		if (b->health == 0)
		{
			self->tick = killBoss;
		}
	}
}

static void killBoss(void)
{
	if (deathCounter % (FPS / 4) == 0)
	{
		if (killPart == MAX_BODY_PARTS)
		{
			die();
			self->flags |= EF_INVISIBLE;
			self->touch = NULL;
		}
		else if (killPart >= 0)
		{
			bodyPart[killPart]->alive = ALIVE_DEAD;
		}
		else if (killPart == -5)
		{
			activateEntities("bossDoor", 1);
		}

		killPart--;
	}

	deathCounter++;
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

	calcSlope(head->x, head->y, originFlag->x, originFlag->y, &dx, &dy);

	sx = abs(head->x - originFlag->x);
	sy = abs(head->y - originFlag->y);

	steps = MAX(sx, sy);

	steps /= MAX_BODY_PARTS;

	dx *= steps;
	dy *= steps;

	x = originFlag->x;
	y = originFlag->y;

	for (i = 0 ; i < MAX_BODY_PARTS ; i++)
	{
		bodyPart[i]->x = x;
		bodyPart[i]->y = y;

		x += dx;
		y += dy;
	}

	/* always face player */
	self->facing = self->x < world.player->x ? FACING_RIGHT : FACING_LEFT;
}

static void die(void)
{
	playPositionalSound(SND_MONSTER_DIE, -1, self->x, self->y, world.player->x, world.player->y);

	throwPusBalls(self->x, self->y, 8);
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
		e->type = ET_STRUCTURE;
		e->x = head->x;
		e->y = head->y;
		e->background = 1;
		e->damage = damageBody;
		e->touch = touch;
		e->die = die;

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

	leftFlag = findStartPoint("bossLeft");
	rightFlag = findStartPoint("bossRight");
	originFlag = rightFlag;

	attackDistance = rrnd(250, 500);
}
