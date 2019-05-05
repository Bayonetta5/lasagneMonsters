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

#include "savePoint.h"

static void tick(void);
static void draw(void);
static void touch(Entity *other);

static AtlasImage *computer[3] = {NULL, NULL, NULL};
static AtlasImage *sparkleTexture = NULL;

void initSavePoint(Entity *e)
{
	SavePoint *s;

	s = malloc(sizeof(SavePoint));
	memset(s, 0, sizeof(SavePoint));

	e->typeName = "savePoint";
	e->type = ET_SAVE_POINT;
	e->data = s;
	e->flags = EF_WEIGHTLESS+EF_STATIC;
	e->tick = tick;
	e->draw = draw;
	e->touch = touch;

	if (computer[0] == NULL)
	{
		computer[0] = getAtlasImage("gfx/entities/computer1.png", 1);
		computer[1] = getAtlasImage("gfx/entities/computer2.png", 1);
		computer[2] = getAtlasImage("gfx/entities/computer3.png", 1);
		sparkleTexture = getAtlasImage("gfx/particles/light.png", 1);
	}

	e->atlasImage = computer[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
}

static void tick(void)
{
	SavePoint *s;

	s = (SavePoint*)self->data;

	if (s->active)
	{
		if (--s->frameTime <= 0)
		{
			if (++s->frame >= 3)
			{
				s->frame = 1;
			}

			s->frameTime = FPS / 3;
		}
	}

	self->atlasImage = computer[s->frame];
}

static void draw(void)
{
	int x, y;
	SavePoint *s;

	s = (SavePoint*)self->data;

	blitAtlasImage(self->atlasImage, self->x - world.camera.x, self->y - world.camera.y, 0, self->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

	if (s->active)
	{
		x = self->x + (self->w / 2) - world.camera.x;
		y = self->y + (self->h / 2) - world.camera.y;

		SDL_SetTextureColorMod(sparkleTexture->texture, 168, 192, 255);
		SDL_SetTextureAlphaMod(sparkleTexture->texture, 96);

		blitAtlasImage(sparkleTexture, x, y, 1, SDL_FLIP_NONE);

		SDL_SetTextureColorMod(sparkleTexture->texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(sparkleTexture->texture, 255);
	}
}

static void touch(Entity *other)
{
	SavePoint *s;

	if (other == world.player)
	{
		s = (SavePoint*)self->data;

		if (!s->active)
		{
			s->active = 1;

			playSound(SND_COMPUTER, -1);

			app.save.saving = FPS * 2;
		}
	}
}
