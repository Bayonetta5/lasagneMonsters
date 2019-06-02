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

#include "teleport.h"

static void logic(void);
static void switchStage(void);

static int targetStageNum;

void initTeleport(int s)
{
	targetStageNum = s;

	app.delegate.logic = logic;
}

static void logic(void)
{
	doEntities();

	doParticles();

	if (doWipe())
	{
		switchStage();
	}
}

static Entity *findSavePoint(void)
{
	Entity *e;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_SAVE_POINT)
		{
			return e;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No save point in stage %d", targetStageNum);
	exit(1);

	return NULL;
}

static void switchStage(void)
{
	Walter walter;
	Entity *e;
	int facing;

	memcpy(&walter, world.player->data, sizeof(Walter));

	facing = world.player->facing;

	destroyStage();

	initStage(targetStageNum, WIPE_FADE);

	e = findSavePoint();

	memcpy(world.player->data, &walter, sizeof(Walter));

	world.player->x = e->x;
	world.player->y = e->y;
	((Walter*)world.player)->checkpoint.x = e->x;
	((Walter*)world.player)->checkpoint.y = e->y;
	world.player->facing = facing;

	/* correctly center on player */
	world.player->cx = world.player->x + (world.player->w / 2);
	world.player->cy = world.player->y + (world.player->h / 2);

	doCamera();
}
