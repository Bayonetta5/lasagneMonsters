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

#define RUN_SPEED    8
#define WALK_SPEED   2

extern AtlasImage *getAtlasImage(char *filename, int required);
extern void lookForPlayer(void);
extern void monsterDie(void);
extern void monsterDraw(void);
extern void monsterDrawLight(void);
extern void monsterSave(cJSON *root);
extern void monsterTakeDamage(int amount, int damageType);
extern void monsterTick(void);
extern void monsterTouch(Entity *other);

extern Entity *self;
extern Stage *stage;
extern World world;
