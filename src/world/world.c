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

#include "world.h"

static void loadStage(const char *filename);
void loadStageData(cJSON *root);

void initWorld(void)
{
	memset(&world, 0, sizeof(World));

	world.particleTail = &world.particleHead;
	world.gameTextTail = &world.gameTextHead;
	world.stagesTail = &world.stagesHead;
}

Stage *getStage(int i)
{
	Stage *s;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		if (s->id == i)
		{
			return s;
		}
	}

	return NULL;
}

Stage *getAdjacentStage(Stage *s, int dx, int dy)
{
	Stage *other;
	int x, y;

	x = s->x + dx;
	y = s->y + dy;

	for (other = world.stagesHead.next ; other != NULL ; other = other->next)
	{
		if (other->x == x && other->y == y)
		{
			return other;
		}
	}

	return NULL;
}

void loadAllStages(void)
{
	char **filenames, filename[MAX_FILENAME_LENGTH];
	int count, i;

	filenames = getFileList("data/stages", &count);

	for (i = 0 ; i < count ; i++)
	{
		sprintf(filename, "data/stages/%s", filenames[i]);

		loadStage(filename);

		free(filenames[i]);
	}

	free(filenames);
}

static void loadStage(const char *filename)
{
	cJSON *root;
	char *json;
	int id;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...\n", filename);

	json = readFile(getFileLocation(filename));

	root = cJSON_Parse(json);

	id = cJSON_GetObjectItem(root, "id")->valueint;

	if (getStage(id) == NULL)
	{
		loadStageData(root);

		free(json);

		cJSON_Delete(root);
	}
}

void loadStageData(cJSON *root)
{
	stage = malloc(sizeof(Stage));
	memset(stage, 0, sizeof(Stage));
	world.stagesTail->next = stage;
	world.stagesTail = stage;

	stage->id = cJSON_GetObjectItem(root, "id")->valueint;
	stage->visited = app.dev.debug ? 1 : cJSON_GetObjectItem(root, "visited")->valueint;
	stage->x = cJSON_GetObjectItem(root, "x")->valueint;
	stage->y = cJSON_GetObjectItem(root, "y")->valueint;

	initEntities(root);

	initMap(cJSON_GetObjectItem(root, "map"));
}

void updatePlayer(void)
{
	Entity *e;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_PLAYER)
		{
			world.player = e;
			return;
		}
	}

	if (!app.dev.editor)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Player not found in stage: %d", stage->id);
		exit(1);
	}
}
