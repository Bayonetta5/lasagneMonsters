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

static void draw(void);
static void drawLight(void);
static void touch(Entity *other);
static void takeDamage(int amount, int damageType);
static void save(cJSON *root);
static void die(void);
static void haltAtEdge(void);
static void jump(void);
static int canWalkOnEntity(void);

void initMonster(Entity *e)
{
	Monster *m;

	m = malloc(sizeof(Monster));
	memset(m, 0, sizeof(Monster));

	m->touchDamage = 1;

	e->type = ET_MONSTER;
	e->data = m;
	e->draw = draw;
	e->drawLight = drawLight;
	e->touch = touch;
	e->damage = takeDamage;
	e->die = die;
	e->save = save;

	stage->numMonsters++;
}

void monsterTick(void)
{
	Monster *m;

	m = (Monster*)self->data;

	m->hitTimer = MAX(m->hitTimer - 16, 0);
	m->reload = MAX(m->reload - 1, 0);

	if (self->isOnGround && m->aiFlags & AIF_HALT_AT_EDGE)
	{
		haltAtEdge();
	}

	if (m->aiFlags & AIF_JUMPS)
	{
		jump();
	}
}

static void haltAtEdge(void)
{
	int mx, my, n;
	Monster *m;

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

		if (!isSolidMap(mx, my, &n))
		{
			m = (Monster*)self->data;

			if (canWalkOnEntity())
			{
				/* ok */
			}
			else if (m->aiFlags & AIF_JUMPS)
			{
				/* gap that can be jumped */
				if (isSolidMap(mx + (self->dx < 0 ? -1 : 1), my, &n))
				{
					self->dy = -(20 - abs(self->dx));
				}
				else if (isSolidMap(mx, my + 1, &n))
				{
					/* ok */
				}
				else
				{
					self->dx = 0;
				}
			}
			else
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

static void jump(void)
{
	int x, y, speed;

	if (self->isOnGround && self->dx != 0)
	{
		speed = 8 * abs(self->dx);

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

static void draw(void)
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

static void drawLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->cy - world.camera.y, MAX(self->w, self->h) / 2, 192, 192, 192, 255);
}

static void takeDamage(int amount, int damageType)
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

static void die(void)
{
	Monster *m;
	Walter *w;
	int playerHealth;

	m = (Monster*)self->data;
	w = (Walter*)world.player->data;

	playPositionalSound(SND_MONSTER_DIE, -1, self->x, self->y, world.player->x, world.player->y);

	throwPusBalls(self->x, self->y, 8);

	throwCoins(self->x, self->y, m->coins);

	playerHealth = ((1.0 * w->health) / w->maxHealth) * 25;

	if (rand() % 25 > playerHealth - 1)
	{
		spawnRandomHealthItem(self->x, self->y);
	}

	stage->numMonsters--;
}

static void touch(Entity *other)
{
	Entity *oldSelf;
	Monster *m;

	if (other == world.player)
	{
		m = (Monster*)self->data;

		oldSelf = self;

		self = other;

		world.player->damage(m->touchDamage, DT_SLIME);

		self = oldSelf;
	}
}

static void save(cJSON *root)
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
