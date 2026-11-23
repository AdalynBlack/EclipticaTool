#pragma once

#include "home_state_struct.h"
#include "game_state.h"
#include "log_parser.h"

static void activate(GtkApplication *app, gpointer user_data);
void home_connect(GtkApplication *app);
static void get_objects(GtkBuilder *builder, struct home_ui *ui);
