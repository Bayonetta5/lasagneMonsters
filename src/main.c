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

#include "main.h"

static void handleCommandLine(int argc, char *argv[]);
static void capFrameRate(long *then, float *remainder);

int main(int argc, char *argv[])
{
	long then, nextSecond;
	float remainder;
	int frames;

	memset(&app, 0, sizeof(App));
	app.texturesTail = &app.texturesHead;

	initLookups();

	initSDL();

	atexit(cleanup);

	initGameSystem();

	handleCommandLine(argc, argv);

	then = SDL_GetTicks();

	frames = remainder = 0;

	nextSecond = SDL_GetTicks() + 1000;

	while (1)
	{
		prepareScene();

		doInput();

		app.delegate.logic();

		app.delegate.draw();

		presentScene();

		frames++;

		capFrameRate(&then, &remainder);

		if (SDL_GetTicks() > nextSecond)
		{
			game.stats[STAT_TIME]++;

			app.dev.fps = frames;

			nextSecond = SDL_GetTicks() + 1000;

			frames = 0;
		}

		if (app.save.saving > 0)
		{
			app.save.saving = MAX(app.save.saving - 1, 0);
			app.save.pulse += 0.1;

			if (app.save.saving == FPS)
			{
				saveGame();
			}
		}
	}

	return 0;
}

static void handleCommandLine(int argc, char *argv[])
{
	int i, n;

	n = 0;

	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-debug") == 0)
		{
			app.dev.debug = 1;

			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
		}

		if (strcmp(argv[i], "-info") == 0)
		{
			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
		}

		if (strcmp(argv[i], "-title") == 0)
		{
			n = 0;
		}

		if (strcmp(argv[i], "-stage") == 0)
		{
			n = atoi(argv[i + 1]);
		}

		if (strcmp(argv[i], "-load") == 0)
		{
			loadGame();

			n = game.stageId;
		}

		if (strcmp(argv[i], "-boots") == 0)
		{
			app.dev.boots = 1;
		}
	}

	if (n > 0)
	{
		loadAllStages();

		initStage(n, WIPE_FADE);
	}
	else
	{
		initTitle();
	}
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
