#include "../common.h"

#define NUM 25

extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a);
extern int lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh);
extern int getDistance(int x1, int y1, int x2, int y2);

extern App app;
