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

#include "mapEditor.h"

static void capFrameRate(long *then, float *remainder);

static int tile;
static int cameraTimer;
static Entity **entities;
static int numEnts;
static int entIndex;
static Entity *entity;
static Entity *selectedEntity;
static int mode;
static Node nodeListHead, *nodeListTail;

static void createEntity(void)
{
	Entity *e;
	int x, y;
	
	x = (app.mouse.x / 8) * 8;
	y = (app.mouse.y / 8) * 8;
	
	x += world.camera.x;
	y += world.camera.y;
	
	e = spawnEditorEntity(entity->typeName, x, y);
	
	addToQuadtree(e, &world.quadtree);
}

static void deleteEntity(void)
{
	Entity *e, *prev;
	
	prev = &stage->entityHead;
	
	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (collision(app.mouse.x + world.camera.x, app.mouse.y + world.camera.y, 1, 1, e->x, e->y, e->w, e->h))
		{
			if (e == stage->entityTail)
			{
				stage->entityTail = prev;
			}
			
			prev->next = e->next;
			
			removeFromQuadtree(e, &world.quadtree);
			
			/* loaded, so safe to delete */
			if (e->id != -1)
			{
				free(e->data);
			}
			
			free(e);
			
			e = prev;
		}
		
		prev = e;
	}
}

static void cycleTile(int dir)
{
	int ok;
	
	do
	{
		tile += dir;
		
		if (tile < 0)
		{
			tile = MAX_TILES - 1;
		}
		else if (tile >= MAX_TILES)
		{
			tile = 0;
		}
		
		ok = world.tiles[tile] != NULL;
	}
	while (!ok);
}

static void cycleEnt(int dir)
{
	entIndex += dir;
	
	if (entIndex < 0)
	{
		entIndex = numEnts - 1;
	}
	else if (entIndex >= numEnts)
	{
		entIndex = 0;
	}
	
	entity = entities[entIndex];
}

static void toggleSelectEntity(void)
{
	Entity *e;
	
	if (selectedEntity == NULL)
	{
		for (e = stage->entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + world.camera.x, app.mouse.y + world.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				selectedEntity = e;
				return;
			}
		}
	}
	else
	{
		removeFromQuadtree(selectedEntity, &world.quadtree);
		
		selectedEntity->x = ((app.mouse.x / 8) * 8) + world.camera.x;
		selectedEntity->y = ((app.mouse.y / 8) * 8) + world.camera.y;
		
		addToQuadtree(selectedEntity, &world.quadtree);
		
		selectedEntity = NULL;
	}
}

static void flipSelectedEntity(void)
{
	Entity *e;
	
	if (selectedEntity != NULL)
	{
		selectedEntity->facing = !selectedEntity->facing;
	}
	else
	{
		for (e = stage->entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + world.camera.x, app.mouse.y + world.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				e->facing = !e->facing;
				return;
			}
		}
	}
}

static void save(void)
{
	char filename[MAX_FILENAME_LENGTH];
	
	sprintf(filename, "data/stages/%03d.json", stage->id);
	
	printf("Saving %s ...\n", filename);
	
	saveStage(filename);
	
	printf("Saving %s ... - done\n", filename);
}

static void addNode(int x, int y)
{
	Node *n;
	
	n = malloc(sizeof(Node));
	memset(n, 0, sizeof(Node));
	nodeListTail->next = n;
	nodeListTail = n;
	
	n->x = x;
	n->y = y;
}

static void floodFill(int to)
{
	Node *n;
	int x, y, from;
	
	x = (app.mouse.x + world.camera.x) / TILE_SIZE;
	y = (app.mouse.y + world.camera.y) / TILE_SIZE;
	
	from = stage->map[x][y];
	
	if (from != to)
	{
		memset(&nodeListHead, 0, sizeof(Node));
		nodeListTail = &nodeListHead;
		
		addNode(x, y);
		
		while (nodeListHead.next)
		{
			n = nodeListHead.next;
			
			if (n->x >= 0 && n->y >= 0 && n->x < MAP_WIDTH && n->y < MAP_HEIGHT)
			{
				if (stage->map[n->x][n->y] == from)
				{
					stage->map[n->x][n->y] = to;
					
					addNode(n->x, n->y - 1);
					addNode(n->x, n->y + 1);
					addNode(n->x - 1, n->y);
					addNode(n->x + 1, n->y);
				}
			}
			
			nodeListHead.next = n->next;
			free(n);
		}
	}
}

static void logic(void)
{
	int x, y;
	
	if (app.mouse.buttons[SDL_BUTTON_LEFT])
	{
		switch (mode)
		{
			case MODE_TILE:
				x = (app.mouse.x + world.camera.x) / TILE_SIZE;
				y = (app.mouse.y + world.camera.y) / TILE_SIZE;
				stage->map[x][y] = tile;
				break;
				
			case MODE_ENT:
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
				createEntity();
				break;
				
			case MODE_PICK:
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
				toggleSelectEntity();
				break;
		}
	}
	
	if (app.mouse.buttons[SDL_BUTTON_RIGHT])
	{
		switch (mode)
		{
			case MODE_TILE:
				x = (app.mouse.x + world.camera.x) / TILE_SIZE;
				y = (app.mouse.y + world.camera.y) / TILE_SIZE;
				stage->map[x][y] = 0;
				break;
				
			case MODE_ENT:
				deleteEntity();
				break;
				
			case MODE_PICK:
				app.mouse.buttons[SDL_BUTTON_RIGHT] = 0;
				flipSelectedEntity();
				break;
		}
	}
	
	if (app.mouse.buttons[SDL_BUTTON_X1])
	{
		app.mouse.buttons[SDL_BUTTON_X1] = 0;
		
		if (mode == MODE_TILE)
		{
			cycleTile(1);
		}
		else if (mode == MODE_ENT)
		{
			cycleEnt(1);
		}
	}
	
	if (app.mouse.buttons[SDL_BUTTON_X2])
	{
		app.mouse.buttons[SDL_BUTTON_X2] = 0;
		
		if (mode == MODE_TILE)
		{
			cycleTile(-1);
		}
		else if (mode == MODE_ENT)
		{
			cycleEnt(-1);
		}
	}
	
	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		app.keyboard[SDL_SCANCODE_SPACE] = 0;
		
		save();
	}
	
	if (app.keyboard[SDL_SCANCODE_1])
	{
		app.keyboard[SDL_SCANCODE_1] = 0;
		
		mode = MODE_TILE;
	}
	
	if (app.keyboard[SDL_SCANCODE_2])
	{
		app.keyboard[SDL_SCANCODE_2] = 0;
		
		mode = MODE_ENT;
	}
	
	if (app.keyboard[SDL_SCANCODE_3])
	{
		app.keyboard[SDL_SCANCODE_3] = 0;
		
		mode = MODE_PICK;
	}
	
	if (mode == MODE_TILE)
	{
		if (app.keyboard[SDL_SCANCODE_F1])
		{
			floodFill(tile);
			
			app.keyboard[SDL_SCANCODE_F1] = 0;
		}
		
		if (app.keyboard[SDL_SCANCODE_F2])
		{
			floodFill(0);
			
			app.keyboard[SDL_SCANCODE_F2] = 0;
		}
	}
	
	if (--cameraTimer <= 0)
	{
		cameraTimer = 3;
		
		if (app.keyboard[SDL_SCANCODE_W])
		{
			world.camera.y -= TILE_SIZE;
		}
		
		if (app.keyboard[SDL_SCANCODE_S])
		{
			world.camera.y += TILE_SIZE;
		}
		
		if (app.keyboard[SDL_SCANCODE_A])
		{
			world.camera.x -= TILE_SIZE;
		}
		
		if (app.keyboard[SDL_SCANCODE_D])
		{
			world.camera.x += TILE_SIZE;
		}
		
		/* use 64, so things don't look wonky on the right-hand side */
		world.camera.x = MIN(MAX(world.camera.x, -(TILE_SIZE * 4)), (MAP_WIDTH * TILE_SIZE) - SCREEN_WIDTH + (TILE_SIZE - 64));
		world.camera.y = MIN(MAX(world.camera.y, -(TILE_SIZE * 4)), (MAP_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT);
	}
}

static void drawCurrentTile(void)
{
	int x, y;
	SDL_Rect r;
	
	x = (app.mouse.x / TILE_SIZE) * TILE_SIZE;
	y = (app.mouse.y / TILE_SIZE) * TILE_SIZE;
	
	blitAtlasImage(world.tiles[tile], x, y, 0, SDL_FLIP_NONE);
	
	r.x = x;
	r.y = y;
	r.w = TILE_SIZE;
	r.h = TILE_SIZE;
	
	SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255);
	SDL_RenderDrawRect(app.renderer, &r);
}

static void drawCurrentEnt(void)
{
	int x, y;
	
	x = (app.mouse.x / 8) * 8;
	y = (app.mouse.y / 8) * 8;
	
	blitAtlasImage(entity->atlasImage, x, y, 0, SDL_FLIP_NONE);
}

static void drawSelectedEnt(void)
{
	int x, y;
	
	if (selectedEntity != NULL)
	{
		x = (app.mouse.x / 8) * 8;
		y = (app.mouse.y / 8) * 8;
		
		removeFromQuadtree(selectedEntity, &world.quadtree);
		
		selectedEntity->x = x + world.camera.x;
		selectedEntity->y = y + world.camera.y;
		
		addToQuadtree(selectedEntity, &world.quadtree);
	}
}

static void drawInfo(void)
{
	Entity *e;
	int x, y;
	
	x = ((app.mouse.x + world.camera.x) / TILE_SIZE) * TILE_SIZE;
	y = ((app.mouse.y + world.camera.y) / TILE_SIZE) * TILE_SIZE;
	
	drawRect(0, 0, SCREEN_WIDTH, 30, 0, 0, 0, 192);
	
	drawText(10, 0, 32, TEXT_LEFT, app.colors.white, "Stage: %d", stage->id);
	
	drawText(310, 0, 32, TEXT_LEFT, app.colors.white, "Pos: %d,%d", x, y);
	
	if (mode == MODE_PICK)
	{
		for (e = stage->entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + world.camera.x, app.mouse.y + world.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				drawText(e->x + (e->w / 2) - world.camera.x, e->y - 32 - world.camera.y, 32, TEXT_CENTER, app.colors.white, "%d,%d", (int)e->x, (int)e->y);
			}
		}
	}
}

static void draw(void)
{
	drawMap();
	
	drawEntities(0);
	drawEntities(1);
	
	switch (mode)
	{
		case MODE_TILE:
			drawCurrentTile();
			break;
			
		case MODE_ENT:
			drawCurrentEnt();
			break;
			
		case MODE_PICK:
			drawSelectedEnt();
			break;
	}
	
	drawInfo();
}

/* defaults */
static void loadTiles(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];
	
	for (i = 1 ; i <= MAX_TILES ; i++)
	{
		sprintf(filename, "gfx/tiles/%d.png", i);
		
		world.tiles[i] = getAtlasImage(filename, 0);
	}
}

static void centreOnPlayer(void)
{
	Entity *e;
	
	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_PLAYER)
		{
			world.camera.x = e->x;
			world.camera.y = e->y;
			
			world.camera.x -= SCREEN_WIDTH / 2;
			world.camera.y -= SCREEN_HEIGHT / 2;
			
			world.camera.x /= TILE_SIZE;
			world.camera.y /= TILE_SIZE;
			
			world.camera.x *= TILE_SIZE;
			world.camera.y *= TILE_SIZE;
		}
		
		e->flags &= ~EF_INVISIBLE;
	}
}

static void tryLoadStage(int n)
{
	Stage *s, *last;
	
	last = &world.stagesHead;
	
	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->id == n)
		{
			initStage(n, 0);
			return;
		}
		
		last = s;
	}
	
	s = malloc(sizeof(Stage));
	memset(s, 0, sizeof(Stage));
	last->next = s;
	last = s;
	
	s->id = n;
	s->entityTail = &s->entityHead;
	
	initStage(n, 0);
}

static void handleCommandLine(int argc, char *argv[])
{
	int i;
	
	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-stage") == 0)
		{
			tryLoadStage(atoi(argv[i + 1]));
		}
	}
}

int main(int argc, char *argv[])
{
	long then;
	float remainder;
	
	memset(&app, 0, sizeof(App));
	app.texturesTail = &app.texturesHead;
	app.dev.editor = 1;
	
	tile = 1;
	cameraTimer = 0;
	mode = MODE_TILE;
	entIndex = 0;
	selectedEntity = NULL;
	
	nodeListTail = &nodeListHead;
	
	initSDL();
	
	atexit(cleanup);
	
	SDL_ShowCursor(1);
	
	initGame();
	
	handleCommandLine(argc, argv);
	
	entities = initAllEnts(&numEnts);
	entity = entities[0];
	
	loadTiles();
	
	centreOnPlayer();
	
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
