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
 * Error Practice Module implementation
 * Focuses on practicing the most frequent error characters (80% rule)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "main.h"
#include "auxiliar.h"
#include "error_logger.h"
#include "error_practice.h"
#include "keyboard.h"

/* Practice state */
static struct {
    gunichar practice_chars[MAX_PRACTICE_CHARS];
    gint count;
    gint total_practiced;
    gint improved;
    gboolean initialized;
} practice = {0};

/* Initialize the error practice module */
void
error_practice_init (void)
{
    if (practice.initialized)
        return;
    
    memset (&practice, 0, sizeof(practice));
    practice.initialized = TRUE;
    
    /* Update the practice set based on current error data */
    error_practice_update_set ();
}

/* Update the practice set based on new Pareto analysis */
void
error_practice_update_set (void)
{
    ErrorCharDetail *top_chars;
    gint top_count;
    gint i;
    
    top_count = error_pareto_get_top_chars (&top_chars);
    
    if (top_count == 0)
    {
        /* No error data, use default practice characters */
        practice.count = 0;
        g_message ("No error data available for practice");
        return;
    }
    
    /* Limit to MAX_PRACTICE_CHARS */
    practice.count = (top_count > MAX_PRACTICE_CHARS) ? MAX_PRACTICE_CHARS : top_count;
    
    /* Copy the top characters to practice set */
    for (i = 0; i < practice.count; i++)
    {
        practice.practice_chars[i] = top_chars[i].uchr;
    }
    
    g_free (top_chars);
    
    g_message ("Practice set updated: %d characters", practice.count);
}

/* Get practice text focusing on top error characters */
gchar *
error_practice_get_text (void)
{
    gchar *text;
    gint i, j;
    gint pos = 0;
    gint word_len;
    
    if (!practice.initialized)
        error_practice_init ();
    
    if (practice.count == 0)
    {
        /* No practice characters, return generic text */
        return g_strdup (_("The quick brown fox jumps over the lazy dog. "));
    }
    
    text = g_malloc0 (PRACTICE_TEXT_LENGTH * 6 + 1);
    
    /* Generate several words with practice characters */
    for (i = 0; i < 50 && pos < PRACTICE_TEXT_LENGTH * 5; i++)
    {
        /* Generate a word */
        word_len = rand () % 6 + 3;  /* 3-8 characters */
        
        for (j = 0; j < word_len && pos < PRACTICE_TEXT_LENGTH * 5; j++)
        {
            gunichar ch;
            
            /* 70% chance to use a practice character */
            if (rand () % 10 < 7 && practice.count > 0)
            {
                ch = practice.practice_chars[rand () % practice.count];
            }
            else
            {
                /* Use a random letter */
                gunichar vowels[] = {'a', 'e', 'i', 'o', 'u'};
                gunichar consonants[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm',
                                        'n', 'p', 'r', 's', 't', 'v', 'w', 'x', 'y', 'z'};
                
                if (j % 2 == 0)
                    ch = vowels[rand () % 5];
                else
                    ch = consonants[rand () % 20];
            }
            
            pos += g_unichar_to_utf8 (ch, text + pos);
        }
        
        /* Add space between words */
        text[pos++] = ' ';
    }
    
    text[pos] = '\0';
    
    return text;
}

/* Get the list of characters being practiced */
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

/* Check if a character is in the practice set */
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

/* Get number of practice characters */
gint
error_practice_get_count (void)
{
    if (!practice.initialized)
        error_practice_init ();
    
    return practice.count;
}

/* Generate a word with practice characters */
void
error_practice_generate_word (gunichar *word, gint max_len)
{
    gint i, j;
    gint word_len;
    
    if (!practice.initialized)
        error_practice_init ();
    
    if (practice.count == 0 || max_len < 1)
    {
        word[0] = L'\0';
        return;
    }
    
    word_len = rand () % (max_len - 1) + 1;
    
    for (i = 0; i < word_len; i++)
    {
        /* 80% chance to use a practice character */
        if (rand () % 10 < 8)
        {
            word[i] = practice.practice_chars[rand () % practice.count];
        }
        else
        {
            /* Use a random vowel */
            gunichar vowels[] = {'a', 'e', 'i', 'o', 'u'};
            word[i] = vowels[rand () % 5];
        }
    }
    
    word[word_len] = L'\0';
}

/* Get practice statistics */
void
error_practice_get_stats (gint *total_practiced, gint *improved)
{
    if (total_practiced)
        *total_practiced = practice.total_practiced;
    
    if (improved)
        *improved = practice.improved;
}
