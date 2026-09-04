/**************************************************************************/
/*  Klavaro - a flexible touch typing tutor                               */
/*  Copyright (C) 2005-2021 Felipe Emmanuel Ferreira de Castro            */
/*                                                                        */
/*  This file is part of Klavaro, which is a free software: you can       */
/*  redistribute it and/or modify it under the terms of the GNU General   */
/*  Public License as published by the Free Software Foundation, either   */
/*  version 3 of the License, or (at your option) any later version.      */
/*                                                                        */
/*  Klavaro is distributed in the hope that it will be useful,            */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU General Public License for more details (in the file COPYING).    */
/*  You should have received a copy of the GNU General Public License     */
/*  along with Klavaro.  If not, see <https://www.gnu.org/licenses/>      */
/**************************************************************************/

#include <sys/stat.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "auxiliar.h"
#include "main.h"

extern GtkBuilder *gui;

GObject *
get_obj (gchar *name)
{
	GObject *obj;
	obj = gtk_builder_get_object (gui, name);
	if (obj == NULL)
		g_warning ("Object not found: %s", name);
	return (obj);
}

GtkWidget *
get_wg (gchar *name)
{
	GObject *obj;
	obj = gtk_builder_get_object (gui, name);
	if (obj == NULL)
		g_warning ("Widget not found: %s", name);
	return (GTK_WIDGET (obj));
}

GtkWindow *
get_win (gchar *name)
{
	GObject *obj;
	obj = gtk_builder_get_object (gui, name);
	if (obj == NULL)
		g_warning ("Window not found: %s", name);
	return (GTK_WINDOW (obj));
}

/* Set the background color of a widget
 */
void
set_wg_bg_color (GtkWidget * widget, gchar *bg_color) 
{
    static GtkCssProvider *cssp = NULL;
    GtkStyleContext *sc;
    gchar *css_bg_color;

    if (cssp == NULL)
	    cssp = gtk_css_provider_new ();

    css_bg_color = g_strdup_printf (
		    "textview text {background-color: %s;}\n"
		    ".background {background-color: %s;}"
		    , bg_color, bg_color);
    sc = gtk_widget_get_style_context (widget);
    gtk_style_context_remove_provider (sc, GTK_STYLE_PROVIDER (cssp));
    gtk_css_provider_load_from_data (cssp, css_bg_color, -1, NULL);
    if (!gtk_style_context_has_class (sc, "background"))
	    gtk_style_context_add_class (sc, "background");
    gtk_style_context_add_provider (sc, GTK_STYLE_PROVIDER (cssp), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_free (css_bg_color);
}

/* Set an image widget with the name of the file provided in the data dir
 */
void
set_pixmap (gchar *widget, gchar *image)
{
	gchar *tmp;
	GtkImage *img;

	tmp = g_build_filename (main_path_data (), image, NULL);
	img = GTK_IMAGE (get_wg (widget));
	gtk_image_set_from_file (img, tmp);
	g_free (tmp);
}

/* Search for the user directory and create it if not found
 */
void
assert_user_dir ()
{
	GDir *dh;

	dh = g_dir_open (main_path_user (), 0, NULL);
	if (dh == NULL)
	{
		g_message ("creating an empty user folder:\n %s", main_path_user ());
		g_mkdir (main_path_user (), DIR_PERM);
		dh = g_dir_open (main_path_user (), 0, NULL);
		if (dh == NULL)
			g_error ("could not creat a user folder, so we must quit!");
	}
	g_dir_close (dh);
}

/* Compare two strings, so that it applies to other sorting functions.
 */
gint
compare_string_function (gconstpointer a, gconstpointer b)
{
	return (strcasecmp (a, b));
}
