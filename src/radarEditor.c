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

static void save(void)
{
	Stage *s;
	char filename[MAX_FILENAME_LENGTH];

	printf("Saving ...\r");

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		stage = s;

		sprintf(filename, "data/stages/%03d.json", stage->id);

		saveStage(filename);
	}

	printf("Saving ... Done\n");

	stage = NULL;
}

static void selectStage(void)
{
	Stage *s;
	int x, y;

	if (stage == NULL)
	{
		for (s = world.stagesHead.next ; s != NULL ; s = s->next)
		{
			x = (s->x * (CELL_SIZE + GRID_SPACING)) + world.camera.x;
			y = (s->y * (CELL_SIZE + GRID_SPACING)) + world.camera.y;

			if (collision(x, y, CELL_SIZE, CELL_SIZE, app.mouse.x, app.mouse.y, 1, 1))
			{
				stage = s;
			}
		}
	}
	else
	{
		stage = NULL;
	}
}

static void logic(void)
{
	if (app.mouse.buttons[SDL_BUTTON_LEFT])
	{
		app.mouse.buttons[SDL_BUTTON_LEFT] = 0;

		selectStage();
	}

	if (app.keyboard[SDL_SCANCODE_A])
	{
		world.camera.x += CAM_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		world.camera.x -= CAM_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_W])
	{
		world.camera.y += CAM_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_S])
	{
		world.camera.y -= CAM_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		app.keyboard[SDL_SCANCODE_SPACE] = 0;

		save();
	}

	if (stage != NULL)
	{
		stage->x = app.mouse.x - world.camera.x;
		stage->x /= (GRID_SPACING + CELL_SIZE);

		stage->y = app.mouse.y - world.camera.y;
		stage->y /= (GRID_SPACING + CELL_SIZE);
	}
}

static void drawStageLinks(void)
{
	Stage *s, *other;
	Entity *e;
	TransferCube *t;
	int sx, sy, ex, ey;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		for (e = s->entityHead.next ; e != NULL ; e = e->next)
		{
			if (e->type == ET_TRANSFER_CUBE)
			{
				t = (TransferCube*)e->data;

				other = getStage(t->targetStage);

				if (other != NULL)
				{
					sx = (s->x * (CELL_SIZE + GRID_SPACING)) + CELL_SIZE / 2;
					sy = (s->y * (CELL_SIZE + GRID_SPACING)) + CELL_SIZE / 2;

					sx += world.camera.x;
					sy += world.camera.y;

					ex = (other->x * (CELL_SIZE + GRID_SPACING)) + CELL_SIZE / 2;
					ey = (other->y * (CELL_SIZE + GRID_SPACING)) + CELL_SIZE / 2;

					ex += world.camera.x;
					ey += world.camera.y;

					drawLine(sx, sy, ex, ey, 255, 255, 255, 255);
				}
			}
		}
	}
}

static void draw(void)
{
	Stage *s;
	int x, y;

	drawStageLinks();

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		x = (s->x * (CELL_SIZE + GRID_SPACING)) + world.camera.x;
		y = (s->y * (CELL_SIZE + GRID_SPACING)) + world.camera.y;

		if (s == stage)
		{
			drawRect(x, y, CELL_SIZE, CELL_SIZE, 192, 192, 0, 255);
		}
		else
		{
			drawRect(x, y, CELL_SIZE, CELL_SIZE, 0, 192, 0, 255);
		}

		drawText(x + (CELL_SIZE / 2), y, 32, TEXT_CENTER, app.colors.white, "%d", s->id);
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

	stage = NULL;

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

	*remainder += 0.666667;

	*then = SDL_GetTicks();
}
