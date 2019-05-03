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

#include "stage.h"

static void logic(void);
static void draw(void);
static void drawBackground(void);
static void doControls(void);
static void initBackgroundData(void);
static void doGame(void);
static void doMenu(void);
static void drawGame(void);
static void drawMenu(void);
static void transfer(void);
static void resume(void);
static void stats(void);
static void options(void);
static void quit(void);
void destroyStage(void);

static int show;
static AtlasImage *backgroundTile;
static Widget *resumeWidget;
static Widget *statsWidget;
static Widget *optionsWidget;
static Widget *quitWidget;
static Widget *previousWidget;
static int backgroundData[MAP_WIDTH][MAP_HEIGHT];

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	memset(&stage, 0, sizeof(Stage));
	
	stage.entityTail = &stage.entityHead;
	stage.particleTail = &stage.particleHead;
	stage.gameTextTail = &stage.gameTextHead;
	
	resumeWidget = getWidget("resume", "stage");
	resumeWidget->action = resume;
	
	statsWidget = getWidget("stats", "stage");
	statsWidget->action = stats;
	
	optionsWidget = getWidget("options", "stage");
	optionsWidget->action = options;
	
	quitWidget = getWidget("quit", "stage");
	quitWidget->action = quit;
	
	initBackgroundData();
	
	backgroundTile = getAtlasImage("gfx/tiles/0.png", 1);
	
	initWipe(WIPE_FADE);
}

void loadStage(int randomTiles)
{
	cJSON *root;
	char *json;
	char filename[MAX_FILENAME_LENGTH];
	
	srand(256 * stage.num);
	
	sprintf(filename, "data/stages/%03d.json", stage.num);
	
	json = readFile(getFileLocation(filename));
		
	root = cJSON_Parse(json);
	
	show = SHOW_GAME;
	
	initMap(cJSON_GetObjectItem(root, "map"));
	
	initQuadtree(&stage.quadtree);
	
	initEntities(root);
	
	if (randomTiles)
	{
		randomizeTiles();
		
		dropToFloor();
	}
	
	free(json);
	
	cJSON_Delete(root);
}

static void logic(void)
{
	if (doWipe())
	{
		switch (show)
		{
			case SHOW_MENU:
				doMenu();
				break;
				
			default:
				doGame();
				break;
		}
	}
	
	doCamera();
}

static void doGame(void)
{
	if (stage.transferCube != NULL)
	{
		transfer();
	}
	
	doControls();
	
	doEntities();
	
	doParticles();
	
	doHud();
}

static void doMenu(void)
{
	doWidgets("stage");
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE] || isControl(CONTROL_PAUSE))
	{
		resumeSound();
		
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		
		clearControl(CONTROL_PAUSE);
		
		show = SHOW_GAME;
	}
}

static void doControls(void)
{
	if (app.keyboard[SDL_SCANCODE_ESCAPE] || isControl(CONTROL_PAUSE))
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		
		clearControl(CONTROL_PAUSE);
		
		show = SHOW_MENU;
		
		showWidgets("stage", 1);
	
		calculateWidgetFrame("stage");
		
		app.selectedWidget = resumeWidget;
		
		pauseSound();
	}
}

static void draw(void)
{
	switch (show)
	{
		case SHOW_MENU:
			drawMenu();
			break;
		
		default:
			drawGame();
			break;
	}
	
	drawWipe();
}

static void drawGame()
{
	app.dev.drawing = 0;
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 64, 64, 64, 64);
	
	drawBackground();
	
	drawEntities(1);
	
	drawMap();
	
	drawEntities(0);
	
	drawParticles();
	
	drawHud();
}

static void drawMenu()
{
	drawGame();
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 96);
	
	drawWidgetFrame();
	
	drawWidgets("stage");
}

/* draws a parallax background */
static void drawBackground(void)
{
	int x, y, x1, x2, y1, y2, mx, my, camX;
	
	camX = stage.camera.x * 0.5f;
	
	x1 = (camX % TILE_SIZE) * -1;
	x2 = x1 + MAP_RENDER_WIDTH * TILE_SIZE + (x1 == 0 ? 0 : TILE_SIZE);

	y1 = (stage.camera.y % TILE_SIZE) * -1;
	y2 = y1 + MAP_RENDER_HEIGHT * TILE_SIZE + (y1 == 0 ? 0 : TILE_SIZE);
	
	mx = camX / TILE_SIZE;
	my = stage.camera.y / TILE_SIZE;
	
	for (y = y1 ; y < y2 ; y += TILE_SIZE)
	{
		for (x = x1 ; x < x2 ; x += TILE_SIZE)
		{
			if (backgroundData[mx][my] == 1)
			{
				blitAtlasImage(backgroundTile, x, y, 0, SDL_FLIP_NONE);
			}
			
			mx++;
		}
		
		mx = camX / TILE_SIZE;
		
		my++;
	}
}

void destroyStage(void)
{
	destroyQuadtree();
	
	destroyEntities();
	
	destroyParticles();
	
	memset(&stage, 0, sizeof(Stage));
	
	stage.entityTail = &stage.entityHead;
	stage.particleTail = &stage.particleHead;
	stage.gameTextTail = &stage.gameTextHead;
}

static Entity *findStartPoint(const char *name)
{
	Entity *e;
	
	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_START_POINT && strcmp(e->name, name) == 0)
		{
			return e;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such start point '%s'", name);
	exit(1);
	
	return NULL;
}

static void transfer(void)
{
	TransferCube transferCube;
	Walter walter;
	Entity *e;
	
	memcpy(&walter, stage.player->data, sizeof(Walter));
	memcpy(&transferCube, stage.transferCube, sizeof(TransferCube));
	
	destroyStage();
	
	stage.num = transferCube.targetStage;
	
	loadStage(1);
	
	e = findStartPoint(transferCube.targetFlag);
	
	memcpy(stage.player->data, &walter, sizeof(Walter));
	
	stage.player->x = e->x;
	stage.player->y = e->y;
}

static void initBackgroundData(void)
{
	int x, y, n;
	
	memset(backgroundData, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
	
	for (x = 0 ; x < MAP_WIDTH ; x++)
	{
		for (y = 0; y < MAP_HEIGHT ; y++)
		{
			n = ((x ^ y) / 3) % 4;
			
			if (n == 3)
			{
				backgroundData[x][y] = 1;
			}
		}
	}
}

static void resume(void)
{
	show = SHOW_GAME;
}

static void options(void)
{
	previousWidget = optionsWidget;
	
	showWidgets("stage", 0);
}

static void stats(void)
{
	previousWidget = statsWidget;
	
	showWidgets("stage", 0);
}

static void quit(void)
{
	destroyStage();
	
	exit(1);
}
