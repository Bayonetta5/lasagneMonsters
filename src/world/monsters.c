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

#include "monsters.h"

static void haltAtEdge(void);

void monsterTick(void)
{
	Monster *m;

	m = (Monster*)self->data;

	m->hitTimer = MAX(m->hitTimer - 16, 0);
	m->reload = MAX(m->reload - 1, 0);

	if (m->aiFlags & AIF_HALT_AT_EDGE)
	{
		haltAtEdge();
	}
}

static void haltAtEdge(void)
{
	int mx, my;

	if (self->dx != 0)
	{
		my = (self->y + self->h) / TILE_SIZE;

		if (self->dx < 0)
		{
			mx = self->x - 1;
		}
		else
		{
			mx = self->x + self->w;
		}

		mx /= TILE_SIZE;

		if (stage->map[mx][my] == 0)
		{
			self->dx = 0;
		}
	}
}

void monsterDraw(void)
{
	Monster *m;

	m = (Monster*)self->data;

	if (m->hitTimer > 0)
	{
		SDL_SetTextureBlendMode(self->atlasImage->texture, SDL_BLENDMODE_ADD);
		SDL_SetTextureColorMod(self->atlasImage->texture, 255 - m->hitTimer, 128 - m->hitTimer, 255);

		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

		SDL_SetTextureColorMod(self->atlasImage->texture, 255, 255, 255);
		SDL_SetTextureBlendMode(self->atlasImage->texture, SDL_BLENDMODE_BLEND);
	}
	else
	{
		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
}

void monsterTakeDamage(int amount)
{
	Monster *m;

	m = (Monster*)self->data;

	m->health -= amount;
	m->hitTimer = 255;
	m->alert = 1;

	if (m->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

void monsterDie(void)
{
	Monster *m;

	m = (Monster*)self->data;

	playPositionalSound(SND_MONSTER_DIE, -1, self->x, self->y, world.player->x, world.player->y);

	throwPusBalls(self->x, self->y, 8);

	throwCoins(self->x, self->y, m->coins);

	if (rand() % 10 == 0)
	{
		spawnRandomHealthItem(self->x, self->y);
	}

	stage->numMonsters--;
}


void monsterTouch(Entity *other)
{
	Entity *oldSelf;

	if (other == world.player)
	{
		oldSelf = self;

		self = other;

		world.player->damage(1);

		self = oldSelf;
	}
}

void monsterFaceTarget(Entity *e)
{
	if (e->x < self->x)
	{
		self->facing = FACING_LEFT;
	}
	else if (e->x > self->x)
	{
		self->facing = FACING_RIGHT;
	}
}

void monsterFaceMoveDir(void)
{
	if (self->dx < 0)
	{
		self->facing = FACING_LEFT;
	}
	else if (self->dx > 0)
	{
		self->facing = FACING_RIGHT;
	}
}
