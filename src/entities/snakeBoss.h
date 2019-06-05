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

#define MAX_BODY_PARTS 10

extern void activateEntities(char *targetName, int activate);
extern void addToQuadtree(Entity *e, Quadtree *root);
extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern Entity *findStartPoint(const char *name);
extern float getAngle(int x1, int y1, int x2, int y2);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern int getDistance(int x1, int y1, int x2, int y2);
extern void initAimedSlimeBullet(Entity *owner, Entity *target);
extern void initSlimeBullet(Entity *owner);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void playPositionalSound(int snd, int ch, int srcX, int srcY, int destX, int destY);
extern void removeFromQuadtree(Entity *e, Quadtree *root);
extern int rrnd(int low, int high);
extern Entity *spawnEntity(void);
extern void throwCoins(int x, int y, int n);
extern void throwPusBalls(int x, int y, int n);

extern Entity *self;
extern World world;
