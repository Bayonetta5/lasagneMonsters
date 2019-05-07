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

#include "entityFactory.h"

static void addInitFunc(const char *id, void (*init)(Entity *e));

static InitFunc initFuncHead, *initFuncTail;
static unsigned long entityId;

void initEntityFactory(void)
{
	memset(&initFuncHead, 0, sizeof(InitFunc));
	initFuncTail = &initFuncHead;

	addInitFunc("chest", initChest);
	addInitFunc("door", initDoor);
	addInitFunc("greenFlyingCyclops", initGreenFlyingCyclops);
	addInitFunc("gumballMachine", initGumballMachine);
	addInitFunc("greenBugEyedMonster", initGreenBugEyedMonster);
	addInitFunc("greenHorse", initGreenHorse);
	addInitFunc("gumball", initGumball);
	addInitFunc("heart", initHeart);
	addInitFunc("horizontalDoor", initHorizontalDoor);
	addInitFunc("key", initKey);
	addInitFunc("piggyBank", initPiggyBank);
	addInitFunc("player", initPlayer);
	addInitFunc("platform", initPlatform);
	addInitFunc("redHorse", initRedHorse);
	addInitFunc("savePoint", initSavePoint);
	addInitFunc("startPoint", initStartPoint);
	addInitFunc("trafficLight", initTrafficLight);
	addInitFunc("transferCube", initTransferCube);

	entityId = 0;
}

static void addInitFunc(const char *id, void (*init)(Entity *e))
{
	InitFunc *initFunc;

	initFunc = malloc(sizeof(InitFunc));
	memset(initFunc, 0, sizeof(InitFunc));
	initFuncTail->next = initFunc;
	initFuncTail = initFunc;

	STRNCPY(initFunc->id, id, MAX_NAME_LENGTH);
	initFunc->init = init;
}

Entity *spawnEntity(void)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage->entityTail->next = e;
	stage->entityTail = e;

	e->id = ++entityId;
	e->facing = FACING_RIGHT;

	return e;
}

static void initEntity(cJSON *root)
{
	char *type;
	InitFunc *initFunc;
	Entity *e;

	type = cJSON_GetObjectItem(root, "type")->valuestring;

	for (initFunc = initFuncHead.next ; initFunc != NULL ; initFunc = initFunc->next)
	{
		if (strcmp(initFunc->id, type) == 0)
		{
			e = spawnEntity();

			STRNCPY(e->name, cJSON_GetObjectItem(root, "name")->valuestring, MAX_NAME_LENGTH);
			e->x = cJSON_GetObjectItem(root, "x")->valueint;
			e->y = cJSON_GetObjectItem(root, "y")->valueint;
			e->facing = lookup(cJSON_GetObjectItem(root, "facing")->valuestring);

			initFunc->init(e);

			if (e->load)
			{
				self = e;

				e->load(root);
			}

			return;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Unknown entity type '%s'", type);
	exit(1);
}

void loadEntities(cJSON *root)
{
	cJSON *node;

	stage->entityTail = &stage->entityHead;

	for (node = root->child ; node != NULL ; node = node->next)
	{
		initEntity(node);
	}
}


/* used by map editor */
Entity **initAllEnts(int *numEnts)
{
	Entity *e, **allEnts;
	InitFunc *initFunc;
	int i;

	*numEnts = 0;

	for (initFunc = initFuncHead.next ; initFunc != NULL ; initFunc = initFunc->next)
	{
		*numEnts = *numEnts + 1;
	}

	allEnts = malloc(sizeof(Entity*) * *numEnts);

	i = 0;

	for (initFunc = initFuncHead.next ; initFunc != NULL ; initFunc = initFunc->next)
	{
		e = malloc(sizeof(Entity));
		memset(e, 0, sizeof(Entity));

		initFunc->init(e);

		allEnts[i++] = e;
	}

	return allEnts;
}

Entity *spawnEditorEntity(const char *type, int x, int y)
{
	InitFunc *initFunc;
	Entity *e;

	for (initFunc = initFuncHead.next ; initFunc != NULL ; initFunc = initFunc->next)
	{
		if (strcmp(initFunc->id, type) == 0)
		{
			e = spawnEntity();

			e->x = x;
			e->y = y;

			initFunc->init(e);

			e->flags &= ~EF_INVISIBLE;

			return e;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Unknown entity type '%s'", type);
	exit(1);
}
