/**************************************************************************/
/*  Klavaro - a flexible touch typing tutor                               */
/*  Copyright (C) 2005-2021 Felipe Emmanuel Ferreira de Castro            */
/*  Error Practice mode: Copyright (C) 2026 ordanax                       */
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
    gboolean single_char_focus;
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

/* Fraction of characters in a word that belong to the practice set (0..1).
 * Used to guarantee that generated texts contain ~80% of error letters. */
static gdouble
error_practice_word_density (const gchar *word)
{
    const gchar *ptr;
    gint total = 0;
    gint matched = 0;

    if (practice.count == 0 || word == NULL || *word == '\0')
        return 0.0;

    for (ptr = word; *ptr != '\0'; ptr = g_utf8_next_char (ptr))
    {
        gunichar uchr = g_utf8_get_char (ptr);
        if (!g_unichar_isalpha (uchr))
            continue;
        total++;
        if (error_practice_is_practice_char (uchr))
            matched++;
    }

    if (total == 0)
        return 0.0;

    return (gdouble) matched / total;
}

/* Minimum practice-char density a dictionary word must have
 * to be used in a generated lesson text */
#define PRACTICE_MIN_DENSITY 0.8

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
error_practice_focus_single_char (gunichar uchr)
{
    if (!practice.initialized)
        error_practice_init ();

    practice.practice_chars[0] = uchr;
    practice.count = 1;
    practice.single_char_focus = TRUE;
}

void
error_practice_update_set (void)
{
    ErrorCharDetail *top_chars;
    gint top_count;
    gint i;

    if (practice.single_char_focus)
        return;

    top_count = error_pareto_get_top_chars (&top_chars);

    practice.count = 0;
    practice.single_char_focus = FALSE;

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

/* Picks a random dictionary word dense enough in practice characters.
 * Returns "" when no suitable word is found, so the caller can
 * generate a synthetic one. */
static const gchar *
error_practice_pick_word (void)
{
    gint i;
    gint best_idx = -1;
    gdouble best_density = 0.0;
    gint nth;

    if (practice_word_count == 0)
        return "";

    if (practice.count == 0)
    {
        gint nth = rand () % practice_word_count;
        if (nth < 0 || nth >= practice_word_count)
            return "";
        return (const gchar *) g_list_nth_data (practice_word_list, nth);
    }

    for (i = 0; i < PRACTICE_WORD_ATTEMPTS; i++)
    {
        nth = rand () % practice_word_count;
        if (nth < 0 || nth >= practice_word_count)
            continue;

        const gchar *cand = (const gchar *) g_list_nth_data (practice_word_list, nth);
        if (cand == NULL)
            continue;

        gdouble density = error_practice_word_density (cand);
        if (density >= PRACTICE_MIN_DENSITY)
            return cand;
        if (density > best_density)
        {
            best_density = density;
            best_idx = nth;
        }
    }

    if (best_idx >= 0 && best_density > 0.0)
        return (const gchar *) g_list_nth_data (practice_word_list, best_idx);

    return "";
}

/* Builds a sentence of words that together contain ~80% of
 * practice characters: dictionary words when available, otherwise
 * synthetic words generated from the practice set. */
static void
error_practice_build_sentence (GString *out)
{
    gint i;

    for (i = 0; i < PRACTICE_WORDS_PER_SENTENCE; i++)
    {
        const gchar *word = error_practice_pick_word ();
        gunichar gen[MAX_PRACTICE_CHARS + 2];
        gboolean used_gen = FALSE;

        if (*word == '\0')
        {
            gint len = 3 + (rand () % 4);
            error_practice_generate_word (gen, len);
            used_gen = TRUE;
        }

        if (i > 0)
            g_string_append_c (out, ' ');

        if (used_gen)
        {
            gchar *u8 = g_ucs4_to_utf8 (gen, -1, NULL, NULL, NULL);
            if (i == 0 && u8 != NULL)
            {
                const gchar *first = g_utf8_offset_to_pointer (u8, 1);
                gchar *upper = g_utf8_strup (u8, first - u8);
                g_string_append (out, upper);
                g_string_append (out, first);
                g_free (upper);
            }
            else
                g_string_append (out, u8 ? u8 : "");
            g_free (u8);
        }
        else if (i == 0 && *word != '\0')
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
    gunichar vowels[20];
    gunichar cons[20];
    gint vlen;
    gint clen;

    if (!practice.initialized)
        error_practice_init ();

    if (max_len < 1)
    {
        word[0] = L'\0';
        return;
    }

    vlen = keyb_get_vowels (vowels);
    clen = keyb_get_consonants (cons);
    if (vlen < 1)
    {
        vowels[0] = L'a';
        vlen = 1;
    }

    for (i = 0; i < max_len; i++)
    {
        if (practice.count > 0 && rand () % 10 < 8)
            word[i] = practice.practice_chars[rand () % practice.count];
        else if (practice.count > 0)
            word[i] = vowels[rand () % vlen];
        else if (clen > 0 && rand () % 3 < 2)
            word[i] = cons[rand () % clen];
        else
            word[i] = vowels[rand () % vlen];
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