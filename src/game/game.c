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

#include "game.h"

static void initMetaData(void);

void initGame(void)
{
	memset(&game, 0, sizeof(Game));

	initMetaData();
}

int deductCoins(int i)
{
	if (game.coins >= i)
	{
		game.coins -= i;
		return 1;
	}

	return 0;
}

static void initMetaData(void)
{
	char **filenames, filename[MAX_FILENAME_LENGTH], *text;
	int count, i;
	cJSON *root, *node;

	filenames = getFileList("data/stages", &count);

	for (i = 0 ; i < count ; i++)
	{
		sprintf(filename, "data/stages/%s", filenames[i]);

		text = readFile(getFileLocation(filename));

		root = cJSON_Parse(text);

		for (node = cJSON_GetObjectItem(root, "entities")->child ; node != NULL ; node = node->next)
		{
			if (cJSON_GetObjectItem(node, "isMonster"))
			{
				game.totalMonsters++;
			}

			if (strcmp(cJSON_GetObjectItem(node, "type")->valuestring, "bbbbb") == 0)
			{
				game.totalGirls++;
			}
		}

		free(text);

		cJSON_Delete(root);

		free(filenames[i]);
	}

	free(filenames);
}

void destroyGame(void)
{
	Entity *e;
	Stage *s;

	while (world.stagesHead.next)
	{
		s = world.stagesHead.next;

		while (s->entityHead.next)
		{
			e = s->entityHead.next;

			s->entityHead.next = e->next;

			free(e);
		}

		world.stagesHead.next = s->next;

		free(s);
	}
}
