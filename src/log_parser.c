#include "log_parser.h"

void ownership_cb(char *entity, char *owner, struct home_state *state) {
	char label[1024];

	sprintf(label, _("%s: %s"), entity, owner);

	if (strcmp(owner, state->game->user.name) == 0)
		gtk_widget_set_name(GTK_WIDGET(state->ui->target_label), "target-self");
	else
		gtk_widget_set_name(GTK_WIDGET(state->ui->target_label), "target-other");

	gtk_label_set_text(GTK_LABEL(state->ui->target_label), label);
}

void damage_dealt_cb(struct damage damage, time_t timestamp, struct home_state *state) {
	char label[512];

	if (state->game->damage.graph == NULL) {
		state->game->damage.graph_bars = 10;
		state->game->damage.graph = (struct damage*) calloc(10, sizeof(struct damage));
	}

	if (state->game->damage.max_stages <= state->game->damage.current_stage) {
		state->game->damage.per_stage = realloc(state->game->damage.per_stage, sizeof(struct stage_state*) * (state->game->damage.current_stage + 1));
		
		size_t num_new_stages = 1 + state->game->damage.current_stage - state->game->damage.max_stages;
		memset(state->game->damage.per_stage + state->game->damage.max_stages, 0, num_new_stages * sizeof(struct damage*));

		state->game->damage.max_stages = state->game->damage.current_stage + 1;
	}

	state->game->damage.per_stage[state->game->damage.current_stage].amount += damage.amount;
	state->game->damage.per_stage[state->game->damage.current_stage].last_updated = damage.last_updated;


	struct damage *latest_damage = &state->game->damage.graph[0];

	if (latest_damage->last_updated != timestamp) {
		
		latest_damage->amount = 0;
		latest_damage->type = UNKNOWN;
		latest_damage->last_updated = timestamp;

		for (int i = state->game->damage.graph_bars; i > 0; i--) {
			state->game->damage.graph[i] = state->game->damage.graph[i - 1];
		}

		state->game->damage.graph[0] = *latest_damage;
	}

	latest_damage->amount += damage.amount;

	sprintf(label, _("%d %s Damage Dealt"), latest_damage->amount, damage.type == STRIKE ? C_("Damage Type", "DIRECT") : C_("Damage Type", "INDIRECT"));
	gtk_label_set_text(GTK_LABEL(state->ui->damage_label), label);

	sprintf(label, _("%d Stage Damage"), state->game->damage.per_stage[state->game->damage.current_stage].amount);
	gtk_label_set_text(GTK_LABEL(state->ui->damage_label), label);
}

void login_cb(char *user, struct home_state *state) {
	strncpy(state->game->user.name, user, 32);
	g_message("User is \"%s\"", user);
}

static void set_phase(float phase, struct home_state *state) {
	char label[256];
	sprintf(label, _("Phase: %2.1f%%"), phase * 100);
	gtk_label_set_text(GTK_LABEL(state->ui->phase_label), label);
}

void stage_change_cb(char *stage_name, float phase, char *class, struct home_state *state) {
	set_phase(phase, state);
}

void boss_fight_cb(char *boss_name, float phase, struct home_state *state) {
	char label[256];

	sprintf(label, "assets/bosses/%s.png", boss_name);
	gtk_image_set_from_file(GTK_IMAGE(state->ui->boss_icon), label);

	set_phase(phase, state);
}

void read_cb(GObject *object, GAsyncResult *result, gpointer user_data) {
	struct home_state *state = user_data;
	GDataInputStream *stream = (GDataInputStream*)object;

	gsize bytes_read;
	GError *error = NULL;

	char* line = g_data_input_stream_read_line_finish_utf8(stream, result, &bytes_read, &error);
	if (error != NULL)
		g_error("g_input_stream_read_finish (%d): %s", error->code, error->message);

	if (line == NULL)
		return;
	g_message("%s", line);

	char level[32];
	char message[1024];
	
	struct tm tm;
	char *strptime_ret = strptime(line, "%Y.%m.%d %H:%M:%S", &tm);
	if (strptime_ret == NULL) {
		g_data_input_stream_read_line_async(stream, G_PRIORITY_DEFAULT, NULL, read_cb, user_data);
		return;
	}
	line = strptime_ret;
	time_t timestamp = mktime(&tm);

	int num = sscanf(line, "%s%*[ -]%1023[^\n]", level, message);
	if (num == 2) {
		char entity[32];
		char player[33];

		int damage_amt;
		char damage_type[11];

		char stage[32];
		float phase;
		char class[20];

		if (2 == sscanf(message,
				"ownership of %s transferred to %32[^\r\n]",
					entity,
					player)) {
			ownership_cb(entity, player, state);

		} else if (2 == sscanf(message,
				"Dealing %d %10[^ ] damage",
					&damage_amt,
					damage_type)) {
			struct damage damage = {.amount = damage_amt, .type = to_damage_type(damage_type)};
			damage_dealt_cb(damage, timestamp, state);

		} else if (1 == sscanf(message,
				"[Authentication - %*[^]]] Preforming post login checks for %32[^(]%*s)",
					player)) {
			// Replace the trailing space with a null terminator
			player[strlen(player) - 1] = '\0';
			login_cb(player, state);

		} else if (3 == sscanf(message,
				"ECLIPTICA - now in stage: %31[^:] %f as class: %19s",
					stage,
					&phase,
					class)) {
			stage_change_cb(stage, phase, class, state);

		} else if (1 == sscanf(message,
				"ECLIPTICA - now fighting boss: %31[^(] on phase: %*f",
					entity)) {
			boss_fight_cb(entity, phase, state);

		} else
			g_debug("Unparsed Line: %s - %s", level, message);
	}

	g_data_input_stream_read_line_async(stream, G_PRIORITY_DEFAULT, NULL, read_cb, user_data);
}

static void check_files(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data) {
	struct home_state *state = user_data;
	struct log_parser_state *logs = state->logs;
	GError *error = NULL;

	if (event_type != G_FILE_MONITOR_EVENT_CHANGED)
		return;
	
	if (!g_file_query_exists(file, NULL)) {
		return;
	}

	if (logs->last_file == NULL || !g_file_equal(file, logs->last_file)) {
		logs->last_file = g_file_dup(file);

		GFileIOStream *stream = g_file_open_readwrite(file, NULL, &error);
		if (error != NULL)
			g_error("g_file_open_readwrite (%d): %s", error->code, error->message);

		if (logs->stream != NULL)
			g_free(logs->stream);

		GInputStream *input_stream = g_io_stream_get_input_stream(&stream->parent_instance);
		logs->stream = g_data_input_stream_new(input_stream);
	}

	if (logs->stream != NULL)
		g_data_input_stream_read_line_async(logs->stream, G_PRIORITY_DEFAULT, NULL, read_cb, user_data);
}

char *find_logs() {
	char *logs = "/home/adi/.steam/steam/steamapps/compatdata/vrchat/users/steamuser/AppData/LocalLow/VRChat/VRChat/";
	return logs;
};

void logwatch_start(GtkApplication *app, struct home_state *home_state) {
	const char* log_directory = find_logs();

	GFile *file = g_file_new_for_path(log_directory);
	GError *error = NULL;
	GFileMonitor *monitor = g_file_monitor_directory(file, G_FILE_MONITOR_NONE, NULL, &error);

	if (error != NULL)
		g_error("g_file_monitor_directory (%d): %s", error->code, error->message);

	g_signal_connect(monitor, "changed", G_CALLBACK(check_files), home_state);
}
