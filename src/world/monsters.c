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

void monsterBecomeCoins(int n)
{
	Entity *e;
	int i;
	
	for (i = 0 ; i < n ; i++)
	{
		e = spawnEntity();
		e->x = self->x + (self->w / 2);
		e->y = self->y;
		
		initCoin(e);
		
		e->dx = (rand() % 500) - (rand() % 500);
		e->dy = -(700 + rand() % 700);
		
		e->dx /= 100;
		e->dy /= 100;
		
		e->tick = NULL;
		
		e->flags &= ~(EF_STATIC|EF_WEIGHTLESS);
	}
}
