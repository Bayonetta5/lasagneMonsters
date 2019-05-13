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
static void drawLight(void);
static void damage(int amount, int damageType);
static void die(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initPlayer(Entity *e)
{
	Walter *w;

	w = malloc(sizeof(Walter));
	memset(w, 0, sizeof(Walter));

	w->health = w->maxHealth = 5;
	w->ammo = w->maxAmmo = 5;
	w->refillRate = 0.05f;

	e->typeName = "player";
	e->data = w;
	e->type = ET_PLAYER;
	e->atlasImage = getAtlasImage("gfx/entities/walter.png", 1);
	e->flags = EF_PUSH+EF_PUSHABLE+EF_SLOW_PUSH+EF_NO_MAP_BOUNDS;
	e->tick = tick;
	e->draw = draw;
	e->drawLight = drawLight;
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

	w->immuneTimer = MAX(w->immuneTimer - 1, 0);
	w->reload = MAX(w->reload - 1, 0);
	w->ammo = MIN(w->ammo + w->refillRate, w->maxAmmo);

	if (self->isOnGround)
	{
		w->jumps = w->hasDoubleJump ? 2 : 1;
	}

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

		if (isControl(CONTROL_JUMP) && w->jumps > 0)
		{
			clearControl(CONTROL_JUMP);

			w->jumps--;

			self->riding = NULL;

			self->dy = -20;

			playSound(SND_JUMP, CH_PLAYER);
		}

		if (isControl(CONTROL_FIRE))
		{
			if (!w->autoFire)
			{
				clearControl(CONTROL_FIRE);
			}

			if (w->ammo > 0 && w->reload == 0)
			{
				w->ammo--;

				/* don't let the player fire too quickly */
				w->reload = w->autoFire;

				initWaterBullet(self);

				playPositionalSound(SND_SHOOT, CH_PLAYER, self->x, self->y, world.player->x, world.player->y);
			}
		}
		else
		{
			w->reload = 0;
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

static void drawLight(void)
{
	drawLightEffect(self->x + (self-> w / 2) - world.camera.x, self->y + (self-> h / 2) - world.camera.y, 160, 255, 255, 255, 128);
}

static void damage(int amount, int damageType)
{
	Walter *w;

	w = (Walter*)self->data;

	if (damageType == DT_SLIME && w->immuneTimer == 0)
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

	world.state = WS_PLAYER_DEAD;
}

static void load(cJSON *root)
{
	Walter *w;

	w = (Walter*)self->data;

	w->health = cJSON_GetValueInt(root, "health", 3);
	w->maxHealth = cJSON_GetValueInt(root, "maxHealth", 3);
	w->ammo = cJSON_GetValueInt(root, "ammo", 5);
	w->maxAmmo = cJSON_GetValueInt(root, "maxAmmo", 5);
	w->refillRate = cJSON_GetValueDouble(root, "refillRate", 0.05f);
	w->autoFire = cJSON_GetValueInt(root, "autoFire", 0);
	w->hasDoubleJump = cJSON_GetValueInt(root, "hasDoubleJump", 0);
}

static void save(cJSON *root)
{
	Walter *w;

	w = (Walter*)self->data;

	cJSON_AddNumberToObject(root, "health", w->health);
	cJSON_AddNumberToObject(root, "maxHealth", w->maxHealth);
	cJSON_AddNumberToObject(root, "ammo", w->ammo);
	cJSON_AddNumberToObject(root, "maxAmmo", w->maxAmmo);
	cJSON_AddNumberToObject(root, "refillRate", w->refillRate);
	cJSON_AddNumberToObject(root, "autoFire", w->autoFire);
	cJSON_AddNumberToObject(root, "hasDoubleJump", w->hasDoubleJump);
}
