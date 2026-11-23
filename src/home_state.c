#include "home_state.h"

static void activate(GtkApplication *app, gpointer user_data) {
	struct home_state *home_state = user_data;

	/* Construct a GtkBuilder instance and load our UI description */
	GtkBuilder *builder = gtk_builder_new();
	gtk_builder_add_from_resource(builder, "/ecliptica/tool/index/home.ui", NULL);

	/* Connect signal handlers to the constructed widgets. */
	GObject *window = gtk_builder_get_object (builder, "window");
	gtk_window_set_application (GTK_WINDOW (window), app);
	
	get_objects(builder, home_state->ui);

	gtk_widget_set_visible (GTK_WIDGET (window), TRUE);

	/* We do not need the builder any more */
	g_object_unref (builder);
}

void home_connect(GtkApplication *app) {
	struct home_state *home_state = (struct home_state*)calloc(1, sizeof(struct home_state));
	home_state->ui   = (struct home_ui*)          calloc(1, sizeof(struct home_ui));
	home_state->game = (struct game_state*)       calloc(1, sizeof(struct game_state));
	home_state->logs = (struct log_parser_state*) calloc(1, sizeof(struct log_parser_state));

	g_signal_connect(app, "activate", G_CALLBACK(activate), home_state);

	logwatch_start(app, home_state);
}

static void get_objects(GtkBuilder *builder, struct home_ui *ui) {
	ui->target_label = gtk_builder_get_object (builder, "targetLabel");
	ui->damage_label = gtk_builder_get_object (builder, "damageLabel");
	ui->phase_label = gtk_builder_get_object (builder, "phaseLabel");
	ui->boss_label = gtk_builder_get_object (builder, "bossLabel");
	ui->boss_icon = gtk_builder_get_object(builder, "bossIcon");
};

