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

#include "common.h"

#define GRID_SPACING   64
#define CELL_SIZE      128
#define CAM_SPEED      8

extern void cleanup(void);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void doInput(void);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color color, const char *format, ...);
extern void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a);
extern void initGameSystem(void);
extern void initSDL(void);
extern void loadAllStages(void);
extern void prepareScene(void);
extern void presentScene(void);
extern void saveStage(char *filename);
extern Stage *getStage(int i);

App app;
Entity *player;
Entity *self;
Game game;
Stage *stage;
World world;
