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

extern void addSlimeBurstParticles(int x, int y);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern void playPositionalSound(int snd, int ch, int srcX, int srcY, int destX, int destY);
extern Entity *spawnEntity(void);
extern void throwCoins(int x, int y, int n);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern void initAimedSlimeBullet(Entity *owner, Entity *target);
extern void monsterDie(void);
extern void monsterDraw(void);
extern void monsterTakeDamage(int amount);
extern void monsterTick(void);
extern void monsterTouch(Entity *other);
extern void lookForPlayer(void);

extern Entity *self;
extern Stage *stage;
extern World world;