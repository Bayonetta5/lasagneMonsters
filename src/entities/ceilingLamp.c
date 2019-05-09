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

#include "ceilingLamp.h"

static void draw(void);

static AtlasImage *texture = NULL;

void initCeilingLamp(Entity *e)
{
	initLamp(e);

	e->typeName = "ceilingLamp";

	if (texture == NULL)
	{
		texture = getAtlasImage("gfx/entities/ceilingLamp.png", 1);
	}

	e->flags |= EF_WEIGHTLESS;

	e->draw = draw;

	e->atlasImage = texture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void draw(void)
{
	int x, y;

	x = self->x - world.camera.x;
	y = self->y - world.camera.y;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, SDL_FLIP_NONE);

	x += (self->w / 2);

	y += 18;

	drawObjectGlowAt(x - 18, y, 255, 255, 192, 64);
	drawObjectGlowAt(x + 18, y, 255, 255, 192, 64);
}
