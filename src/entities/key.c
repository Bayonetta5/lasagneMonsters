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
static void touch(Entity *other);

void initKey(Entity *e)
{
	Item *k;
	
	k = malloc(sizeof(Item));
	memset(k, 0, sizeof(Item));
	
	k->bobValue = rand() % 10;
	
	e->typeName = "key";
	e->type = ET_ITEM;
	e->data = k;
	e->atlasImage = getAtlasImage("gfx/entities/key.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_STATIC+EF_DELETE;
	e->tick = tick;
	e->touch = touch;
}

static void tick(void)
{
	Item *k;
	
	k = (Item*)self->data;
	
	k->bobValue += 0.1;
	
	self->y += sin(k->bobValue) * 0.5;
}

static void touch(Entity *other)
{
	if (self->alive == ALIVE_ALIVE && other != NULL && other->type == ET_PLAYER)
	{
		self->alive = ALIVE_DEAD;
		
		playPositionalSound(SND_KEY, CH_ITEM, self->x, self->y, stage.player->x, stage.player->y);
		
		addPowerupParticles(self->x + self->w / 2, self->y + self->h / 2);
	}
}
