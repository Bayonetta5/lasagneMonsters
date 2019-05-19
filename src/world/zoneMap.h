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

#define CELL_SIZE      64
#define GRID_SPACING   16

extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
extern void clearControl(int type);
extern void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color color, const char *format, ...);
extern Stage *getAdjacentStage(Stage *s, int dx, int dy);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern int getDistance(int x1, int y1, int x2, int y2);
extern int isControl(int type);
extern void initRadar(void (*done)(void));

extern App app;
extern Stage *stage;
extern World world;
