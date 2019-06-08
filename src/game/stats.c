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

#include "stats.h"

static void logic(void);
static void draw(void);
static void back(void);
static void initStatNames(void);
static void drawArrows(void);
static void drawStats(void);
static void calculate(void);

static void (*oldDraw)(void);
static void (*returnFromStats)(void);
char *statNames[STAT_MAX];
static int start, end;
static AtlasImage *arrow;

void initStats(void (*done)(void))
{
	initStatNames();

	calculate();

	arrow = getAtlasImage("gfx/main/arrow.png", 1);

	app.selectedWidget = getWidget("back", "stats");
	app.selectedWidget->action = back;

	calculateWidgetFrame("stats");

	showWidgets("stats", 1);

	oldDraw = app.delegate.draw;

	returnFromStats = done;

	start = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (app.keyboard[SDL_SCANCODE_UP] || isControl(CONTROL_UP))
	{
		start = MAX(start - 1, 0);
	}

	if (app.keyboard[SDL_SCANCODE_DOWN] || isControl(CONTROL_DOWN))
	{
		start = MIN(start + 1, STAT_TIME - NUM_VISIBLE_STATS);
	}

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		back();
	}

	end = start + NUM_VISIBLE_STATS;

	doWidgets("stats");
}

static void draw(void)
{
	drawStats();

	drawArrows();

	drawWidgetFrame();

	drawWidgets("stats");
}

static void drawArrows(void)
{
	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start > 0)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) - 25, 490, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start < STAT_TIME - 1)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) + 25, 490, 1, SDL_FLIP_VERTICAL);

	SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
}

static void drawStats(void)
{
	int i, y, h, m, s;
	SDL_Rect r;

	r.w = 700;
	r.h = 450;
	r.x = (SCREEN_WIDTH - r.w) / 2;
	r.y = 115;

	oldDraw();

	drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "STATS");

	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 255);
	drawOutlineRect(r.x, r.y, r.w, r.h, 255, 255, 255, 255);

	y = 125;

	for (i = start ; i < end ; i++)
	{
		if (i < STAT_MAX - 1)
		{
			drawText(r.x + 25, y, 48, TEXT_LEFT, app.colors.white, statNames[i]);

			switch (i)
			{
				case STAT_MOVED:
				case STAT_FALLEN:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%dm", game.stats[i] / 24);
					break;

				case STAT_PERCENT_COMPLETE:
				case STAT_ACCURACY:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%d%%", game.stats[i]);
					break;

				default:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%d", game.stats[i]);
					break;
			}

			y += 48;
		}
	}

	h = (game.stats[STAT_TIME] / 3600);
	m = (game.stats[STAT_TIME] / 60) % 60;
	s = game.stats[STAT_TIME] % 60;

	drawText(r.x + 25, r.y + r.h - 50, 48, TEXT_LEFT, app.colors.white, statNames[STAT_TIME]);
	drawText(r.x + r.w - 25, r.y + r.h - 50, 48, TEXT_RIGHT, app.colors.white, "%02d:%02d:%02d", h, m, s);
}

static void back(void)
{
	returnFromStats();
}

static void calculate(void)
{
	Entity *e;
	Stage *s;
	float percent;
	int remainingStatEntities;

	percent = game.stats[STAT_SHOTS_HIT];
	percent /= game.stats[STAT_SHOTS_FIRED];
	percent *= 100;

	game.stats[STAT_ACCURACY] = percent;

	game.stats[STAT_ZONES_VISITED] = 0;

	remainingStatEntities = 0;

	for (s = world.stagesHead.next ; s != NULL ; s = s->next)
	{
		/* ignore these levels */
		if (s->id != 0 && s->id != 999)
		{
			for (e = s->entityHead.next ; e != NULL ; e = e->next)
			{
				switch (e->type)
				{
					case ET_MONSTER:
					case ET_BBBBB:
					case ET_KEY:
					case ET_POWERUP:
						remainingStatEntities++;
						break;

					case ET_CHEST:
						if (!((Chest*)e->data)->found)
						{
							remainingStatEntities++;
						}
						break;

					default:
						break;
				}
			}
		}

		if (s->visited)
		{
			game.stats[STAT_ZONES_VISITED]++;
		}
	}

	percent = game.totalStatEntities - remainingStatEntities;
	percent /= game.totalStatEntities;
	percent *= 100;

	game.stats[STAT_PERCENT_COMPLETE] = percent;
}

static void initStatNames(void)
{
	statNames[STAT_SHOTS_FIRED] = "Shots fired";
	statNames[STAT_SHOTS_HIT] = "Shots hit";
	statNames[STAT_ACCURACY] = "Accuracy";
	statNames[STAT_SHOTS_DESTROYED] = "Shots destroyed";
	statNames[STAT_ZONES_VISITED] = "Zones visited";
	statNames[STAT_JUMPS] = "Times jumped";
	statNames[STAT_MOVED] = "Distance moved";
	statNames[STAT_FALLEN] = "Distance fallen";
	statNames[STAT_HEALTH_EATEN] = "Health picked up";
	statNames[STAT_HEALTH_LOST] = "Total health lost";
	statNames[STAT_COINS_COLLECTED] = "Coins collected";
	statNames[STAT_COINS_SPENT] = "Coins spent";
	statNames[STAT_RETRIES] = "Retries";
	statNames[STAT_GUMBALLS_BOUGHT] = "Gumballs bought";
	statNames[STAT_CHESTS_OPENED] = "Chest opened";
	statNames[STAT_KEYS_FOUND] = "Keys found";
	statNames[STAT_PERCENT_COMPLETE] = "Percent complete";
	statNames[STAT_TIME] = "Time";
}
