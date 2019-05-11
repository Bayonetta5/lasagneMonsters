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

#include "misc.h"

void drawEntityLinkLines(Entity *e, char *targetName)
{
	Entity *other;
	int sx, sy, ex, ey;

	sx = e->x + (e->w / 2) - world.camera.x;
	sy = e->y + (e->h / 2) - world.camera.y;

	for (other = stage->entityHead.next ; other != NULL ; other = other->next)
	{
		if (strcmp(other->name, targetName) == 0)
		{
			ex = other->x + (other->w / 2) - world.camera.x;
			ey = other->y + (other->h / 2) - world.camera.y;

			drawLine(sx, sy, ex, ey, 192, 192, 255, 255);
		}
	}
}
