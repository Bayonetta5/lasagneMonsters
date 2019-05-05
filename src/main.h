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

extern void cleanup(void);
extern void doInput(void);
extern void initGameSystem(void);
extern void initSDL(void);
extern void initStage(int n, int i);
extern void prepareScene(void);
extern void presentScene(void);
extern void loadMusic(char *filename);
extern void playMusic(int loop);
extern void saveGame(void);

App app;
Entity *player;
Entity *self;
Game game;
Stage *stage;
World world;
