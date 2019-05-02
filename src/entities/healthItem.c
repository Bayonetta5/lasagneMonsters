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


#include "healthItem.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *appleTexture = NULL;
static AtlasImage *chocolateTexture = NULL;
static AtlasImage *beerTexture = NULL;

void initHealthItem(Entity *e)
{
	Item *i;
	int n;
	
	i = malloc(sizeof(Item));
	memset(i, 0, sizeof(Item));
	
	i->health = FPS * 5;
	
	e->typeName = "health";
	e->type = ET_ITEM;
	e->data = i;
	e->flags = EF_DELETE+EF_FRICTION;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;
	
	if (appleTexture == NULL)
	{
		appleTexture = getAtlasImage("gfx/entities/apple.png", 1);
		chocolateTexture = getAtlasImage("gfx/entities/chocolate.png", 1);
		beerTexture = getAtlasImage("gfx/entities/beer.png", 1);
	}
	
	n = rand() % 25;
	
	if (n == 0)
	{
		e->atlasImage = beerTexture;
		i->value = 5;
	}
	else if (n < 10)
	{
		e->atlasImage = chocolateTexture;
		i->value = 2;
	}
	else
	{
		e->atlasImage = appleTexture;
		i->value = 1;
	}
	
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	Item *i;
	
	i = (Item*)self->data;
	
	if (--i->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

static void draw(void)
{
	Item *i;
	
	i = (Item*)self->data;
	
	if (i->health > FPS || (i->health < FPS && i->health % 5 == 0))
	{
		blitAtlasImage(self->atlasImage, self->x - stage.camera.x, self->y - stage.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
	}
}

static void touch(Entity *other)
{
	Item *i;
	Walter *w;
	
	if (self->alive == ALIVE_ALIVE && other == stage.player)
	{
		i = (Item*)self->data;
		
		w = (Walter*)other->data;
		
		w->health = MIN(w->health + i->value, w->maxHealth);
		
		addGameText(self->x, self->y, "+%dHP", i->value);
		
		self->alive = ALIVE_DEAD;
	}
}
