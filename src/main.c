#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <locale.h>

#include "glib.h"
#include "home_state.h"

static void activate(GtkApplication *app, gpointer user_data) {
	GtkCssProvider *provider = gtk_css_provider_new();
	GdkDisplay *display = gdk_display_get_default();
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	const gchar *css_file = "resource:///ecliptica/tool/index/main.css";
	gtk_css_provider_load_from_file(provider, g_file_new_for_uri(css_file));

	g_object_unref(provider);
}

int main(int argc, char *argv[]) {
#ifdef GTK_SRCDIR
	g_chdir (GTK_SRCDIR);
#endif
	setlocale(LC_ALL, "");
	bindtextdomain(GETTEXT_PACKAGE, "po/");
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	GtkApplication *app = gtk_application_new ("ecliptica.tool", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

	home_connect(app);
	
	int status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;
}
