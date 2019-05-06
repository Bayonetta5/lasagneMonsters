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

#include "save.h"

static void saveGameData(cJSON *root);
static void saveStages(cJSON *root);
static void saveMap(Stage *s, cJSON *root);
static void saveEntities(Stage *s, cJSON *root);
static void saveStats(cJSON *root);

void saveGame(void)
{
	char filename[MAX_PATH_LENGTH], *out;
	cJSON *root;

	sprintf(filename, "%s/%s", app.saveDir, SAVE_FILENAME);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Saving game '%s' ...", filename);

	root = cJSON_CreateObject();

	saveGameData(root);

	saveStages(root);

	saveStats(root);

	out = cJSON_Print(root);

	writeFile(filename, out);

	cJSON_Delete(root);

	free(out);
}

static void saveGameData(cJSON *root)
{
	cJSON *gameJSON;

	gameJSON = cJSON_CreateObject();

	cJSON_AddNumberToObject(gameJSON, "stageId", game.stageId);
	cJSON_AddNumberToObject(gameJSON, "coins", game.coins);
	cJSON_AddNumberToObject(gameJSON, "keys", game.keys);
	cJSON_AddNumberToObject(gameJSON, "autoFire", game.autoFire);

	cJSON_AddItemToObject(root, "game", gameJSON);
}

static void saveStages(cJSON *root)
{
	Stage *s;
	cJSON *stagesJSON, *stageJSON;

	stagesJSON = cJSON_CreateArray();

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		stageJSON = cJSON_CreateObject();

		cJSON_AddNumberToObject(stageJSON, "id", s->id);

		saveEntities(s, stageJSON);

		saveMap(s, stageJSON);

		cJSON_AddItemToArray(stagesJSON, stageJSON);
	}

	cJSON_AddItemToObject(root, "stages", stagesJSON);
}

static void saveEntities(Stage *s, cJSON *root)
{
	Entity *e;
	cJSON *entityJSON, *entitiesJSON;

	entitiesJSON = cJSON_CreateArray();

	for (e = s->entityHead.next ; e != NULL ; e = e->next)
	{
		if (!(e->flags & EF_TRANSIENT))
		{
			self = e;

			entityJSON = cJSON_CreateObject();

			cJSON_AddStringToObject(entityJSON, "type", e->typeName);
			cJSON_AddStringToObject(entityJSON, "name", e->name);
			cJSON_AddNumberToObject(entityJSON, "x", e->x);
			cJSON_AddNumberToObject(entityJSON, "y", e->y);
			cJSON_AddStringToObject(entityJSON, "facing", getLookupName("FACING_", e->facing));

			if (e->save)
			{
				e->save(entityJSON);
			}

			cJSON_AddItemToArray(entitiesJSON, entityJSON);
		}
	}

	cJSON_AddItemToObject(root, "entities", entitiesJSON);
}

static void saveMap(Stage *s, cJSON *root)
{
	int x, y;
	unsigned long dLen, eLen, cLen;
	FILE *fp;
	char *buff, *cData;
	cJSON *mapJSON;

	fp = open_memstream(&buff, &dLen);

	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			fprintf(fp, "%d ", s->map[x][y]);
		}
	}

	fclose(fp);

	cData = compressData(buff, &eLen, &cLen);

	mapJSON = cJSON_CreateObject();

	cJSON_AddStringToObject(mapJSON, "data", cData);
	cJSON_AddNumberToObject(mapJSON, "eLen", eLen);
	cJSON_AddNumberToObject(mapJSON, "cLen", cLen);
	cJSON_AddNumberToObject(mapJSON, "dLen", dLen);

	cJSON_AddItemToObject(root, "map", mapJSON);

	free(buff);

	free(cData);
}

static void saveStats(cJSON *root)
{
	cJSON *statsJSON, *statJSON;
	int i;

	statsJSON = cJSON_CreateArray();

	for (i = 0 ; i < STAT_MAX ; i++)
	{
		statJSON = cJSON_CreateObject();

		cJSON_AddStringToObject(statJSON, "key", getLookupName("STAT_", i));
		cJSON_AddNumberToObject(statJSON, "value", game.stats[i]);

		cJSON_AddItemToArray(statsJSON, statJSON);
	}

	cJSON_AddItemToObject(root, "stats", statsJSON);
}

/* used by map editor */
void saveStage(const char *filename)
{
	char *out;
	cJSON *root;

	root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "id", stage->id);

	saveEntities(stage, root);

	saveMap(stage, root);

	out = cJSON_Print(root);

	writeFile(filename, out);

	cJSON_Delete(root);

	free(out);
}
