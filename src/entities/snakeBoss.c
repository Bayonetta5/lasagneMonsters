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
static void draw(void);
static void die(void);
static void preFire(void);
static void fireShots(void);
static void damage(int amount, int type);
static void touch(Entity *other);
static void initBodyParts(Entity *e);
static void updateBodyParts(void);

static Entity *head;
static Entity *bodyPart[MAX_BODY_PARTS];
static Entity *leftFlag, *rightFlag, *originFlag;
static AtlasImage *straightAtlasImage, *aimedAtlasImage;
static int deathCounter;
static int killPart;
static int thinkTime;
static int shotsToFire;
static int reload;
static int shotType;
static int hitTimer;
static int strikeDistance;

/* This boss is hard-coded to the layout of stage 14. */
void initSnakeBoss(Entity *e)
{
	Boss *b;

	b = malloc(sizeof(Boss));
	memset(b, 0, sizeof(Boss));

	b->health = b->maxHealth = 250;

	e->typeName = "snakeBoss";
	e->background = 1;
	e->type = ET_MONSTER;
	e->data = b;
	e->flags = EF_NO_WORLD_CLIP+EF_WEIGHTLESS+EF_NO_ENT_CLIP;
	e->init = init;
	e->tick = tick;
	e->draw = draw;
	e->damage = damage;
	e->touch = touch;

	straightAtlasImage = getAtlasImage("gfx/entities/snakeBossHead1.png", 1);
	aimedAtlasImage = getAtlasImage("gfx/entities/snakeBossHead2.png", 1);

	e->atlasImage = straightAtlasImage;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	head = e;

	thinkTime = 0;
	shotsToFire = 0;
	reload = 0;
	shotType = SHOT_TYPE_STRAIGHT;
	deathCounter = 0;
	hitTimer = 0;
	killPart = MAX_BODY_PARTS;
}

static void tick(void)
{
	if (rand() % 4 == 0)
	{
		self->flags |= EF_NO_WORLD_CLIP;

		self->tick = retract;
	}
	else
	{
		thinkTime = FPS * rrnd(1, 3);

		self->tick = chase;
	}

	preFire();

	updateBodyParts();
}

static void chase(void)
{
	int distance;

	distance = getDistance(self->x, self->y, world.player->x, world.player->y);

	if (distance > strikeDistance)
	{
		calcSlope(world.player->x, world.player->y, self->x, self->y, &self->dx, &self->dy);
	}
	else
	{
		calcSlope(self->x, self->y, world.player->x, world.player->y, &self->dx, &self->dy);
	}

	updateBodyParts();

	fireShots();

	if (--thinkTime <= 0)
	{
		self->tick = tick;
	}
}

static void retract(void)
{
	calcSlope(originFlag->x, originFlag->y, self->x, self->y, &self->dx, &self->dy);

	self->dx *= 5;
	self->dy *= 5;

	updateBodyParts();

	fireShots();

	/* swap side */
	if (getDistance(self->x, self->y, originFlag->x, originFlag->y) <= 5)
	{
		if (rand() % 3 >= 0)
		{
			if (originFlag == leftFlag)
			{
				originFlag = rightFlag;
			}
			else
			{
				originFlag = leftFlag;
			}
		}

		self->x = originFlag->x;
		self->y = originFlag->y;

		thinkTime = rrnd(FPS, FPS * 2);

		self->tick = emerge;

		/* select shot type */
		if (rand() % 2)
		{
			shotType = SHOT_TYPE_STRAIGHT;
			self->atlasImage = straightAtlasImage;
		}
		else
		{
			shotType = SHOT_TYPE_AIMED;
			self->atlasImage = aimedAtlasImage;
		}

	}
}

static void emerge(void)
{
	self->dx = 0;
	self->dy = -5;

	updateBodyParts();

	fireShots();

	if (--thinkTime <= 0)
	{
		self->flags &= ~EF_NO_WORLD_CLIP;

		self->tick = tick;

		strikeDistance = rrnd(100, 350);
	}
}

static void damage(int amount, int type)
{
	Boss *b;

	b = (Boss*)self->data;

	if (b->health > 0 && type == DT_WATER)
	{
		hitTimer = 255;

		amount *= 5;

		b->health = MAX(b->health - amount, 0);

		if (b->health == 0)
		{
			self->tick = killBoss;
		}
	}
}

static void preFire(void)
{
	if (rand() % 2 == 0)
	{
		shotsToFire = rrnd(3, 8);
	}
}

static void fireShots(void)
{
	if (shotsToFire > 0 && --reload <= 0)
	{
		switch (shotType)
		{
			case SHOT_TYPE_AIMED:
				initAimedSlimeBullet(self, world.player);
				break;

			default:
				initSlimeBullet(self);
				break;
		}

		playPositionalSound(SND_SLIME_SHOOT, -1, self->x, self->y, world.player->x, world.player->y);

		shotsToFire--;

		reload = FPS / 4;
	}
}

static void killBoss(void)
{
	hitTimer = 0;

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
			self->alive = ALIVE_DEAD;

			activateEntities("bossDoor", 1);
		}

		killPart--;
	}

	deathCounter++;
}

static void draw(void)
{
	if (hitTimer == 0)
	{
		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
	else
	{
		SDL_SetTextureColorMod(self->atlasImage->texture, 255, 255 - hitTimer, 255 - hitTimer);

		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

		SDL_SetTextureColorMod(self->atlasImage->texture, 255, 255, 255);
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
	float x, y, dx, dy, ox, oy;

	ox = head->x + (head->facing ? 64 : -16);
	oy = head->y - 8;

	calcSlope(ox, oy, originFlag->x, originFlag->y, &dx, &dy);

	sx = abs(ox - originFlag->x);
	sy = abs(oy - originFlag->y);

	steps = MAX(sx, sy);

	steps /= MAX_BODY_PARTS;

	dx *= steps;
	dy *= steps;

	x = originFlag->x;
	y = originFlag->y;

	for (i = 0 ; i < MAX_BODY_PARTS ; i++)
	{
		removeFromQuadtree(bodyPart[i], &world.quadtree);

		bodyPart[i]->x = x;
		bodyPart[i]->y = y;

		addToQuadtree(bodyPart[i], &world.quadtree);

		x += dx;
		y += dy;
	}

	/* always face player */
	self->facing = self->x < world.player->x ? FACING_RIGHT : FACING_LEFT;

	hitTimer = MAX(hitTimer - 32, 0);
}

static void die(void)
{
	playPositionalSound(SND_MONSTER_DIE, -1, self->cx, self->cy, world.player->x, world.player->y);

	throwPusBalls(self->cx, self->cy, 8);
}

static void damageBody(int amount, int type)
{
	if (type == DT_WATER)
	{
		hitTimer = 255;

		world.boss->health = MAX(world.boss->health - amount, 1);

		if (rand() % 3 == 0)
		{
			throwCoins(self->cx, self->cy, 1);
		}
	}
}

static void initBodyParts(Entity *owner)
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
		e->draw = draw;
		e->touch = touch;
		e->die = die;

		e->owner = owner;

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

	initBodyParts(head);

	self->x = originFlag->x;
	self->y = originFlag->y;

	self->tick = emerge;

	thinkTime = FPS * 1.5;
}
