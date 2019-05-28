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

#include "entities.h"

static void move(Entity *e);
static void clipToMap(Entity *e);
static int push(Entity *e, float dx, float dy);
static void moveToWorld(Entity *e, float dx, float dy);
static void moveToEntities(Entity *e, float dx, float dy, Entity **candidates, int background);
static int canPush(Entity *e, Entity *other);
static int entityComparator(const void *a, const void *b);

static Entity deadListHead, *deadListTail;

void initEntities(cJSON *root)
{
	memset(&deadListHead, 0, sizeof(Entity));
	deadListTail = &deadListHead;

	loadEntities(cJSON_GetObjectItem(root, "entities"));
}

void doEntities(void)
{
	Entity *e, *prev;

	prev = &stage->entityHead;

	app.dev.collisions = app.dev.ents = 0;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		removeFromQuadtree(e, &world.quadtree);

		app.dev.ents++;

		self = e;

		if (e->tick)
		{
			e->tick();
		}

		if (!(e->flags & EF_STATIC))
		{
			move(e);
		}

		e->cx = e->x + (e->w / 2);
		e->cy = e->y + (e->h / 2);

		if (e->alive != ALIVE_DEAD)
		{
			addToQuadtree(e, &world.quadtree);
		}
		else
		{
			if (e->die)
			{
				e->die();
			}

			if (e == stage->entityTail)
			{
				stage->entityTail = prev;
			}

			prev->next = e->next;

			/* add to dead list */
			if (!(e->flags & EF_DELETE))
			{
				deadListTail->next = e;
				deadListTail = e;
				deadListTail->next = NULL;
			}
			else
			{
				if (e->data)
				{
					free(e->data);
				}

				free(e);
			}

			e = prev;
		}

		prev = e;
	}

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		removeFromQuadtree(e, &world.quadtree);

		if (e->riding != NULL)
		{
			push(e, e->riding->dx, 0);
		}

		if (!(e->flags & (EF_NO_WORLD_CLIP|EF_NO_MAP_BOUNDS)))
		{
			clipToMap(e);
		}

		addToQuadtree(e, &world.quadtree);
	}
}

static void clipToMap(Entity *e)
{
	e->x = MIN(MAX(e->x, stage->bounds.x), stage->bounds.w - (e->w + 16));
	e->y = MIN(MAX(e->y, stage->bounds.y), stage->bounds.h);

	if ((e->x == stage->bounds.x && e->dx < 0) || (e->x == stage->bounds.w - (e->w + 16) && e->dx > 0))
	{
		e->dx = 0;
	}

	if ((e->y == stage->bounds.y && e->dy < 0) || (e->y == stage->bounds.h && e->dy > 0))
	{
		e->dy = 0;
	}
}

static void move(Entity *e)
{
	if (!(e->flags & EF_WEIGHTLESS))
	{
		e->dy += 1.5;
		e->dy = MAX(MIN(e->dy, 18), -999);
	}

	if (e->riding != NULL && e->riding->dy > 0)
	{
		e->dy = e->riding->dy + 1;
	}

	e->riding = NULL;

	e->isOnGround = 0;

	push(e, e->dx, 0);

	push(e, 0, e->dy);
}

static int push(Entity *e, float dx, float dy)
{
	float ex, ey;
	Entity *candidates[MAX_QT_CANDIDATES];

	ex = e->x + dx;
	ey = e->y + dy;

	if (dx != 0 || dy != 0)
	{
		e->x += dx;
		e->y += dy;

		getAllEntsWithin(e->x, e->y, e->w, e->h, candidates, e);

		moveToEntities(e, dx, dy, candidates, 0);

		if (!(e->flags & EF_NO_WORLD_CLIP))
		{
			moveToWorld(e, dx, dy);
		}

		moveToEntities(e, dx, dy, candidates, 1);
	}

	return e->x == ex && e->y == ey;
}

static void moveToWorld(Entity *e, float dx, float dy)
{
	int mx, my, hit, n, adj;

	hit = 0;

	if (dx != 0)
	{
		mx = dx > 0 ? (e->x + e->w) : e->x;
		mx /= TILE_SIZE;

		my = (e->y / TILE_SIZE);

		hit = 0;

		if (isSolidMap(mx, my, &n))
		{
			hit = 1;
		}

		my = (e->y + e->h - 1) / TILE_SIZE;

		if (isSolidMap(mx, my, &n))
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dx > 0 ? -e->w : TILE_SIZE;

			e->x = (mx * TILE_SIZE) + adj;

			if (e->flags & EF_BOUNCES)
			{
				e->dx = -e->dx;
			}
			else
			{
				e->dx = 0;
			}
		}
	}

	if (dy != 0)
	{
		my = dy > 0 ? (e->y + e->h) : e->y;
		my /= TILE_SIZE;

		mx = e->x / TILE_SIZE;

		hit = 0;

		if (isSolidMap(mx, my, &n))
		{
			hit = 1;
		}

		mx = (e->x + e->w - 1) / TILE_SIZE;

		if (isSolidMap(mx, my, &n))
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dy > 0 ? -e->h : TILE_SIZE;

			e->y = (my * TILE_SIZE) + adj;

			if (e->flags & EF_BOUNCES)
			{
				e->dy = -e->dy;
			}
			else
			{
				e->dy = 0;
			}

			if (dy > 0)
			{
				e->isOnGround = 1;

				if (e->flags & EF_FRICTION)
				{
					e->dx *= 0.975;
				}
			}
		}
	}

	if (hit)
	{
		if (e->damage)
		{
			if (n >= TILE_SLIME && n < TILE_FOREGROUND)
			{
				e->damage(3, DT_SLIME);
			}
		}

		if (e->touch)
		{
			e->touch(NULL);
		}
	}
}

static void moveToEntities(Entity *e, float dx, float dy, Entity **candidates, int background)
{
	Entity *other, *oldSelf;
	int adj, i;
	float pushPower;

	for (i = 0, other = candidates[0] ; i < MAX_QT_CANDIDATES && other != NULL ; other = candidates[++i])
	{
		if (other->background == background && (!(other->flags & EF_NO_COLS)) && collision(e->x, e->y, e->w, e->h, other->x, other->y, other->w, other->h))
		{
			if (!(e->flags & EF_NO_ENT_CLIP) && !(other->flags & EF_NO_ENT_CLIP))
			{
				if (canPush(e, other))
				{
					removeFromQuadtree(other, &world.quadtree);

					pushPower = e->flags & EF_SLOW_PUSH ? 0.5f : 1.0f;

					oldSelf = self;

					self = other;

					if (dx != 0)
					{
						if (!push(other, e->dx * pushPower, 0))
						{
							e->x = other->x;

							if (e->dx > 0)
							{
								e->x -= e->w;
							}
							else
							{
								e->x += other->w;
							}
						}
					}

					if (dy != 0)
					{
						if (!push(other, 0, e->dy * pushPower))
						{
							e->y = other->y;

							if (e->dy > 0)
							{
								e->y -= e->h;
							}
							else
							{
								e->y += other->h;
							}
						}
					}

					self = oldSelf;

					addToQuadtree(other, &world.quadtree);
				}

				if (other->flags & EF_SOLID)
				{
					if (dy != 0)
					{
						adj = dy > 0 ? -e->h : other->h;

						e->y = other->y + adj;

						if (e->flags & EF_BOUNCES)
						{
							e->dy = -e->dy;
						}
						else
						{
							e->dy = 0;
						}

						if (dy > 0)
						{
							e->isOnGround = 1;

							if (!(e->flags & EF_WEIGHTLESS))
							{
								e->riding = other;
							}
						}
					}

					if (dx != 0)
					{
						adj = dx > 0 ? -e->w : other->w;

						e->x = other->x + adj;

						if (e->flags & EF_BOUNCES)
						{
							e->dx = -e->dx;
						}
						else
						{
							e->dx = 0;
						}
					}
				}
			}

			if (e->touch)
			{
				e->touch(other);
			}

			if (other->flags & EF_STATIC && other->touch)
			{
				oldSelf = self;

				self = other;

				other->touch(e);

				self = oldSelf;
			}
		}
	}
}

static int canPush(Entity *e, Entity *other)
{
	if (e->flags & EF_SOLID || other->flags & EF_SOLID)
	{
		if (e->flags & EF_PUSH && other->flags & EF_PUSHABLE)
		{
			return 1;
		}
	}

	return 0;
}

void dropToFloor(void)
{
	int onGround;

	onGround = 0;

	for (self = stage->entityHead.next ; self != NULL ; self = self->next)
	{
		addToQuadtree(self, &world.quadtree);

		if (self->init)
		{
			self->init();
		}
	}

	while (!onGround)
	{
		onGround = 1;

		for (self = stage->entityHead.next ; self != NULL ; self = self->next)
		{
			if ((!(self->flags & EF_WEIGHTLESS)) && !self->isOnGround && self != world.player)
			{
				removeFromQuadtree(self, &world.quadtree);

				push(self, 0, 8);

				addToQuadtree(self, &world.quadtree);

				onGround = 0;
			}
		}
	}
}

void drawEntities(int background)
{
	Entity *candidates[MAX_QT_CANDIDATES];
	int i, n;

	getAllEntsWithin(world.camera.x, world.camera.y, SCREEN_WIDTH, SCREEN_HEIGHT, candidates, NULL);

	n = 0;

	for (i = 0, self = candidates[0] ; i < MAX_QT_CANDIDATES && self != NULL ; self = candidates[++i]) {n++;}

	qsort(candidates, n, sizeof(Entity*), entityComparator);

	for (i = 0, self = candidates[0] ; i < MAX_QT_CANDIDATES && self != NULL ; self = candidates[++i])
	{
		if (self->background == background && !(self->flags & EF_INVISIBLE))
		{
			app.dev.drawing++;

			self->draw();
		}
	}
}

void drawEntityLights(void)
{
	Entity *candidates[MAX_QT_CANDIDATES];
	int i;

	getAllEntsWithin(world.camera.x, world.camera.y, SCREEN_WIDTH, SCREEN_HEIGHT, candidates, NULL);

	for (i = 0, self = candidates[0] ; i < MAX_QT_CANDIDATES && self != NULL ; self = candidates[++i])
	{
		if (self->drawLight)
		{
			self->drawLight();
		}
	}
}

void activateEntities(char *targetName, int active)
{
	Entity *e, *oldSelf;

	oldSelf = self;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->activate && strcmp(e->name, targetName) == 0)
		{
			self = e;

			e->activate(active);
		}
	}

	self = oldSelf;
}

void removeFromDeadlist(Entity *candidate)
{
	Entity *e, *prev;

	prev = &deadListHead;

	for (e = deadListHead.next ; e != NULL ; e = e->next)
	{
		if (e == candidate)
		{
			if (e == deadListTail)
			{
				deadListTail = prev;
			}

			prev->next = e->next;
			e->next = NULL;

			e = prev;
		}

		prev = e;
	}
}

void destroyEntities(void)
{
	Entity *e, *prev;

	prev = &stage->entityHead;

	for (e = stage->entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->flags & EF_TRANSIENT)
		{
			if (e == stage->entityTail)
			{
				stage->entityTail = prev;
			}

			prev->next = e->next;
			free(e);
			e = prev;
		}

		prev = e;
	}

	while (deadListHead.next)
	{
		e = deadListHead.next;
		deadListHead.next = e->next;
		free(e->data);
		free(e);
	}

	deadListTail = &deadListHead;
}

static int entityComparator(const void *a, const void *b)
{
	Entity *e1 = *((Entity**)a);
	Entity *e2 = *((Entity**)b);

	return e2->type - e1->type;
}
