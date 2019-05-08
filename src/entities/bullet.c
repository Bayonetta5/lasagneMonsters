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

#include "bullet.h"

static Entity *initBullet(Entity *owner);
static void waterBulletDie(void);
static void slimeBulletDie(void);

static AtlasImage *waterBulletTexture;
static AtlasImage *slimeBulletTexture;
static AtlasImage *aimedSlimeBulletTexture;

void initBullets(void)
{
	waterBulletTexture = getAtlasImage("gfx/entities/waterBullet.png", 1);
	slimeBulletTexture = getAtlasImage("gfx/entities/slimeBullet.png", 1);
	aimedSlimeBulletTexture = getAtlasImage("gfx/entities/aimedSlimeBullet1.png", 1);
}

void initWaterBullet(Entity *owner)
{
	Entity *e;

	e = initBullet(owner);

	e->atlasImage = waterBulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->die = waterBulletDie;

	e->x = owner->x;
	e->y = owner->y;
	e->y += (e->h / 2);

	if (e->facing == FACING_RIGHT)
	{
		e->x += self->w;
	}

	((Bullet*)e->data)->health = FPS;
}

void initSlimeBullet(Entity *owner)
{
	Entity *e;

	e = initBullet(owner);

	e->atlasImage = slimeBulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->die = slimeBulletDie;

	e->x = owner->x;
	e->y = owner->y;
	e->y += owner->h / 2;
	e->y -= e->h / 2;

	if (e->facing == FACING_RIGHT)
	{
		e->x += self->w;
	}
}

void initAimedSlimeBullet(Entity *owner, Entity *target)
{
	Entity *e;

	e = initBullet(owner);

	e->atlasImage = aimedSlimeBulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->die = slimeBulletDie;

	e->x = owner->x + (owner->w / 2) - (e->w / 2);
	e->y = owner->y + (owner->h / 2) - (e->h / 2);

	calcSlope(target->x, target->y, e->x, e->y, &e->dx, &e->dy);

	e->dx *= 8;
	e->dy *= 8;
}

static void tick(void)
{
	Bullet *b;

	b = (Bullet*)self->data;

	if (--b->health <= 0)
	{
		self->alive = ALIVE_DEAD;
		self->die = NULL;
	}
}

static void damage(int amount)
{
	self->alive = ALIVE_DEAD;
}

static void touch(Entity *other)
{
	Entity *oldSelf;

	if (self->alive == ALIVE_ALIVE)
	{
		if (other != NULL)
		{
			if (other->damage && other != self->owner)
			{
				oldSelf = self;

				self = other;

				other->damage(1);

				self = oldSelf;

				self->alive = ALIVE_DEAD;

			}
			else if (other->flags & EF_SOLID)
			{
				self->alive = ALIVE_DEAD;
			}
		}
		else
		{
			self->alive = ALIVE_DEAD;
		}
	}
}

static Entity *initBullet(Entity *owner)
{
	Entity *e;
	Bullet *b;

	b = malloc(sizeof(Bullet));
	memset(b, 0, sizeof(Bullet));

	b->health = FPS * 60;

	e = spawnEntity();

	e->data = b;
	e->type = ET_BULLET;
	e->typeName = "bullet";
	e->facing = owner->facing;
	e->dx = owner->facing == FACING_RIGHT ? 12 : -12;
	e->flags = EF_WEIGHTLESS+EF_NO_MAP_BOUNDS+EF_DELETE+EF_TRANSIENT;
	e->owner = self;

	e->tick = tick;
	e->damage = damage;
	e->touch = touch;

	return e;
}

static void waterBulletDie(void)
{
	playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, world.player->x, world.player->y);

	addWaterBurstParticles(self->x + (self->w / 2), self->y + (self->h / 2));
}

static void slimeBulletDie(void)
{
	playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, world.player->x, world.player->y);

	addSlimeBurstParticles(self->x + (self->w / 2), self->y + (self->h / 2));
}
