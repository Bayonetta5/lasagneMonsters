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
static void damage(int amount);
static void touch(Entity *other);
static void die(void);
static void spit(void);

static AtlasImage *bulletTexture;

void initGreenBugEyedMonster(Entity *e)
{
	Monster *m;
	
	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));
	
	m->health = m->maxHealth = 10;
	
	e->typeName = "greenBugEyedMonster";
	e->type = ET_MONSTER;
	e->data = m;
	e->atlasImage = getAtlasImage("gfx/entities/greenBugEyedMonster1.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->touch = touch;
	e->tick = tick;
	e->damage = damage;
	e->die = die;
	
	bulletTexture = getAtlasImage("gfx/entities/slimeBullet.png", 1);
	
	stage.numMonsters = ++stage.totalMonsters;
}

static void tick(void)
{
	if (0)
	{
		preAttack();
	}
}

static void preAttack(void)
{
	if (0)
	{
		spit();
	}
}

/* === Slime bullets === */

static void bulletTouch(Entity *other)
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
				
				playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
			}
			else if (other->flags & EF_SOLID)
			{
				self->alive = ALIVE_DEAD;
				
				playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
			}
		}
		else
		{
			self->alive = ALIVE_DEAD;
			
			playPositionalSound(SND_WATER_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
		}
	}
}

static void bulletDie(void)
{
	addSlimeBurstParticles(self->x, self->y);
}

static void spit(void)
{
	Monster *m;
	Entity *e;
	
	m = (Monster*)self->data;
	
	m->reload = MAX(m->reload - 1, 0);
	
	if (m->reload == 0)
	{
		if (abs(self->y - stage.player->y) <= TILE_SIZE / 2)
		{
			e = spawnEntity();
		
			e->type = ET_BULLET;
			e->typeName = "bullet";
			e->x = self->x;
			e->y = self->y;
			e->facing = self->facing;
			e->dx = self->facing ? 12 : -12;
			e->flags = EF_WEIGHTLESS+EF_NO_MAP_BOUNDS+EF_DELETE;
			e->atlasImage = bulletTexture;
			e->w = e->atlasImage->rect.w;
			e->h = e->atlasImage->rect.h;
			e->touch = bulletTouch;
			e->die = bulletDie;
			e->owner = self;
			
			e->y += (e->h / 2);
			
			if (e->facing)
			{
				e->x += self->w;
			}
			
			m->reload = FPS / 2;
			
			if (--m->shotsToFire <= 0)
			{
				self->tick = tick;
			}
		}
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

static void touch(Entity *other)
{
	if (other == stage.player)
	{
		stage.player->damage(2);
	}
}

static void die(void)
{
	throwCoins(self->x, self->y, 1 + rand() % 2);
	
	stage.numMonsters--;
}
