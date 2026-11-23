#pragma once

#include <time.h>
#include <string.h>

struct stage_state {
	char *stage;
	char *boss;
	float phase;
	time_t start_time;
};

struct player_state {
	char name[32];
	char class[32];
};

enum DAMAGE_TYPE {
	UNKNOWN = -1,
	STRIKE = 0,
	NON_STRIKE = 1,
};

struct damage {
	int amount;
	enum DAMAGE_TYPE type;
	time_t last_updated;
};

struct damage_state {
	struct damage *per_stage;
	short current_stage;
	short max_stages;

	struct damage *graph;
	short graph_bars;
};

struct game_state {
	struct stage_state stage;
	struct player_state user;
	struct damage_state damage;
	time_t start_time;
};

enum DAMAGE_TYPE to_damage_type(char *damage_string);
