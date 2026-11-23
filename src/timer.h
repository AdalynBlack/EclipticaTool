#include <gtk/gtk.h>

#include "home_state_struct.h"

void timer_activate(struct home_state *state);
void set_stage_timer(struct home_state *state, time_t start_time);
void set_game_timer(struct home_state *state, time_t start_time);
