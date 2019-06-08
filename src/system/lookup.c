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

#include "lookup.h"

static Lookup head;
static Lookup *tail;

static void addLookup(const char *name, unsigned long value);

void initLookups(void)
{
	memset(&head, 0, sizeof(Lookup));
	tail = &head;

	addLookup("WT_BUTTON", WT_BUTTON);
	addLookup("WT_SELECT", WT_SELECT);
	addLookup("WT_INPUT", WT_INPUT);

	addLookup("STAT_SHOTS_FIRED", STAT_SHOTS_FIRED);
	addLookup("STAT_SHOTS_HIT", STAT_SHOTS_HIT);
	addLookup("STAT_ACCURACY", STAT_ACCURACY);
	addLookup("STAT_SHOTS_DESTROYED", STAT_SHOTS_DESTROYED);
	addLookup("STAT_ZONES_VISITED", STAT_ZONES_VISITED);
	addLookup("STAT_JUMPS", STAT_JUMPS);
	addLookup("STAT_MOVED", STAT_MOVED);
	addLookup("STAT_FALLEN", STAT_FALLEN);
	addLookup("STAT_HEALTH_EATEN", STAT_HEALTH_EATEN);
	addLookup("STAT_HEALTH_LOST", STAT_HEALTH_LOST);
	addLookup("STAT_COINS_COLLECTED", STAT_COINS_COLLECTED);
	addLookup("STAT_COINS_SPENT", STAT_COINS_SPENT);
	addLookup("STAT_RETRIES", STAT_RETRIES);
	addLookup("STAT_GUMBALLS_BOUGHT", STAT_GUMBALLS_BOUGHT);
	addLookup("STAT_CHESTS_OPENED", STAT_CHESTS_OPENED);
	addLookup("STAT_KEYS_FOUND", STAT_KEYS_FOUND);
	addLookup("STAT_PERCENT_COMPLETE", STAT_PERCENT_COMPLETE);
	addLookup("STAT_TIME", STAT_TIME);

	addLookup("DR_NOTHING", DR_NOTHING);
	addLookup("DR_KEY", DR_KEY);
	addLookup("DR_REMOTE", DR_REMOTE);

	addLookup("CT_STANDING", CT_STANDING);
	addLookup("CT_HANGING", CT_HANGING);
	addLookup("CT_SWINGING", CT_SWINGING);

	addLookup("FACING_LEFT", FACING_LEFT);
	addLookup("FACING_RIGHT", FACING_RIGHT);

	addLookup("GO_PROMPT", GO_PROMPT);
	addLookup("GO_RELOAD", GO_RELOAD);
	addLookup("GO_CONTINUE", GO_CONTINUE);

	addLookup("left", CONTROL_LEFT);
	addLookup("right", CONTROL_RIGHT);
	addLookup("up", CONTROL_UP);
	addLookup("down", CONTROL_DOWN);
	addLookup("fire", CONTROL_FIRE);
	addLookup("jump", CONTROL_JUMP);
	addLookup("use", CONTROL_USE);
	addLookup("map", CONTROL_MAP);
	addLookup("pause", CONTROL_PAUSE);
}

static void addLookup(const char *name, unsigned long value)
{
	Lookup *lookup = malloc(sizeof(Lookup));
	memset(lookup, 0, sizeof(Lookup));

	STRNCPY(lookup->name, name, MAX_NAME_LENGTH);
	lookup->value = value;

	tail->next = lookup;
	tail = lookup;
}

char *getLookupName(char *prefix, long num)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (l->value == num && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			return l->name;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value %ld, prefix=%s", num, prefix);

	exit(1);

	return "";
}

unsigned long lookup(const char *name)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (strcmp(l->name, name) == 0)
		{
			return l->value;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value '%s'", name);
	exit(1);

	return 0;
}
