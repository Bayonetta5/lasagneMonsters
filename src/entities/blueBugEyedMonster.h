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

#include "../common.h"

#define RUN_SPEED    4
#define WALK_SPEED   2

extern AtlasImage *getAtlasImage(char *filename, int required);
extern void initBabyBugEyedMonster(Entity *e);
extern void initMonster(Entity *e);
extern void initSlimeBullet(Entity *owner);
extern void lookForPlayer(void);
extern void monsterTick(void);
extern void playPositionalSound(int snd, int ch, int srcX, int srcY, int destX, int destY);
extern int rrnd(int min, int max);
extern Entity *spawnEntity(void);

extern Entity *self;
extern World world;
