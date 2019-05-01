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
static void touch(Entity *other);
static void damage(int amount);
static void die(void);

void initGreenHorse(Entity *e)
{
	Monster *m;
	
	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));
	
	m->health = m->maxHealth = 5;
	
	e->typeName = "greenHorse";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = getAtlasImage("gfx/entities/greenHorse1.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->touch = touch;
	e->damage = damage;
	e->die = die;
	
	stage.numMonsters = ++stage.totalMonsters;
}

static void tick(void)
{
	Monster *m;
	
	m = (Monster*)self->data;
	
	if (--m->thinkTime <= 0)
	{
		switch (rand() % 3)
		{
			case 0:
				self->dx = 0;
				m->thinkTime = FPS * (1 + (rand() % 3));
				break;
				
			default:
				self->dx = rand() % 2 == 0 ? - WALK_SPEED : WALK_SPEED;
				m->thinkTime = FPS * (1 + (rand() % 2));
				break;
		}
	}
	
	if (self->dx < 0)
	{
		self->facing = 0;
	}
	else if (self->dx > 0)
	{
		self->facing = 1;
	}
	
	haltAtEdge();
}

static void touch(Entity *other)
{
	Entity *oldSelf;
	
	if (other == stage.player)
	{
		oldSelf = self;
		
		self = other;
		
		stage.player->damage(1);
		
		self = oldSelf;
	}
}

static void damage(int amount)
{
	Monster *m;
	
	m = (Monster*)self->data;
	
	m->health -= amount;
	
	if (m->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

static void die(void)
{
	throwPusBalls(self->x, self->y, 8);
	
	throwCoins(self->x, self->y, 1 + rand() % 2);
	
	if (rand() % 5 == 0)
	{
		spawnRandomHealthItem(self->x, self->y);
	}
	
	stage.numMonsters--;
}
