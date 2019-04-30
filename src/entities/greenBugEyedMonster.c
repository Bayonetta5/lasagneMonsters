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
static void damage(int amount);
static void die(void);

void initGreenBugEyedMonster(Entity *e)
{
	Monster *m;
	
	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));
	
	m->hp = m->maxHP = 25;
	
	e->typeName = "greenBugEyedMonster";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = getAtlasImage("gfx/entities/greenBugEyedMonster1.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->damage = damage;
	e->die = die;
}

static void tick(void)
{
}

static void damage(int amount)
{
	Monster *m;
	
	m = (Monster*)self->data;
	
	m->hp -= amount;
	
	if (m->hp <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

static void die(void)
{
}
