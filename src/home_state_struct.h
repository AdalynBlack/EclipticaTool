#pragma once

#include <gtk/gtk.h>

struct home_state {
	struct home_ui *ui;
	struct game_state *game;
	struct log_parser_state *logs;
};

struct home_ui {
	GObject *target_label;

	GObject *damage_label;
	GObject *stage_damage_label;
	GObject *boss_damage_label;

	GObject *phase_label;
	GObject *boss_label;
	GObject *boss_icon;
};
