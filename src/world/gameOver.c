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
static void draw(void);
static void preReloadGame(void);
static void reloadGame(void);
static void restartAtCheckpoint(void);
static void quit(void);

static int timeout;
static int reloading;
static void (*oldLogic)(void);
static void (*oldDraw)(void);
static Widget *reloadWidget;
static Widget *retryWidget;
static Widget *quitWidget;

void initGameOver(void)
{
	timeout = FPS * 2;

	reloading = 0;

	reloadWidget = getWidget("reload", "gameOver");
	reloadWidget->action = preReloadGame;

	retryWidget = getWidget("retry", "gameOver");
	retryWidget->action = restartAtCheckpoint;

	quitWidget = getWidget("quit", "gameOver");
	quitWidget->action = quit;

	calculateWidgetFrame("gameOver");

	app.selectedWidget = reloadWidget;

	oldLogic = app.delegate.logic;
	oldDraw = app.delegate.draw;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (timeout > 0 && !reloading)
	{
		if (--timeout == 0)
		{
			showWidgets("gameOver", 1);
		}
	}

	doEntities();

	doParticles();

	if (timeout == 0)
	{
		doWidgets("gameOver");
	}

	if (reloading && doWipe())
	{
		reloadGame();
	}
}

static void draw(void)
{
	oldDraw();

	if (timeout == 0)
	{
		drawText(SCREEN_WIDTH / 2, 75, 128, TEXT_CENTER, app.colors.white, "GAME OVER");

		drawWidgetFrame();

		drawWidgets("gameOver");
	}
}

static void restartAtCheckpoint(void)
{
	Walter *w;

	w = (Walter*)world.player->data;

	world.player->x = w->checkpoint.x;
	world.player->y = w->checkpoint.y;
	world.player->alive = ALIVE_ALIVE;

	w->health = w->maxHealth;
	w->immuneTimer = FPS * 3;

	removeFromDeadlist(world.player);

	stage->entityTail->next = world.player;
	stage->entityTail = world.player;

	/* reduce time by 5 minutes */
	game.time -= (FPS * 60 * 5);

	world.showHUD = 1;

	initWipe(WIPE_FADE);

	app.delegate.logic = oldLogic;
	app.delegate.draw = oldDraw;
}

static void preReloadGame(void)
{
	reloading = 1;

	timeout = 1;

	initWipe(WIPE_OUT);
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

static void quit(void)
{
	exit(0);
}
