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

#include "key.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *keyTexture = NULL;
static AtlasImage *sparkleTexture = NULL;

void initKey(Entity *e)
{
	Item *k;
	
	k = malloc(sizeof(Item));
	memset(k, 0, sizeof(Item));
	
	k->bobValue = rand() % 10;
	k->by = e->y;
	
	e->typeName = "key";
	e->type = ET_ITEM;
	e->data = k;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_STATIC+EF_DELETE;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
	
	if (keyTexture == NULL)
	{
		keyTexture = getAtlasImage("gfx/entities/key.png", 1);
		sparkleTexture = getAtlasImage("gfx/particles/light.png", 1);
	}
	
	e->atlasImage = keyTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Item *k;
	
	k = (Item*)self->data;
	
	k->bobValue += 0.1;
	
	k->by += sin(k->bobValue) * 0.5;
}

static void draw(void)
{
	Item *k;
	int x, y;
	
	k = (Item*)self->data;
	
	x = self->x + (self->w / 2) - world.camera.x;
	y = k->by + (self->h / 2) - world.camera.y;
	
	SDL_SetTextureColorMod(sparkleTexture->texture, 255, 128, 64);
	SDL_SetTextureAlphaMod(sparkleTexture->texture, 64);
	
	blitAtlasImage(sparkleTexture, x, y, 1, SDL_FLIP_NONE);
	
	SDL_SetTextureColorMod(sparkleTexture->texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(sparkleTexture->texture, 255);
	
	blitAtlasImage(self->atlasImage, self->x - world.camera.x, k->by - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

static void touch(Entity *other)
{
	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		stage->keys++;
		
		self->alive = ALIVE_DEAD;
		
		playPositionalSound(SND_KEY, CH_ITEM, self->x, self->y, world.player->x, world.player->y);
		
		addPowerupParticles(self->x + self->w / 2, self->y + self->h / 2);
		
		addGameText(self->x, self->y, "+Key");
	}
}
