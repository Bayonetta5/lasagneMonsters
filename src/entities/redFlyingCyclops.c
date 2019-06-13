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

#include "redFlyingCyclops.h"

void initRedFlyingCyclops(Entity *e)
{
	Monster *m;

	initGreenFlyingCyclops(e);

	m = (Monster*)e->data;

	m->health = m->maxHealth = 12;
	m->coins = 5;
	m->minShotsToFire = 2;
	m->maxShotsToFire = 5;
	m->reloadTime = FPS / 5;

	e->typeName = "redFlyingCyclops";
	e->atlasImage = getAtlasImage("gfx/entities/redFlyingCyclops1.png", 1);
}
