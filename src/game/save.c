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

static void saveMap(cJSON *root);
static void saveEntities(cJSON *root);

void saveStage(const char *filename)
{
	char *out;
	cJSON *root;
	
	root = cJSON_CreateObject();
	
	saveEntities(root);
	
	saveMap(root);
	
	out = cJSON_Print(root);
	
	writeFile(filename, out);
	
	cJSON_Delete(root);
	
	free(out);
}

static void saveMap(cJSON *root)
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
			fprintf(fp, "%d ", stage.map[x][y]);
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

static void saveEntities(cJSON *root)
{
	Entity *e;
	cJSON *entityJSON, *entitiesJSON;
	
	entitiesJSON = cJSON_CreateArray();
	
	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (!(e->flags & EF_TRANSIENT))
		{
			self = e;
			
			entityJSON = cJSON_CreateObject();
			
			cJSON_AddStringToObject(entityJSON, "type", e->typeName);
			cJSON_AddNumberToObject(entityJSON, "x", e->x);
			cJSON_AddNumberToObject(entityJSON, "y", e->y);
			
			if (strlen(e->name) > 0)
			{
				cJSON_AddStringToObject(entityJSON, "name", e->name);
			}
			
			if (e->save)
			{
				e->save(entityJSON);
			}
			
			cJSON_AddItemToArray(entitiesJSON, entityJSON);
		}
	}
	
	cJSON_AddItemToObject(root, "entities", entitiesJSON);
}
