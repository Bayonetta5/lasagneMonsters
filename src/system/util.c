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

#include "util.h"

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	app.dev.collisions++;

	return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

int lineLineCollision(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	float denominator, numerator1, numerator2, r, s;

	denominator = ((x2 - x1) * (y4 - y3)) - ((y2 - y1) * (x4 - x3));
	numerator1 = ((y1 - y3) * (x4 - x3)) - ((x1 - x3) * (y4 - y3));
	numerator2 = ((y1 - y3) * (x2 - x1)) - ((x1 - x3) * (y2 - y1));

	if (denominator == 0)
	{
		return numerator1 == 0 && numerator2 == 0;
	}

	r = numerator1 / denominator;
	s = numerator2 / denominator;

	return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
}

int lineRectCollision(int x1, int y1, int x2, int y2, int rx, int ry, int rw, int rh)
{
	return
		lineLineCollision(x1, y1, x2, y2, rx, ry, rx + rw, ry) || // top
		lineLineCollision(x1, y1, x2, y2, rx, ry + rh, rx + rw, ry + rh) || // bottom
		lineLineCollision(x1, y1, x2, y2, rx, ry, rx, ry + rh) || // left
		lineLineCollision(x1, y1, x2, y2, rx + rw, ry, rx + rw, ry + rh) // right
	;
}

void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy)
{
	int steps = MAX(abs(x1 - x2), abs(y1 - y2));

	*dx = *dy = 0;

	if (steps != 0)
	{
		*dx = (x1 - x2);
		*dx /= steps;

		*dy = (y1 - y2);
		*dy /= steps;
	}
}

float getAngle(int x1, int y1, int x2, int y2)
{
	float angle = -90 + atan2(y1 - y2, x1 - x2) * (180 / PI);
	return angle >= 0 ? angle : 360 + angle;
}

int getDistance(int x1, int y1, int x2, int y2)
{
	int x, y;

	x = x2 - x1;
	y = y2 - y1;

	return sqrt(x * x + y *y);
}

unsigned long hashcode(const char *str)
{
    unsigned long hash = 5381;
    int c;

	c = *str;

	while (c)
	{
        hash = ((hash << 5) + hash) + c;

        c = *str++;
	}

	hash = ((hash << 5) + hash);

	return hash;
}

void *resize(void *array, int oldSize, int newSize)
{
	void **newArray;
	int copySize;

	copySize = newSize > oldSize ? oldSize : newSize;

	newArray = malloc(newSize);
	memset(newArray, 0, newSize);
	memcpy(newArray, array, copySize);
	free(array);

	return newArray;
}
