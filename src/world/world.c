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

static Stage *loadStage(int i, Stage *tail);

void initWorld(void)
{
	memset(&world, 0, sizeof(World));

	world.particleTail = &world.particleHead;
	world.gameTextTail = &world.gameTextHead;
}

Stage *getStage(int i)
{
	Stage *s, *tail;

	tail = &world.stagesHead;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		tail = s;

		if (s->id == i)
		{
			return s;
		}
	}

	return loadStage(i, tail);
}

static Stage *loadStage(int i, Stage *tail)
{
	cJSON *root;
	char *json, filename[MAX_FILENAME_LENGTH];

	sprintf(filename, "data/stages/%03d.json", i);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Loading %s ...\n", filename);

	if (fileExists(filename))
	{
		json = readFile(getFileLocation(filename));

		root = cJSON_Parse(json);

		stage = malloc(sizeof(Stage));
		memset(stage, 0, sizeof(Stage));
		tail->next = stage;
		tail = stage;

		stage->id = cJSON_GetObjectItem(root, "id")->valueint;

		initMap(cJSON_GetObjectItem(root, "map"));

		initEntities(root);

		free(json);

		cJSON_Delete(root);

		return stage;
	}

	if (!app.dev.editor)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such stage: %d", i);
		exit(1);
	}

	return NULL;
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
