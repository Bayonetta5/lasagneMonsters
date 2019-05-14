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

#include "radarEditor.h"

static void capFrameRate(long *then, float *remainder);

static int delay = 0;
static Stage *selectedStage;

static void logic(void)
{
	if (--delay <= 0)
	{
		if (app.keyboard[SDL_SCANCODE_W])
		{
			selectedStage->y--;
		}

		if (app.keyboard[SDL_SCANCODE_S])
		{
			selectedStage->y++;
		}

		if (app.keyboard[SDL_SCANCODE_A])
		{
			selectedStage->x--;
		}

		if (app.keyboard[SDL_SCANCODE_D])
		{
			selectedStage->x++;
		}

		if (app.keyboard[SDL_SCANCODE_LEFT])
		{
			world.camera.x++;
		}

		if (app.keyboard[SDL_SCANCODE_RIGHT])
		{
			world.camera.x--;
		}

		if (app.keyboard[SDL_SCANCODE_UP])
		{
			world.camera.y++;
		}

		if (app.keyboard[SDL_SCANCODE_DOWN])
		{
			world.camera.y--;
		}

		delay = 4;
	}
}

static void draw(void)
{
	Stage *s;
	int x, y;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		x = (s->x + world.camera.x) * CELL_W;
		y = (s->y + world.camera.y) * CELL_H;

		if (s == selectedStage)
		{
			drawRect(x, y, CELL_W - 4, CELL_H - 4, 192, 192, 0, 255);
		}
		else
		{
			drawRect(x, y, CELL_W - 4, CELL_H - 4, 0, 192, 0, 255);
		}

		drawText(x + (CELL_W / 2), y, 32, TEXT_CENTER, app.colors.white, "%d", s->id);
	}
}


int main(int argc, char *argv[])
{
	long then;
	float remainder;

	memset(&app, 0, sizeof(App));
	app.texturesTail = &app.texturesHead;
	app.dev.editor = 1;
	app.dev.disableLights = 1;

	initSDL();

	atexit(cleanup);

	SDL_ShowCursor(1);

	initGameSystem();

	loadAllStages();

	selectedStage = world.stagesHead.next;

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	then = SDL_GetTicks();

	remainder = 0;

	while (1)
	{
		prepareScene();

		doInput();

		logic();

		draw();

		presentScene();

		capFrameRate(&then, &remainder);
	}

	return 0;
}

static void capFrameRate(long *then, float *remainder)
{
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1)
	{
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();
}
