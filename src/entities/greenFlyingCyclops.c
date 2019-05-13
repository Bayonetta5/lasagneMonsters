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

#include "greenFlyingCyclops.h"

static void tick(void);
static void patrol(void);
static void wander(void);
static void hover(void);
static void preCharge(void);
static void chasePlayer(void);

void initGreenFlyingCyclops(Entity *e)
{
	Monster *m;

	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));

	m->health = m->maxHealth = 5;
	m->coins = 2;

	e->typeName = "greenFlyingCyclops";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = getAtlasImage("gfx/entities/greenFlyingCyclops1.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_PUSHABLE+EF_WEIGHTLESS;

	e->tick = tick;
	e->draw = monsterDraw;
	e->drawLight = monsterDrawLight;
	e->touch = monsterTouch;
	e->damage = monsterTakeDamage;
	e->die = monsterDie;
	e->save = monsterSave;

	stage->numMonsters++;
}

static void tick(void)
{
	Monster *m;

	m = (Monster*)self->data;

	lookForPlayer();

	if (m->alert)
	{
		self->facing = self->x < world.player->x ? FACING_RIGHT : FACING_LEFT;

		preCharge();

		m->alert = 0;
	}
	else
	{
		patrol();
	}

	monsterTick();
}

static void preCharge(void)
{
	Monster *m;

	m = (Monster*)self->data;

	m->thinkTime = 0;

	m->shotsToFire = 0;

	switch (rand() % 4)
	{
		case 1:
			m->shotsToFire = 1 + rand() % 2;
			self->tick = hover;
			break;

		case 2:
			self->tick = chasePlayer;
			break;

		case 3:
			m->shotsToFire = 1 + rand() % 2;
			self->tick = chasePlayer;
			break;

		default:
			self->tick = hover;
			break;
	}
}

static void chasePlayer(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->thinkTime == 0)
	{
		calcSlope(world.player->x, world.player->y, self->x, self->y, &self->dx, &self->dy);

		self->dx *= 2;
		self->dy *= 2;

		m->thinkTime = FPS;

		monsterFaceMoveDir();
	}
	else
	{
		if (m->shotsToFire > 0)
		{
			if (m->reload == 0)
			{
				initAimedSlimeBullet(self, world.player);

				monsterFaceTarget(world.player);

				m->shotsToFire--;

				m->reload = FPS / 2;

				playPositionalSound(SND_SLIME_SHOOT, -1, self->x, self->y, world.player->x, world.player->y);
			}
		}
		else if (--m->thinkTime <= 0)
		{
			self->tick = tick;
		}
	}

	monsterTick();
}

static void hover(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->thinkTime == 0)
	{
		self->dx = self->dy = 0;

		m->thinkTime = FPS;
	}
	else
	{
		if (m->shotsToFire > 0)
		{
			if (m->reload == 0)
			{
				initAimedSlimeBullet(self, world.player);

				m->shotsToFire--;

				m->reload = FPS / 2;

				playPositionalSound(SND_SLIME_SHOOT, -1, self->x, self->y, world.player->x, world.player->y);
			}
		}
		else if (--m->thinkTime <= 0)
		{
			self->tick = tick;
		}
	}

	monsterTick();
}

static void patrol(void)
{
	Monster *m;

	m = (Monster*)self->data;

	/* blocked */
	if (self->dx == 0 || self->dy == 0 || m->thinkTime <= 0)
	{
		m->thinkTime = 0;

		if (rand() % 5 <= 2)
		{
			self->tick = hover;
		}
		else
		{
			self->tick = wander;
		}
	}
}

static void wander(void)
{
	Monster *m;
	int tx, ty;

	m = (Monster*)self->data;

	if (m->thinkTime == 0)
	{
		tx = self->x + rand() % 256 - rand() % 256;
		ty = self->y + rand() % 256 - rand() % 256;

		calcSlope(tx, ty, self->x, self->y, &self->dx, &self->dy);

		self->dx *= 2;
		self->dy *= 2;

		monsterFaceMoveDir();

		m->thinkTime = FPS;
	}
	else if (--m->thinkTime <= 0)
	{
		self->tick = tick;
	}

	monsterTick();
}
