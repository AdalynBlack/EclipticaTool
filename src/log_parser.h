#pragma once

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <time.h>

#include "home_state_struct.h"
#include "game_state.h"

struct log_parser_state {
	GFile *last_file;
	GDataInputStream *stream;
};

void ownership_cb(char *entity, char *owner, struct home_state *state);
void damage_dealt_cb(struct damage damage, time_t timestamp, struct home_state *state);
void login_cb(char *user, struct home_state *state);
void stage_change_cb(char *stage_name, float phase, char *class, struct home_state *state);
void boss_fight_cb(char *boss_name, float phase, struct home_state *state);

void read_cb(GObject *object, GAsyncResult *result, gpointer user_data);

void logwatch_start(GtkApplication *app, struct home_state *home);
