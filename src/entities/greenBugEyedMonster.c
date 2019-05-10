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

#include "greenBugEyedMonster.h"

static void tick(void);
static void preAttack(void);
static void stand(void);
static void chasePlayer(void);
static void fireShots(void);
static void patrol(void);

static AtlasImage *textures[2] = {NULL};

void initGreenBugEyedMonster(Entity *e)
{
	Monster *m;

	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));

	m->health = m->maxHealth = 20;
	m->coins = 5;
	m->aiFlags = AIF_HALT_AT_EDGE;

	if (textures[0] == NULL)
	{
		textures[0] = getAtlasImage("gfx/entities/greenBugEyedMonster1.png", 1);
		textures[1] = getAtlasImage("gfx/entities/greenBugEyedMonster2.png", 1);
	}

	e->typeName = "greenBugEyedMonster";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = textures[0];
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

	switch (rand() % 4)
	{
		case 1:
			if (abs(self->y - world.player->y) <= 16)
			{
				m->shotsToFire = 1 + rand() % 2;
			}
			self->tick = stand;
			break;

		case 2:
			self->tick = chasePlayer;
			break;

		case 3:
			if (abs(self->y - world.player->y) <= 16)
			{
				m->shotsToFire = 1 + rand() % 2;
			}
			self->tick = chasePlayer;
			break;

		default:
			self->tick = stand;
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

			m->reload = FPS / 2;

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
