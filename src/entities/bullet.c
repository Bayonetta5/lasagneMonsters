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

static Entity *initBullet(Entity *owner, int damage, int damageType);
static void waterBulletDie(void);
static void slimeBulletDie(void);
static void drawSlimeBulletLight(void);
static void tickBouncer(void);
static int isBulletOwner(Entity *other);

static AtlasImage *waterBulletTexture;
static AtlasImage *slimeBulletTexture;
static AtlasImage *aimedSlimeBulletTexture;
static AtlasImage *bouncerTexture;

void initBullets(void)
{
	waterBulletTexture = getAtlasImage("gfx/entities/waterBullet.png", 1);
	slimeBulletTexture = getAtlasImage("gfx/entities/slimeBullet.png", 1);
	aimedSlimeBulletTexture = getAtlasImage("gfx/entities/aimedSlimeBullet1.png", 1);
	bouncerTexture = getAtlasImage("gfx/entities/slimeBouncer1.png", 1);
}

void initWaterBullet(Entity *owner)
{
	Entity *e;

	e = initBullet(owner, 1, DT_WATER);

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

	e = initBullet(owner, 1, DT_SLIME);

	e->atlasImage = slimeBulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->drawLight = drawSlimeBulletLight;
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

	e = initBullet(owner, 1, DT_SLIME);

	e->atlasImage = aimedSlimeBulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->drawLight = drawSlimeBulletLight;
	e->die = slimeBulletDie;

	e->x = owner->cx - (e->w / 2);
	e->y = owner->cy - (e->h / 2);

	calcSlope(target->x, target->y, e->x, e->y, &e->dx, &e->dy);

	e->dx *= 8;
	e->dy *= 8;
}

void initSlimeBouncerBullet(Entity *owner, Entity *target)
{
	Entity *e;

	e = initBullet(owner, 1, DT_SLIME);

	e->atlasImage = bouncerTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->drawLight = drawSlimeBulletLight;
	e->die = slimeBulletDie;
	e->tick = tickBouncer;

	e->flags |= EF_BOUNCES;
	e->flags &= ~EF_WEIGHTLESS;

	((Bullet*)e)->health = FPS * 3;

	e->x = owner->x;
	e->y = owner->y;
	e->y += owner->h / 2;
	e->y -= e->h / 2;

	e->dy = -(12 + rand() % 9);

	if (e->facing == FACING_RIGHT)
	{
		e->dx = 6;

		e->x += self->w;
	}
	else
	{
		e->dx = -6;
	}
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

static void tickBouncer(void)
{
	Bullet *b;

	b = (Bullet*)self->data;

	if (--b->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

static void drawSlimeBulletLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->cy - world.camera.y, 16, 128, 255, 128, 255);
}

static void damage(int amount, int damageType)
{
	self->alive = ALIVE_DEAD;
}

static void touch(Entity *other)
{
	Entity *oldSelf;
	Bullet *b;

	if (self->alive == ALIVE_ALIVE)
	{
		if (other != NULL)
		{
			if (other->damage && !isBulletOwner(other))
			{
				b = (Bullet*)self->data;

				oldSelf = self;

				self = other;

				other->damage(b->damage, b->damageType);

				self = oldSelf;

				self->alive = ALIVE_DEAD;

				if (self->owner == world.player)
				{
					/* player's bullet hit another bullet */
					if (other->type == ET_BULLET)
					{
						game.stats[STAT_SHOTS_DESTROYED]++;
					}
					else
					{
						game.stats[STAT_SHOTS_HIT]++;
					}
				}
			}
			else if ((other->flags & EF_SOLID) && (!(self->flags & EF_BOUNCES)))
			{
				self->alive = ALIVE_DEAD;
			}
		}
		else if (!(self->flags & EF_BOUNCES))
		{
			self->alive = ALIVE_DEAD;
		}
	}
}

static int isBulletOwner(Entity *other)
{
	if (self->owner == other)
	{
		return 1;
	}
	else if (other != NULL)
	{
		return isBulletOwner(other->owner);
	}

	return 0;
}

static Entity *initBullet(Entity *owner, int amount, int damageType)
{
	Entity *e;
	Bullet *b;

	b = malloc(sizeof(Bullet));
	memset(b, 0, sizeof(Bullet));

	b->health = FPS * 60;
	b->damage = amount;
	b->damageType = damageType;

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

	addWaterBurstParticles(self->cx, self->cy);
}

static void slimeBulletDie(void)
{
	playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, world.player->x, world.player->y);

	addSlimeBurstParticles(self->cx, self->cy);
}
