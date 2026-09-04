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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "main.h"
#include "auxiliar.h"
#include "translation.h"
#include "error_logger.h"
#include "error_practice.h"
#include "keyboard.h"

#define PRACTICE_WORDS_PER_SENTENCE 6
#define PRACTICE_SENTENCES 4
#define PRACTICE_WORD_ATTEMPTS 25

static struct {
    gunichar practice_chars[MAX_PRACTICE_CHARS];
    gint count;
    gint total_practiced;
    gint improved;
    gboolean initialized;
} practice = {0};

static GList *practice_word_list = NULL;
static gint practice_word_count = 0;
static gchar *practice_word_buf = NULL;

/* Loads the language word dictionary (e.g. ru.words) */
static void
error_practice_load_dict (void)
{
    gchar *tmp_name;
    gchar *tmp_code;
    gchar *word;

    if (practice_word_list != NULL)
        return;

    tmp_code = main_preferences_get_string ("interface", "language");
    tmp_name = g_strconcat (main_path_data (), G_DIR_SEPARATOR_S, tmp_code, ".words", NULL);
    g_free (tmp_code);

    if (!g_file_test (tmp_name, G_FILE_TEST_IS_REGULAR))
    {
        g_free (tmp_name);
        tmp_name = trans_lang_get_similar_file_name (".words");
        g_message ("error practice: loading dictionary from:\n %s", tmp_name);
    }

    if (g_file_get_contents (tmp_name, &practice_word_buf, NULL, NULL))
    {
        word = practice_word_buf;
        while (*word != '\0')
        {
            gchar *end = word;
            while (*end != '\n' && *end != '\r' && *end != '\0')
                end++;
            if (end == word)
                break;
            *end = '\0';
            practice_word_list = g_list_prepend (practice_word_list, word);
            practice_word_count++;
            word = end + 1;
        }
    }
    else
    {
        g_message ("error practice: could not open dictionary: %s", tmp_name);
    }

    g_free (tmp_name);
}

/* Checks whether a word contains any of the practice characters */
static gboolean
error_practice_word_has_char (const gchar *word)
{
    gint i;
    gchar *lower;
    gboolean found = FALSE;

    lower = g_utf8_strdown (word, -1);
    for (i = 0; i < practice.count; i++)
    {
        gchar utf8[7];
        gint len;
        gchar *lower_char;
        gchar *ptr;

        len = g_unichar_to_utf8 (g_unichar_tolower (practice.practice_chars[i]), utf8);
        utf8[len] = '\0';
        lower_char = g_utf8_strdown (utf8, -1);
        ptr = strstr (lower, lower_char);
        g_free (lower_char);
        if (ptr != NULL)
        {
            found = TRUE;
            break;
        }
    }
    g_free (lower);

    return found;
}

void
error_practice_init (void)
{
    if (practice.initialized)
        return;

    memset (&practice, 0, sizeof(practice));
    practice.initialized = TRUE;

    error_practice_update_set ();
}

void
error_practice_update_set (void)
{
    ErrorCharDetail *top_chars;
    gint top_count;
    gint i;

    top_count = error_pareto_get_top_chars (&top_chars);

    practice.count = 0;

    if (top_count > 0)
    {
        practice.count = (top_count > MAX_PRACTICE_CHARS) ? MAX_PRACTICE_CHARS : top_count;

        for (i = 0; i < practice.count; i++)
        {
            practice.practice_chars[i] = top_chars[i].uchr;
        }

        g_free (top_chars);
    }

    g_message ("Practice set updated: %d characters", practice.count);
}

/* Picks a random word from the dictionary, biased towards words
 * that contain practice characters */
static const gchar *
error_practice_pick_word (void)
{
    gint i;

    if (practice_word_count == 0)
        return "";

    for (i = 0; i < PRACTICE_WORD_ATTEMPTS; i++)
    {
        const gchar *cand =
            (const gchar *) g_list_nth_data (practice_word_list,
                                             rand () % practice_word_count);
        if (error_practice_word_has_char (cand))
            return cand;
    }

    return (const gchar *) g_list_nth_data (practice_word_list,
                                            rand () % practice_word_count);
}

/* Builds a sentence of real dictionary words, keeping words that
 * contain practice characters at the majority of positions */
static void
error_practice_build_sentence (GString *out)
{
    gint i;

    for (i = 0; i < PRACTICE_WORDS_PER_SENTENCE; i++)
    {
        const gchar *word = error_practice_pick_word ();

        if (i > 0)
            g_string_append_c (out, ' ');

        if (i == 0 && *word != '\0')
        {
            const gchar *first = g_utf8_offset_to_pointer (word, 1);
            gchar *upper = g_utf8_strup (word, first - word);
            g_string_append (out, upper);
            g_string_append (out, first);
            g_free (upper);
        }
        else
        {
            g_string_append (out, word);
        }
    }

    g_string_append_c (out, '.');
    g_string_append_c (out, '\n');
}

/* Generates a practice text drawn from the language dictionary. */
gchar *
error_practice_get_text (void)
{
    GString *out;
    gint s;

    if (!practice.initialized)
        error_practice_init ();

    error_practice_load_dict ();

    out = g_string_new (NULL);

    for (s = 0; s < PRACTICE_SENTENCES; s++)
        error_practice_build_sentence (out);

    return g_string_free (out, FALSE);
}

gint
error_practice_get_chars (gunichar **chars)
{
    if (!practice.initialized)
        error_practice_init ();

    if (practice.count == 0)
    {
        *chars = NULL;
        return 0;
    }

    *chars = g_new (gunichar, practice.count);
    memcpy (*chars, practice.practice_chars, practice.count * sizeof(gunichar));

    return practice.count;
}

gboolean
error_practice_is_practice_char (gunichar uchr)
{
    gint i;

    if (!practice.initialized)
        error_practice_init ();

    for (i = 0; i < practice.count; i++)
    {
        if (practice.practice_chars[i] == uchr)
            return TRUE;
    }

    return FALSE;
}

gint
error_practice_get_count (void)
{
    if (!practice.initialized)
        error_practice_init ();

    return practice.count;
}

void
error_practice_generate_word (gunichar *word, gint max_len)
{
    gint i;

    if (!practice.initialized)
        error_practice_init ();

    if (practice.count == 0 || max_len < 1)
    {
        word[0] = L'\0';
        return;
    }

    for (i = 0; i < max_len; i++)
    {
        if (rand () % 10 < 8)
            word[i] = practice.practice_chars[rand () % practice.count];
        else
        {
            gunichar vowels[] = {'a', 'e', 'i', 'o', 'u'};
            word[i] = vowels[rand () % 5];
        }
    }

    word[max_len] = L'\0';
}

void
error_practice_get_stats (gint *total_practiced, gint *improved)
{
    if (total_practiced)
        *total_practiced = practice.total_practiced;

    if (improved)
        *improved = practice.improved;
}