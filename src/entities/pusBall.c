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

#include "pusBall.h"

static void tick(void);
static void touch(Entity *other);

void initPusBall(Entity *e)
{
	e->type = ET_DECORATION;
	e->tick = tick;
	e->touch = touch;
	e->flags = EF_TRANSIENT;

	e->drawLight = NULL;
}

static void tick(void)
{
	addPusParticle(self->x, self->y);
}

static void touch(Entity *other)
{
	if (other == NULL)
	{
		self->alive = ALIVE_DEAD;

		playPositionalSound(SND_PUS_SPLAT, CH_SPLAT, self->x, self->y, world.player->x, world.player->y);

		addPusBurstParticles(self->x, self->y);
	}
}
