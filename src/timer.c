#include "timer.h"

gboolean timer_cb(gpointer user_data) {
	return G_SOURCE_CONTINUE;
};

void timer_activate(struct home_state *state) {
	g_timeout_add(100, timer_cb, state);
}

void set_stage_timer(struct home_state *state, time_t start_time) {
	state->game->stage.start_time = start_time;
}

void set_game_timer(struct home_state *state, time_t start_time) {
	state->game->start_time = start_time;
}
