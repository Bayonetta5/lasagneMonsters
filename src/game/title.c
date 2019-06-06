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

#include "title.h"

static void logic(void);
static void draw(void);
static void newGame(void);
static void continueGame(void);
static void options(void);
static void stats(void);
static void quit(void);

static Widget *previousWidget;
static void (*oldLogic)(void);
static void (*oldDraw)(void);

void initTitle(void)
{
	Widget *w;

	calculateWidgetFrame("title");

	w = getWidget("new", "title");
	w->action = newGame;

	w = getWidget("continue", "title");
	w->action = continueGame;

	w = getWidget("options", "title");
	w->action = options;

	w = getWidget("stats", "title");
	w->action = stats;
	w->disabled = 1;

	w = getWidget("quit", "title");
	w->action = quit;

	app.selectedWidget = getWidget("new", "title");

	showWidgets("title", 1);

	loadStage("data/stages/000.json");

	initStage(0, WIPE_FADE);

	world.showHUD = 0;

	loadMusic("music/Mini_Epic_Theme.mp3");

	playMusic(0);

	oldLogic = app.delegate.logic;
	oldDraw = app.delegate.draw;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doWipe();

	doEntities();

	doWidgets("title");

	world.camera.x = 0;
	world.camera.y = 0;
}

static void draw(void)
{
	oldDraw();

	drawWidgetFrame();

	drawWidgets("title");

	drawText(10, SCREEN_HEIGHT - 35, 32, TEXT_LEFT, app.colors.white, "Copyright Parallel Realities, 2019");
	drawText(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 35, 32, TEXT_RIGHT, app.colors.white, "Version %.2f.%d", VERSION, REVISION);
}

static void returnFrom(void)
{
	showWidgets("title", 1);

	calculateWidgetFrame("title");

	app.selectedWidget = previousWidget;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void newGame(void)
{
	char filename[MAX_PATH_LENGTH];

	sprintf(filename, "%s/%s", app.saveDir, SAVE_FILENAME);

	deleteFile(filename);

	app.save.saving = FPS * 2;

	showWidgets("title", 0);

	destroyStage();

	loadAllStages();

	initStage(1, WIPE_IN);
}

static void continueGame(void)
{
	showWidgets("title", 0);

	destroyStage();

	loadGame();

	loadAllStages();

	initStage(game.stageId, WIPE_IN);
}

static void options(void)
{
	previousWidget = app.selectedWidget;

	showWidgets("title", 0);

	initOptions(returnFrom);
}

static void stats(void)
{
	previousWidget = app.selectedWidget;
}

static void quit(void)
{
	exit(1);
}
