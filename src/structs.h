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

typedef struct Texture Texture;
typedef struct Entity Entity;
typedef struct Quadtree Quadtree;
typedef struct InitFunc InitFunc;
typedef struct Particle Particle;
typedef struct cJSON cJSON;
typedef struct AtlasImage AtlasImage;
typedef struct Lookup Lookup;
typedef struct GameText GameText;
typedef struct Widget Widget;
typedef struct Stage Stage;
typedef struct Node Node;

struct Texture {
	char name[MAX_NAME_LENGTH];
	SDL_Texture *texture;
	Texture *next;
};

struct Lookup {
	char name[MAX_NAME_LENGTH];
	long value;
	Lookup *next;
};

struct InitFunc {
	char id[MAX_NAME_LENGTH];
	void (*init)(Entity *e);
	InitFunc *next;
};

struct AtlasImage {
	char filename[MAX_DESCRIPTION_LENGTH];
	SDL_Texture *texture;
	SDL_Rect rect;
	AtlasImage *next;
};

struct Entity {
	unsigned long id;
	unsigned int type;
	char *typeName;
	char name[MAX_NAME_LENGTH];
	float x;
	float y;
	int w;
	int h;
	int facing;
	float dx;
	float dy;
	int alive;
	int isOnGround;
	int background;
	void (*data);
	AtlasImage *atlasImage;
	void (*init)(void);
	void (*tick)(void);
	void (*draw)(void);
	void (*touch)(Entity *other);
	void (*damage)(int damage);
	void (*activate)(int active);
	void (*die)(void);
	void (*load)(cJSON *root);
	void (*save)(cJSON *root);
	long flags;
	Entity *owner;
	Entity *riding;
	Entity *next;
};

typedef struct {
	float bobValue;
	float by;
	int thinkTime;
	int health;
	int touchDelay;
	int value;
	int frame;
	int frameTime;
} Item;

typedef struct {
	int health;
	int maxHealth;
	int thinkTime;
	int reload;
	int shotsToFire;
	int hitTimer;
	int coins;
	int alert;
	long aiFlags;
} Monster;

typedef struct {
	int immuneTimer;
	int health, maxHealth;
	float ammo, maxAmmo;
	int reload;
	int jumps;
	float refillRate;
	int autoFire;
	int hasDoubleJump;
} Walter;

typedef struct {
	int health;
} Bullet;

typedef struct {
	int sx;
	int sy;
	int ex;
	int ey;
	int open;
	int requires;
	int speed;
} Door;

typedef struct {
	int destinationStage;
	int destinationName[MAX_NAME_LENGTH];
} Portal;

typedef struct {
	int targetStage;
	char targetFlag[MAX_NAME_LENGTH];
	int w, h;
} TransferCube;

typedef struct {
	float sx;
	float sy;
	float ex;
	float ey;
	int speed;
	int pause;
	int pauseTimer;
	int enabled;
	float dx;
	float dy;
} Platform;

typedef struct {
	int on;
	char targetName[MAX_NAME_LENGTH];
} TrafficLight;

typedef struct {
	int active;
	int frame;
	int frameTime;
} SavePoint;

typedef struct {
	int open;
	int coins;
	int delay;
} Chest;

typedef struct {
	int frame;
	int frameTime;
	int requiredCoins;
	int pauseTime;
	char targetName[MAX_NAME_LENGTH];
} PiggyBank;

typedef struct {
	int cageType;
	float swingX;
	float swingY;
	int health;
	int chainX;
	int chainY;
	int hitTimer;
} BBBBB;

struct Particle {
	float x;
	float y;
	float dx;
	float dy;
	int life;
	AtlasImage *atlasImage;
	SDL_Color color;
	int weightless;
	Particle *next;
};

struct Quadtree {
	int depth;
	int x, y, w, h;
	Entity **ents;
	int capacity;
	int numEnts;
	int addedTo;
	Quadtree *node[4];
};

struct GameText {
	char text[MAX_NAME_LENGTH];
	int x, y;
	int health;
	GameText *next;
};

struct Stage {
	int id;
	int map[MAP_WIDTH][MAP_HEIGHT];
	int numMonsters;
	int numGirls;
	Entity entityHead, *entityTail;
	Stage *next;
};

typedef struct {
	Entity *player;
	Stage stagesHead;
	AtlasImage *tiles[MAX_TILES];
	Particle particleHead, *particleTail;
	GameText gameTextHead, *gameTextTail;
	Quadtree quadtree;
	TransferCube *transferCube;
	struct {
		int x;
		int y;
		int minX;
		int maxX;
		int minY;
		int maxY;
	} camera;
} World;

typedef struct {
	unsigned int stageId;
	int coins;
	int keys;
	unsigned int stats[STAT_MAX];
} Game;

struct Widget {
	char name[MAX_NAME_LENGTH];
	char groupName[MAX_NAME_LENGTH];
	char text[MAX_NAME_LENGTH];
	int type;
	int x;
	int y;
	int w;
	int h;
	int value;
	int disabled;
	int visible;
	int numOptions;
	char **options;
	void (*action)(void);
	Widget *prev;
	Widget *next;
};

struct Node {
	Node *next;
	int x, y;
};

typedef struct {
	char saveDir[MAX_FILENAME_LENGTH];
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Texture *backBuffer;
	int keyboard[MAX_KEYBOARD_KEYS];
	int joypadButton[SDL_CONTROLLER_BUTTON_MAX];
	int joypadAxis[JOYPAD_AXIS_MAX];
	Texture texturesHead, *texturesTail;
	Widget widgetsHead, *widgetsTail, *selectedWidget;
	SDL_Joystick *joypad;
	int awaitingWidgetInput;
	int lastKeyPressed;
	int lastButtonPressed;
	struct {
		void (*logic)(void);
		void (*draw)(void);
	} delegate;
	struct {
		SDL_Color red;
		SDL_Color orange;
		SDL_Color yellow;
		SDL_Color green;
		SDL_Color blue;
		SDL_Color cyan;
		SDL_Color purple;
		SDL_Color white;
		SDL_Color black;
		SDL_Color lightGrey;
		SDL_Color darkGrey;
	} colors;
	struct Mouse {
		int x;
		int y;
		int buttons[MAX_MOUSE_BUTTONS];
	} mouse;
	struct Config {
		int winWidth;
		int winHeight;
		int soundVolume;
		int musicVolume;
		int fullscreen;
		int keyControls[CONTROL_MAX];
		int joypadControls[CONTROL_MAX];
		int deadzone;
	} config;
	struct {
		unsigned int wrap;
	} text;
	struct {
		int type;
		int value;
	} wipe;
	struct {
		int saving;
		float pulse;
		AtlasImage *atlasImage;
	} save;
	struct {
		int debug;
		int fps;
		int ents;
		int collisions;
		int drawing;
		int editor;
	} dev;
} App;
