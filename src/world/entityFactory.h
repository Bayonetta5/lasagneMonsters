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
#include "../json/cJSON.h"

extern void initBBBBB(Entity *e);
extern void initBattery(Entity *e);
extern void initBoots(Entity *e);
extern void initCeilingLamp(Entity *e);
extern void initChest(Entity *e);
extern void initDoor(Entity *e);
extern void initEntity(Entity *e);
extern void initGreenBugEyedMonster(Entity *e);
extern void initGreenFlyingCyclops(Entity *e);
extern void initGreenHorse(Entity *e);
extern void initGumball(Entity *e);
extern void initGumballMachine(Entity *e);
extern void initHeart(Entity *e);
extern void initHorizontalDoor(Entity *e);
extern void initKey(Entity *e);
extern void initWaterBottle(Entity *e);
extern void initLamp(Entity *e);
extern void initPiggyBank(Entity *e);
extern void initPlatform(Entity *e);
extern void initPlayer(Entity *e);
extern void initRedHorse(Entity *e);
extern void initSavePoint(Entity *e);
extern void initStartPoint(Entity *e);
extern void initTrafficLight(Entity *e);
extern void initTransferCube(Entity *e);
extern long lookup(const char *key);

extern Entity *self;
extern Stage *stage;
