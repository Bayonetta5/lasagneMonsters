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

static void initAllStages(void);

void initWorld(void)
{
	memset(&world, 0, sizeof(World));
	
	world.particleTail = &world.particleHead;
	world.gameTextTail = &world.gameTextHead;
	
	initAllStages();
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

static void initAllStages(void)
{
	char **filenames, filename[MAX_FILENAME_LENGTH];
	int count, i;
	Stage *tail;
	
	tail = &world.stagesHead;

	filenames = getFileList("data/stages", &count);
	
	for (i = 0 ; i < count ; i++)
	{
		stage = malloc(sizeof(Stage));
		memset(stage, 0, sizeof(Stage));
		tail->next = stage;
		tail = stage;
		
		stage->entityTail = &stage->entityHead;
		
		sprintf(filename, "data/stages/%s", filenames[i]);
		
		loadStage(filename);
	
		free(filenames[i]);
	}
	
	free(filenames);
}
