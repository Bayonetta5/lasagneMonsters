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

#define PI 3.14159265358979323846
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CAROLINE(a,b) (((a)<(b))?(a):(b))
#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720

#define CONFIG_FILENAME    "config.json"
#define SAVE_FILENAME      "game.json"

#define FPS   60

#define MAX_TILES    255

#define TILE_SIZE    48

#define TILE_AIR            0
#define TILE_WALL           1
#define TILE_SLIME          200
#define TILE_FOREGROUND     225

#define MAP_WIDTH    128
#define MAP_HEIGHT   128

#define MAP_RENDER_WIDTH    27
#define MAP_RENDER_HEIGHT   15

#define MAX_TIPS    12

#define MAX_QT_CANDIDATES   256

#define MAX_NAME_LENGTH           32
#define MAX_DESCRIPTION_LENGTH    256
#define MAX_LINE_LENGTH           1024
#define MAX_FILENAME_LENGTH       256
#define MAX_PATH_LENGTH           4096

#define MAX_KEYBOARD_KEYS   350
#define MAX_MOUSE_BUTTONS   6

#define NUM_ATLAS_BUCKETS  32

#define EF_NONE            0
#define EF_WEIGHTLESS      (2 << 0)
#define EF_SOLID           (2 << 1)
#define EF_PUSH            (2 << 2)
#define EF_NO_WORLD_CLIP   (2 << 3)
#define EF_NO_MAP_BOUNDS   (2 << 4)
#define EF_PUSHABLE        (2 << 5)
#define EF_SLOW_PUSH       (2 << 6)
#define EF_NO_ENT_CLIP     (2 << 7)
#define EF_INVISIBLE       (2 << 8)
#define EF_STATIC          (2 << 9)
#define EF_DELETE          (2 << 10)
#define EF_FRICTION        (2 << 11)
#define EF_TRANSIENT       (2 << 12)
#define EF_NO_COLS         (2 << 13)

#define AIF_NONE           0
#define AIF_HALT_AT_EDGE   (2 << 0)
#define AIF_JUMPS          (2 << 1)

#define EPSILON      0.000001

enum
{
	ET_PLAYER,
	ET_MONSTER,
	ET_BULLET,
	ET_ITEM,
	ET_STRUCTURE,
	ET_DOOR,
	ET_PLATFORM,
	ET_TRAP,
	ET_SWITCH,
	ET_START_POINT,
	ET_TRANSFER_CUBE,
	ET_SAVE_POINT,
	ET_LIGHT,
	ET_DECORATION
};

enum
{
	ALIVE_ALIVE,
	ALIVE_DYING,
	ALIVE_DEAD
};

enum
{
	DR_NOTHING,
	DR_KEY,
	DR_REMOTE
};

enum
{
	CT_STANDING,
	CT_HANGING,
	CT_SWINGING
};

enum
{
	WIPE_NONE = -1,
	WIPE_FADE,
	WIPE_IN,
	WIPE_OUT
};

enum
{
	WT_BUTTON,
	WT_SELECT,
	WT_INPUT
};

enum
{
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
};

enum
{
	FACING_RIGHT,
	FACING_LEFT
};

enum
{
	DT_WATER,
	DT_SLIME
};

enum
{
	SND_JUMP,
	SND_SHOOT,
	SND_INJURED,
	SND_SCREAM,
	SND_WATER_HIT,
	SND_COIN,
	SND_KEY,
	SND_DEATH,
	SND_DOOR,
	SND_DOOR_DONE,
	SND_TRAFFIC_LIGHT,
	SND_PUS_SPLAT,
	SND_MONSTER_DIE,
	SND_COMPUTER,
	SND_CHEST,
	SND_CRUNCH,
	SND_BUY,
	SND_SLIME_SHOOT,
	SND_SQUEAK,
	SND_POWERUP,
	SND_NAV,
	SND_NEGATIVE,
	SND_TIP,
	SND_BREAK,
	SND_FANFARE,
	SND_YAY,
	SND_CLAPPING,
	SND_DOOR_MECH,
	SND_MAX
};

enum
{
	CH_PLAYER,
	CH_HIT,
	CH_ITEM,
	CH_SPLAT,
	CH_DOOR,
	CH_UNRESERVED,
	CH_MAX = (CH_UNRESERVED + 32)
};

enum
{
	JOYPAD_AXIS_X,
	JOYPAD_AXIS_Y,
	JOYPAD_AXIS_MAX
};

enum
{
	CONTROL_LEFT,
	CONTROL_RIGHT,
	CONTROL_UP,
	CONTROL_DOWN,
	CONTROL_JUMP,
	CONTROL_FIRE,
	CONTROL_USE,
	CONTROL_MAP,
	CONTROL_PAUSE,
	CONTROL_MAX
};

enum
{
	STAT_TIME,
	STAT_MAX
};
