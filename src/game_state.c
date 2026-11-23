#include "game_state.h"

enum DAMAGE_TYPE to_damage_type(char *damage_string) {
	if (strcmp(damage_string, "STRIKE") == 0)
		return STRIKE;
	if (strcmp(damage_string, "NON-STRIKE") == 0)
		return NON_STRIKE;
	return UNKNOWN;
}
