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

#include "gameOver.h"

static void logic(void);
static void reloadGame(void);

static int timeout;

void initGameOver(void)
{
	timeout = FPS * 3;

	app.delegate.logic = logic;
}

static void logic(void)
{
	if (timeout > 0 && --timeout == 0)
	{
		initWipe(WIPE_OUT);
	}

	doEntities();

	doParticles();

	if (timeout == 0)
	{
		if (doWipe())
		{
			reloadGame();
		}
	}
}

static void reloadGame(void)
{
	long time;

	time = game.time;

	destroyStage();

	destroyGame();

	loadGame();

	initStage(game.stageId, WIPE_IN);

	game.time = time;

	app.save.saving = FPS * 2;
}
