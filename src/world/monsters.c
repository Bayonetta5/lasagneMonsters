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
static void climbStairs(void);
static int canWalkOnEntity(void);

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
	else if (m->aiFlags & AIF_CLIMB_STAIRS)
	{
		climbStairs();
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

		if (stage->map[mx][my] < TILE_WALL || stage->map[mx][my] >= TILE_SLIME)
		{
			if (!canWalkOnEntity())
			{
				self->dx = 0;
			}
		}
	}
}

static int canWalkOnEntity(void)
{
	int x, y, i, x1, y1, x2, y2;
	Entity *candidates[MAX_QT_CANDIDATES], *e;

	if (self->dx < 0)
	{
		x = self->x - 1;
	}
	else
	{
		x = self->x + self->w;
	}

	y = self->y;

	x1 = x;
	y1 = y;
	x2 = x;
	y2 = y + self->h + 8;

	getAllEntsWithin(x1, y1, x2, y2, candidates, self);

	for (i = 0, e = candidates[0] ; i < MAX_QT_CANDIDATES && e != NULL ; e = candidates[++i])
	{
		if (lineRectCollision(x1, y1, x2, y2, e->x, e->y, e->w, e->h))
		{
			if (e->flags & EF_SOLID)
			{
				return 1;
			}
		}
	}

	return 0;
}

static void climbStairs(void)
{
	int x, y, speed;

	if (self->isOnGround && self->dx != 0)
	{
		speed = 4 * abs(self->dx);

		if (self->dx < 0)
		{
			x = (self->x - speed) / TILE_SIZE;
		}
		else
		{
			x = (self->x + self->w + speed) / TILE_SIZE;
		}

		y = (self->y + self->h - 1) / TILE_SIZE;

		/* jump if a wall in front and space above wall is empty */
		if (isInsideMap(x, y) && stage->map[x][y] >= TILE_WALL && stage->map[x][y] < TILE_FOREGROUND && isInsideMap(x, y - 1) && stage->map[x][y - 1] == 0)
		{
			self->dy = -20;
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

void monsterDrawLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->cy - world.camera.y, MAX(self->w, self->h) / 2, 192, 192, 192, 255);
}

void monsterTakeDamage(int amount, int damageType)
{
	Monster *m;

	if (damageType == DT_WATER)
	{
		m = (Monster*)self->data;

		m->health -= amount;
		m->hitTimer = 255;
		m->alert = 1;

		if (m->health <= 0)
		{
			self->alive = ALIVE_DEAD;
		}
	}
}

void monsterDie(void)
{
	Monster *m;
	Walter *w;
	int playerHealth;

	m = (Monster*)self->data;
	w = (Walter*)world.player->data;

	playPositionalSound(SND_MONSTER_DIE, -1, self->x, self->y, world.player->x, world.player->y);

	throwPusBalls(self->x, self->y, 8);

	throwCoins(self->x, self->y, m->coins);

	playerHealth = ((1.0 * w->health) / w->maxHealth) * 10;

	if (rand() % 10 > playerHealth - 1)
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

		world.player->damage(1, DT_SLIME);

		self = oldSelf;
	}
}

void monsterSave(cJSON *root)
{
	cJSON_AddNumberToObject(root, "isMonster", 1);
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
