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

#include "sound.h"

static void loadSounds(void);
static void channelDone(int c);
static int findFreeChannel(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;
static int channelVolumes[CH_MAX];
static char *randomMusicFiles[] = {
	"music/30thstreet.mp3",
	"music/collesium.mp3",
	"music/hades.mp3",
	"music/lightdark.mp3",
	"music/Monsterous.ogg",
	"music/phil.mp3",
	"music/Zander Noriega - Darker Waves.mp3"
};

static char *prevMusic;
static int nextRandom;

void initSounds(void)
{
	int i;

	memset(sounds, 0, sizeof(Mix_Chunk*) * SND_MAX);

	music = NULL;

	loadSounds();

	for (i = 0 ; i < CH_MAX ; i++)
	{
		channelVolumes[i] = 0;
	}

	Mix_ChannelFinished(channelDone);

	prevMusic = NULL;

	nextRandom = 0;
}

void loadMusic(char *filename)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(filename);
}

void playMusic(int loop)
{
	Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void playRandomStageMusic(void)
{
	int n;
	char *m;

	if (SDL_GetTicks() > nextRandom)
	{
		n = rand() % (sizeof(randomMusicFiles) / sizeof(char*));

		m = randomMusicFiles[n];

		if (m != prevMusic)
		{
			loadMusic(m);

			playMusic(1);

			prevMusic = m;
		}
	}

	/* wait a least one minute before changing */
	nextRandom = SDL_GetTicks() + (1000 * 60);
}

void playSound(int id, int channel)
{
	int c;

	if (channel == -1)
	{
		channel = findFreeChannel();
	}

	c = Mix_PlayChannel(channel, sounds[id], 0);

	if (c != -1)
	{
		Mix_SetPosition(c, 0, 0);
		Mix_SetDistance(c, 0);
	}
}

void playPositionalSound(int id, int channel, int srcX, int srcY, int destX, int destY)
{
	float distance, bearing, vol;

	distance = getDistance(destX, destY, srcX, srcY);

	if (distance <= SCREEN_WIDTH)
	{
		vol = 255;
		vol /= SCREEN_WIDTH;
		vol *= distance;

		if (channel == -1)
		{
			channel = findFreeChannel();
		}

		if (vol >= channelVolumes[channel])
		{
			Mix_PlayChannel(channel, sounds[id], 0);

			if (distance >= SCREEN_WIDTH / 8)
			{
				bearing = 360 - getAngle(srcX, srcY, destX, destY);
				Mix_SetPosition(channel, (Sint16)bearing, (Uint8)vol);
			}
			else
			{
				Mix_SetDistance(channel, vol);
			}
		}
	}
}

static int findFreeChannel(void)
{
	int i;

	for (i = CH_UNRESERVED ; i < CH_MAX ; i++)
	{
		if (!Mix_Playing(i))
		{
			return i;
		}
	}

	return CH_UNRESERVED + (rand() % (CH_MAX - CH_UNRESERVED));
}

void pauseSound(void)
{
	Mix_Pause(-1);
}

void resumeSound(void)
{
	Mix_Resume(-1);
}

static void channelDone(int c)
{
	channelVolumes[c] = 0;
}

static void loadSounds(void)
{
	sounds[SND_JUMP] = Mix_LoadWAV("sound/331381__qubodup__public-domain-jump-sound.ogg");
	sounds[SND_COIN] = Mix_LoadWAV("sound/135936__bradwesson__collectcoin.ogg");
	sounds[SND_KEY] = Mix_LoadWAV("sound/mortice_key_drop_on_concrete_floor.ogg");
	sounds[SND_DEATH] = Mix_LoadWAV("sound/death.ogg");
	sounds[SND_SHOOT] = Mix_LoadWAV("sound/258047__jagadamba__water-spraying-from-a-bottle-02.mp3");
	sounds[SND_WATER_HIT] = Mix_LoadWAV("sound/446115__justinvoke__wet-splat.ogg");
	sounds[SND_TRAFFIC_LIGHT] = Mix_LoadWAV("sound/264446__kickhat__open-button-1.ogg");
	sounds[SND_PUS_SPLAT] = Mix_LoadWAV("sound/87535__flasher21__splat.ogg");
	sounds[SND_MONSTER_DIE] = Mix_LoadWAV("sound/447930__breviceps__step-on-a-slug-splat-2.ogg");
	sounds[SND_INJURED] = Mix_LoadWAV("sound/163441__under7dude__man-getting-hit.ogg");
	sounds[SND_SCREAM] = Mix_LoadWAV("sound/270447__littlerobotsoundfactory__scream-male-02.ogg");
	sounds[SND_DOOR] = Mix_LoadWAV("sound/145702__d-w__brick-drag-concrete-01.ogg");
	sounds[SND_DOOR_DONE] = Mix_LoadWAV("sound/15901__someonesilly__knock.ogg");
	sounds[SND_COMPUTER] = Mix_LoadWAV("sound/377083__cribbler__turn-on-printer.ogg");
	sounds[SND_CHEST] = Mix_LoadWAV("sound/chest.ogg");
	sounds[SND_CRUNCH] = Mix_LoadWAV("sound/393816__thedcheck__crunching-chips.ogg");
	sounds[SND_BUY] = Mix_LoadWAV("sound/210604__18hiltc__sci-fi-weapon-reloading.ogg");
	sounds[SND_SLIME_SHOOT] = Mix_LoadWAV("sound/434479__dersuperanton__splatter.ogg");
	sounds[SND_SQUEAK] = Mix_LoadWAV("sound/232017__danmitch3ll__squeaky-dog-toy.ogg");
	sounds[SND_POWERUP] = Mix_LoadWAV("sound/162467__kastenfrosch__gotitem.ogg");
	sounds[SND_NAV] = Mix_LoadWAV("sound/nav.ogg");
	sounds[SND_NEGATIVE] = Mix_LoadWAV("sound/negative.ogg");
	sounds[SND_TIP] = Mix_LoadWAV("sound/tip.ogg");
	sounds[SND_BREAK] = Mix_LoadWAV("sound/371094__mccormick-iain__glass-break-1.ogg");
	sounds[SND_FANFARE] = Mix_LoadWAV("sound/449069__ricniclas__fanfare.ogg");
	sounds[SND_YAY] = Mix_LoadWAV("sound/69338__timtube__crowd-yay.ogg");
	sounds[SND_CLAPPING] = Mix_LoadWAV("sound/209792__alterr__applause-mono-24bit-48khz.ogg");
	sounds[SND_DOOR_MECH] = Mix_LoadWAV("sound/148309__keemocore__robot-step-short-medium-heavy-edit.ogg");
}

void destroySounds(void)
{
	int i;

	for (i = 0 ; i < SND_MAX ; i++)
	{
		if (sounds[i])
		{
			Mix_FreeChunk(sounds[i]);
		}
	}

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}
}
