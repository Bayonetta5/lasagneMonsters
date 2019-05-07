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

#include "gumballMachine.h"

static void touch(Entity *other);

void initGumballMachine(Entity *e)
{
	e->typeName = "gumballMachine";
	e->type = ET_STRUCTURE;
	e->atlasImage = getAtlasImage("gfx/entities/gumballMachine.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_STATIC;
	e->touch = touch;
}

static void touch(Entity *other)
{
	if (other == world.player)
	{
		if (isControl(CONTROL_USE))
		{
			clearControl(CONTROL_USE);

			if (game.coins > 10)
			{
				playSound(SND_BUY, -1);

				game.coins -= 10;

				spawnGumball(self->x, self->y);
			}
		}
	}
}
