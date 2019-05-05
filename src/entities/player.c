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

#include "player.h"

static void tick(void);
static void draw(void);
static void damage(int amount);
static void die(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initPlayer(Entity *e)
{
	Walter *w;

	w = malloc(sizeof(Walter));
	memset(w, 0, sizeof(Walter));

	w->health = w->maxHealth = 3;
	w->ammo = w->maxAmmo = 5;

	e->typeName = "player";
	e->data = w;
	e->type = ET_PLAYER;
	e->atlasImage = getAtlasImage("gfx/entities/walter.png", 1);
	e->flags = EF_PUSH+EF_PUSHABLE+EF_SLOW_PUSH+EF_NO_MAP_BOUNDS;
	e->tick = tick;
	e->draw = draw;
	e->damage = damage;
	e->die = die;
	e->load = load;
	e->save = save;

	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Walter *w;

	w = (Walter*)self->data;

	self->dx = 0;
	w->action = 0;

	w->immuneTimer = MAX(w->immuneTimer - 1, 0);
	w->reload = MAX(w->reload - 1, 0);
	w->ammo = MIN(w->ammo + 0.05f, w->maxAmmo);

	if (self->alive == ALIVE_ALIVE)
	{
		if (isControl(CONTROL_LEFT))
		{
			self->dx = -PLAYER_MOVE_SPEED;

			self->facing = FACING_LEFT;
		}

		if (isControl(CONTROL_RIGHT))
		{
			self->dx = PLAYER_MOVE_SPEED;

			self->facing = FACING_RIGHT;
		}

		if (isControl(CONTROL_JUMP) && self->isOnGround)
		{
			self->riding = NULL;

			self->dy = -20;

			playSound(SND_JUMP, CH_PLAYER);
		}

		if (isControl(CONTROL_FIRE))
		{
			if (!game.autoFire)
			{
				clearControl(CONTROL_FIRE);
			}

			if (w->ammo > 0 && w->reload == 0)
			{
				w->ammo--;

				/* don't let the player fire too quickly */
				w->reload = game.autoFire;

				initWaterBullet(self);

				playPositionalSound(SND_SHOOT, CH_SHOOT, self->x, self->y, world.player->x, world.player->y);
			}
		}
		else
		{
			w->reload = 0;
		}

		if (isControl(CONTROL_USE))
		{
			clearControl(CONTROL_USE);

			w->action = 1;
		}
	}
}

static void draw(void)
{
	Walter *w;

	w = (Walter*)self->data;

	if (w->immuneTimer % 5 == 0)
	{
		blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
}

static void damage(int amount)
{
	Walter *w;

	w = (Walter*)self->data;

	if (w->immuneTimer == 0)
	{
		w->health -= amount;

		if (w->health <= 0)
		{
			self->alive = ALIVE_DEAD;
		}

		w->immuneTimer = FPS;

		playSound(SND_INJURED, -1);
	}
}

static void die(void)
{
	addDeathParticles(self->x, self->y);

	playSound(SND_DEATH, -1);

	playSound(SND_SCREAM, CH_PLAYER);
}

static void load(cJSON *root)
{
	self->facing = strcmp(cJSON_GetObjectItem(root, "facing")->valuestring, "left") == 0 ? FACING_LEFT : FACING_RIGHT;
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "facing", self->facing == FACING_LEFT ? "left" : "right");
}
