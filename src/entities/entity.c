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

#include "entity.h"

static void draw(void);
static void drawLight(void);

static unsigned long entityId = 0;

void initEntity(Entity *e)
{
	e->id = ++entityId;
	e->facing = FACING_RIGHT;
	e->draw = draw;
	e->drawLight = drawLight;
}

static void draw(void)
{
	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

static void drawLight(void)
{
	drawLightEffect(self->cx - world.camera.x, self->cy - world.camera.y, MAX(self->w, self->h), 128, 128, 128, 255);
}
