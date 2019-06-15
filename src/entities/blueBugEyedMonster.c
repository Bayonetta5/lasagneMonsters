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

#include "blueBugEyedMonster.h"

static void tick(void);
static void preAttack(void);
static void stand(void);
static void chasePlayer(void);
static void fireShots(void);
static void patrol(void);
static void die(void);

static AtlasImage *textures[2] = {NULL};
static void (*superDie)(void);

void initBlueBugEyedMonster(Entity *e)
{
	Monster *m;

	initMonster(e);

	m = (Monster*)e->data;

	m->health = m->maxHealth = 8;
	m->coins = 5;
	m->aiFlags = AIF_HALT_AT_EDGE;
	m->minShotsToFire = 2;
	m->maxShotsToFire = 5;
	m->reloadTime = FPS / 4;

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/entities/blueBugEyedMonster1.png", 1);
		textures[1] = getAtlasImage("gfx/entities/blueBugEyedMonster2.png", 1);
	}

	e->typeName = "blueBugEyedMonster";
	e->atlasImage = textures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->flags = EF_PUSHABLE;

	superDie = e->die;
	e->die = die;
}

static void tick(void)
{
	Monster *m;

	m = (Monster*)self->data;

	lookForPlayer();

	if (m->alert)
	{
		self->facing = self->x < world.player->x ? FACING_RIGHT : FACING_LEFT;

		preAttack();

		m->alert = 0;
	}
	else
	{
		patrol();
	}

	monsterTick();
}

static void preAttack(void)
{
	Monster *m;

	m = (Monster*)self->data;

	m->thinkTime = 0;

	m->shotsToFire = 0;

	switch (rand() % 3)
	{
		case 1:
			if (abs(self->y - world.player->y) <= 16)
			{
				m->shotsToFire = rrnd(m->minShotsToFire, m->maxShotsToFire);
			}
			self->tick = stand;
			break;

		case 2:
			self->tick = chasePlayer;
			break;

		default:
			if (abs(self->y - world.player->y) <= 16)
			{
				m->shotsToFire = rrnd(m->minShotsToFire, m->maxShotsToFire);
			}
			self->tick = chasePlayer;
			break;
	}
}

static void chasePlayer(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->thinkTime == 0)
	{
		self->dx = 0;

		m->thinkTime = FPS;
	}
	else
	{
		if (self->facing == FACING_LEFT)
		{
			self->dx = -RUN_SPEED;
		}
		else
		{
			self->dx = RUN_SPEED;
		}

		fireShots();
	}

	monsterTick();
}

static void stand(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->thinkTime == 0)
	{
		self->dx = 0;

		m->thinkTime = FPS;
	}
	else
	{
		fireShots();
	}

	monsterTick();
}

static void fireShots(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->shotsToFire > 0)
	{
		if (m->reload == 0)
		{
			self->atlasImage = textures[1];

			initSlimeBullet(self);

			m->shotsToFire--;

			m->reload = m->reloadTime;

			playPositionalSound(SND_SLIME_SHOOT, -1, self->x, self->y, world.player->x, world.player->y);
		}
	}
	else if (--m->thinkTime <= 0)
	{
		self->atlasImage = textures[0];

		self->tick = tick;
	}
}

static void patrol(void)
{
	Monster *m;

	m = (Monster*)self->data;

	/* blocked - turn around */
	if (self->dx == 0)
	{
		if (m->thinkTime == 0)
		{
			m->thinkTime = FPS;
		}
		else if (--m->thinkTime <= 0)
		{
			self->facing = !self->facing;

			if (self->facing == FACING_LEFT)
			{
				self->dx = -WALK_SPEED;
			}
			else
			{
				self->dx = WALK_SPEED;
			}
		}
	}
}

static void die(void)
{
	int i, n;
	Entity *e;

	n = 3 + rand() % 5;

	superDie();

	for (i = 0 ; i < n ; i++)
	{
		e = spawnEntity();

		initBabyBugEyedMonster(e);

		e->x = self->x;
		e->y = self->y;
		e->dx = rrnd(-100, 100);
		e->dy = rrnd(-240, -120);

		e->dx /= 100;
		e->dy /= 100;

		e->facing = e->dx < 0 ? FACING_LEFT : FACING_RIGHT;
	}
}
