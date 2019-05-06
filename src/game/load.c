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

#include "load.h"

static void loadGameData(cJSON *root);
static void loadStages(cJSON *root);
static void loadStats(cJSON *root);

void loadGame(void)
{
	char filename[MAX_PATH_LENGTH], *json;
	cJSON *root;

	destroyGame();

	initGame();

	sprintf(filename, "%s/%s", app.saveDir, SAVE_FILENAME);

	json = readFile(getFileLocation(filename));

	root = cJSON_Parse(json);

	loadGameData(cJSON_GetObjectItem(root, "game"));

	loadStages(cJSON_GetObjectItem(root, "stages"));

	loadStats(cJSON_GetObjectItem(root, "stats"));

	free(json);

	cJSON_Delete(root);
}

static void loadGameData(cJSON *root)
{
	game.stageId = cJSON_GetObjectItem(root, "stageId")->valueint;
	game.coins = cJSON_GetObjectItem(root, "coins")->valueint;
	game.keys = cJSON_GetObjectItem(root, "keys")->valueint;
	game.autoFire = cJSON_GetObjectItem(root, "autoFire")->valueint;
}

static void loadStages(cJSON *root)
{
	cJSON *node;
	Stage *tail;

	tail = &world.stagesHead;

	for (node = root->child ; node != NULL ; node = node->next)
	{
		stage = malloc(sizeof(Stage));
		memset(stage, 0, sizeof(Stage));
		tail->next = stage;
		tail = stage;

		stage->id = cJSON_GetObjectItem(node, "id")->valueint;

		initMap(cJSON_GetObjectItem(node, "map"));

		initEntities(node);
	}
}

static void loadStats(cJSON *root)
{

}
