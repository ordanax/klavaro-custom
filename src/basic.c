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

/*
 * Basic course
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "auxiliar.h"
#include "main.h"
#include "keyboard.h"
#include "tutor.h"
#include "basic.h"

#define MAX_BASIC_CHAR_SET (8 * 14)
struct
{
	gint lesson;
	gunichar char_set[MAX_BASIC_CHAR_SET];
	glong char_set_size;
	gboolean lesson_increased;
} basic;

/*******************************************************************************
 * Interface functions
 */
gint
basic_get_lesson ()
{
	return (basic.lesson);
}

void
basic_set_lesson (gint lesson)
{
	if (lesson >= 0)
	{
		basic.lesson = lesson;
		main_preferences_set_int ("tutor", "basic_lesson", lesson);
	}
}

gunichar *
basic_get_char_set ()
{
	return (basic.char_set);
}

gboolean
basic_get_lesson_increased ()
{
	return (basic.lesson_increased);
}

void
basic_set_lesson_increased (gboolean state)
{
	basic.lesson_increased = state;
}

/*******************************************************************************
 * Initialize basic vars
 */
void
basic_init ()
{
	/* Retrieve the last lesson where the student had selected.
	 */
	basic.lesson_increased = FALSE;

	if (main_preferences_exist ("tutor", "basic_lesson"))
		basic.lesson = main_preferences_get_int ("tutor", "basic_lesson");
	else
		basic_set_lesson (1);
	basic_init_char_set ();
}

/**********************************************************************
 * Read the characters to be used with the current basic.lesson
 */
gint
basic_init_char_set ()
{
	gint i, j, k;
	gchar line_str[16];
	gchar *lesson_file;
	gchar *lesson_str;
	gunichar *tmpuc;
	FILE *fh;
	GtkWidget *wg;

	/*
	 * Custom lessons
	 */
	wg = get_wg ("togglebutton_edit_basic_lesson");
	if (basic.lesson > 43 && basic.lesson <= MAX_BASIC_LESSONS)
	{
		lesson_file = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "basic_lesson_%i.txt",
			       	main_path_user (), basic.lesson);
		if (g_file_get_contents (lesson_file, &lesson_str, NULL, NULL))
		{
			tmpuc = g_utf8_to_ucs4_fast (lesson_str, -1, &basic.char_set_size);
			if (basic.char_set_size > MAX_BASIC_CHAR_SET)
				basic.char_set_size = MAX_BASIC_CHAR_SET;
			for (i = j = 0; i < basic.char_set_size; i++)
				if (g_unichar_isgraph (tmpuc[i]))
					basic.char_set[j++] = tmpuc[i];
			basic.char_set[j] = L'\0';
			basic.char_set_size = j;
			g_free (tmpuc);
			g_free (lesson_str);
		}
		else
		{
			basic.char_set[0] = L' ';
			basic.char_set[1] = L' ';
			basic.char_set[2] = L'\0';
			basic.char_set_size = 2;
		}
		g_free (lesson_file);

		gtk_widget_set_sensitive (wg, TRUE);
		return (-1);
	}
	gtk_widget_set_sensitive (wg, FALSE);

	/*
	 * Open the lesson file
	 */
	lesson_file = g_build_filename (main_path_data (), "basic_lessons.txt", NULL);
	fh = (FILE *) g_fopen (lesson_file, "r");
	g_free (lesson_file);
	if (!fh)
		g_error ("couldn't find the basic lessons' file.");

	/*
	 * Search the lesson
	 */
	for (i = 1; i < basic.lesson; i++)
		for (j = 0; j < 11; j++)
			if (!(fgets (line_str, 16, fh)))
				break;

	/*
	 * Pass heading line
	 */
	if (!(fgets (line_str, 16, fh)))
	{
		basic.char_set[0] = L'\0';
		basic.char_set_size = 0;
		fclose (fh);
		return (-1);
	}

	/*
	 * Get chars, lower set
	 */
	for (k = 0, i = 0; i < 4; i++)
	{
		if (!(fgets (line_str, 16, fh)))
		{
			basic.char_set[0] = L'\0';
			basic.char_set_size = 0;
			fclose (fh);
			return (-1);
		}
		for (j = 0; j < 14; j++)
		{
			if (line_str[j] == '1' && g_unichar_isgraph (keyb_get_lochars (i, j)))
				basic.char_set[k++] = g_unichar_tolower (keyb_get_lochars (i, j));
		}
	}

	/*
	 * Pass blank line
	 */
	if (!(fgets (line_str, 16, fh)))
	{
		basic.char_set[0] = L'\0';
		basic.char_set_size = 0;
		fclose (fh);
		return (-1);
	}

	/*
	 * Get chars, upper set
	 */
	for (i = 0; i < 4; i++)
	{
		if (!(fgets (line_str, 16, fh)))
		{
			basic.char_set[0] = L'\0';
			basic.char_set_size = 0;
			fclose (fh);
			return (-1);
		}
		for (j = 0; j < 14; j++)
			if (line_str[j] == '1' && g_unichar_isgraph (keyb_get_upchars (i, j)))
				basic.char_set[k++] = g_unichar_tolower (keyb_get_upchars (i, j));
	}
	fclose (fh);

	basic.char_set[k] = L'\0';
	basic.char_set_size = k;
	return (0);
}

/**********************************************************************
 * Save the lesson's character set defined in the custom lesson entry,
 * for the current lesson
 */
void
basic_save_lesson (gchar * charset)
{
	gchar *lesson_file;
	FILE *fh;

	lesson_file = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "basic_lesson_%i.txt", main_path_user (), basic.lesson);
	fh = (FILE *) g_fopen (lesson_file, "w");
	if (fh == NULL)
		g_warning ("couldn't save the file:\n %s", lesson_file);
	else
	{
		fprintf (fh, "%s", charset);
		fclose (fh);
		if (strlen (charset) < 2)
			g_unlink (lesson_file);
	}
	g_free (lesson_file);
}

/**********************************************************************
 * Put the lesson's characters at the main screen
 */
#define N_LINES 8
void
basic_draw_lesson ()
{
	gint i, j, k, len;
	gint idx, rnd;
	gchar *ut8_tmp;
	gchar *ut8_hack;
	gunichar sentence[9 * 6 + 4];
	gunichar char_pool[2*(N_LINES * 9 * 5)];
	GtkTextBuffer *buf;

	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (get_wg ("text_tutor")));

	len = basic.char_set_size;
	if (len < 2)
	{
		g_warning ("No character set for this lesson.");
		return;
	}

	if (basic.char_set[0] == basic.char_set[1]) 
	{
		ut8_tmp = g_strconcat (_("This custom lesson is empty."
				      " You can define two or more different characters "
				      "to generate a personal practice."
				      " Press the editting button in the upper-right corner."), 
				keyb_get_utf8_paragraph_symbol (), "\n", NULL);
		ut8_hack = g_strstr_len (ut8_tmp, -1, "editting");
		if (ut8_hack != NULL)
		{
			ut8_hack += 4;
			while (*ut8_hack != '\0')
			{
				*ut8_hack = *(ut8_hack+1);
				ut8_hack++;
			}
		}
		gtk_text_buffer_insert_at_cursor (buf, ut8_tmp, -1);
		gtk_text_buffer_insert_at_cursor (buf, ut8_tmp, -1);
		gtk_text_buffer_insert_at_cursor (buf, ut8_tmp, -1);
		gtk_text_buffer_insert_at_cursor (buf, ut8_tmp, -1);
		g_free (ut8_tmp);
		return;
	}

	/*
	 * Draw the lines as sentences
	 */
	memmove (char_pool, basic.char_set, len * sizeof (gunichar));
	if (len > 4 && len < 14)
	{
		memmove (char_pool+len, basic.char_set, len * sizeof (gunichar));
		len *= 2;
	}
	sentence[9 * 6] = L'\n';
	sentence[9 * 6 + 1] = L'\0';
	sentence[9 * 6 + 2] = L'\0';
	for (i = 0; i < N_LINES; i++)
	{			/* lines (sentences) */
		idx = 0;
		for (j = 0; j < 9; j++)
		{		/* words */
			for (k = 0; k < 5; k++)
			{	/* letters */
				rnd = rand () % len;
				sentence[idx++] = char_pool[rnd];
				len--;
				char_pool[rnd] = char_pool[len];
				if (len == 0)
				{
					len = basic.char_set_size;
					memmove (char_pool, basic.char_set, len * sizeof (gunichar));
					if (len > 4 && len < 14)
					{
						memmove (char_pool+len, basic.char_set, len * sizeof (gunichar));
						len *= 2;
					}
				}
				if (keyb_is_diacritic (sentence[idx-1]))
 				{
 					if (tutor_is_tibetan())
 					{
 						if (k > 0 && keyb_is_vowel(sentence[idx-1]) && keyb_is_vowel(sentence[idx-2]) )
						{
 							sentence[idx-1] = TIBETAN_WORD_DELIMITER;
 						}
 					}	
 					else
					{
 						sentence[idx-1] = L' ';
					}
 				}
			}
 			if (j == 8)
 				sentence[idx] = UPSYM;
 			else
				sentence[idx] = tutor_is_tibetan() ? TIBETAN_WORD_DELIMITER : L' ';
			idx++;
		}
		ut8_tmp = g_ucs4_to_utf8 (sentence, -1, NULL, NULL, NULL);
		gtk_text_buffer_insert_at_cursor (buf, ut8_tmp, -1);
		g_free (ut8_tmp);
		if (len == 2 && i >= N_LINES/2-1)
			break;
	}
}

/**********************************************************************
 * Put on the screen the final comments
 */
void
basic_comment (gdouble accuracy)
{
	gchar *tmp_str;
	GtkLabel *wg_label;
	GtkWidget *wg;
	GtkTextBuffer *buf;

	/*
	 * Comments
	 */
	if (accuracy < tutor_goal_accuracy ())
		tmp_str = g_strdup (":-(\n");
	else
	{
		basic_set_lesson (basic.lesson + 1);
		if (basic.lesson > 43)
		{
			if (basic.lesson > MAX_BASIC_LESSONS)
				basic_set_lesson (MAX_BASIC_LESSONS);
			wg_label = GTK_LABEL (get_wg ("label_heading"));
			gtk_label_set_text (wg_label, _("Positions of keys seems to be learned!"));
			tmp_str = g_strdup (_(" Congratulations!\n"
					      " You have accomplished the entire basic course.\n"
					      " Go to the next type of exercise: adaptability.\n"
					      " There you will practice mainly the accuracy.\n"));
		}
		else
			tmp_str = g_strdup (_(" All right, now you got it!\n Go to the next lesson.\n"));

		basic_init_char_set ();
		if (basic.lesson != MAX_BASIC_LESSONS)
			basic.lesson_increased = TRUE;
	}

	wg = get_wg ("text_tutor");
	buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (wg));
	gtk_text_buffer_insert_at_cursor (buf, tmp_str, strlen (tmp_str));
	g_free (tmp_str);
}
