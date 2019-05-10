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

#include "greenHorse.h"

static void tick(void);
static void patrol(void);
static void preCharge(void);
static void charge(void);

void initGreenHorse(Entity *e)
{
	Monster *m;

	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));

	m->health = m->maxHealth = 5;
	m->coins = 1;
	m->aiFlags = AIF_HALT_AT_EDGE;

	e->typeName = "greenHorse";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = getAtlasImage("gfx/entities/greenHorse1.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->draw = monsterDraw;
	e->drawLight = monsterDrawLight;
	e->touch = monsterTouch;
	e->damage = monsterTakeDamage;
	e->die = monsterDie;

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

	if (abs(self->y - world.player->y) <= 16)
	{
		self->dx = 0;
		m->thinkTime = FPS;
		self->tick = charge;
	}
	else
	{
		self->tick = tick;
	}
}

static void charge(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->thinkTime > 0)
	{
		if (--m->thinkTime <= 0)
		{
			if (self->facing == FACING_LEFT)
			{
				self->dx = -RUN_SPEED;
			}
			else
			{
				self->dx = RUN_SPEED;
			}
		}
	}
	else if (self->dx == 0)
	{
		self->tick = tick;
	}

	monsterTick();
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
