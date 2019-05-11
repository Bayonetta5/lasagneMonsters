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

#include "draw.h"

static void initColor(SDL_Color *c, int r, int g, int b);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);

void initGraphics(void)
{
	initColor(&app.colors.red, 255, 0, 0);
	initColor(&app.colors.orange, 255, 128, 0);
	initColor(&app.colors.yellow, 255, 255, 0);
	initColor(&app.colors.green, 0, 255, 0);
	initColor(&app.colors.blue, 0, 0, 255);
	initColor(&app.colors.cyan, 0, 255, 255);
	initColor(&app.colors.purple, 255, 0, 255);
	initColor(&app.colors.white, 255, 255, 255);
	initColor(&app.colors.black, 0, 0, 0);
	initColor(&app.colors.lightGrey, 192, 192, 192);
	initColor(&app.colors.darkGrey, 128, 128, 128);

	app.backBuffer = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	app.lightMap = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 144);

	SDL_SetTextureBlendMode(app.lightMap, SDL_BLENDMODE_MOD);
}

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 16, 16, 16, 255);
	SDL_SetRenderTarget(app.renderer, app.lightMap);
	SDL_RenderClear(app.renderer);

	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(app.renderer, app.backBuffer);
	SDL_RenderClear(app.renderer);
}

void drawLightMap(void)
{
	SDL_Rect dest;

	if (!app.dev.disableLights)
	{
		dest.x = 0;
		dest.y = 0;
		dest.w = SCREEN_WIDTH;
		dest.h = SCREEN_HEIGHT;

		SDL_RenderCopy(app.renderer, app.lightMap, NULL, &dest);
	}
}

void presentScene(void)
{
	if (app.dev.debug)
	{
		drawText(SCREEN_WIDTH - 5, SCREEN_HEIGHT - 30, 32, TEXT_RIGHT, app.colors.white, "%dfps | Ents: %d | Cols: %d | Draw: %d", app.dev.fps, app.dev.ents, app.dev.collisions, app.dev.drawing);
	}

	if (app.save.saving)
	{
		if (app.save.atlasImage == NULL)
		{
			app.save.atlasImage = getAtlasImage("gfx/main/saving.png", 1);
		}

		SDL_SetTextureAlphaMod(app.save.atlasImage->texture, 255 * ((1.0 + sin(app.save.pulse)) / 2));
		blitAtlasImage(app.save.atlasImage, SCREEN_WIDTH - (15 + app.save.atlasImage->rect.w), SCREEN_HEIGHT - (15 + app.save.atlasImage->rect.h), 0, SDL_FLIP_NONE);
		SDL_SetTextureAlphaMod(app.save.atlasImage->texture, 255);
	}

	SDL_SetRenderTarget(app.renderer, NULL);
	SDL_RenderCopy(app.renderer, app.backBuffer, NULL, NULL);
	SDL_RenderPresent(app.renderer);
}

void blit(SDL_Texture *texture, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, 0, NULL, flip);
}

void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = atlasImage->rect.w;
	dest.h = atlasImage->rect.h;

	if (center)
	{
		dest.x -= (dest.w / 2);
		dest.y -= (dest.h / 2);
	}

	SDL_RenderCopyEx(app.renderer, atlasImage->texture, &atlasImage->rect, &dest, 0, NULL, flip);
}

void drawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderFillRect(app.renderer, &rect);

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
}

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawRect(app.renderer, &rect);

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
}

void drawLine(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{
	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawLine(app.renderer, x1, y1, x2, y2);

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
}

static void initColor(SDL_Color *c, int r, int g, int b)
{
	memset(c, 0, sizeof(SDL_Color));
	c->r = r;
	c->g = g;
	c->b = b;
	c->a = 255;
}
