#include "test.h"

static void logic(void);
static void draw(void);

static Entity e[NUM], *closest;

void initTest(void)
{
	int i;

	app.delegate.logic = logic;
	app.delegate.draw = draw;

	for (i = 0 ; i < NUM ; i++)
	{
		e[i].x = rand() % 1200;
		e[i].y = rand() % 600;
		e[i].w = 25;
		e[i].h = 25;
	}
}

static void logic(void)
{
	int i, cDist, dist;

	closest = NULL;

	for (i = 0 ; i < NUM ; i++)
	{
		if (lineRectCollision(100, 100, app.mouse.x, app.mouse.y, e[i].x, e[i].y, e[i].w, e[i].h))
		{
			dist = getDistance(100, 100, e[i].x + (e[i].w / 2), e[i].y + (e[i].h / 2));

			if (closest == NULL || dist < cDist)
			{
				closest = &e[i];

				cDist = dist;
			}
		}
	}
}

static void draw(void)
{
	int i;

	for (i = 0 ; i < NUM ; i++)
	{
		drawRect(e[i].x, e[i].y, e[i].w, e[i].h, 0, 255, 0, &e[i] == closest ? 255 : 128);
	}

	drawLine(100, 100, app.mouse.x, app.mouse.y, 255, 255, 255, 255);
}
