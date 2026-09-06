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

/*
 * Error Practice Module for Klavaro
 * Focuses on practicing the most frequent error characters (80% rule)
 */

#ifndef ERROR_PRACTICE_H
#define ERROR_PRACTICE_H

#include <glib.h>

#define MAX_PRACTICE_CHARS 20
#define PRACTICE_TEXT_LENGTH 500

/* Initialize the error practice module */
void error_practice_init (void);

/* Get practice text focusing on top error characters */
gchar * error_practice_get_text (void);

/* Get the list of characters being practiced */
gint error_practice_get_chars (gunichar **chars);

/* Check if a character is in the practice set */
gboolean error_practice_is_practice_char (gunichar uchr);

/* Get number of practice characters */
gint error_practice_get_count (void);

/* Update the practice set based on new Pareto analysis */
void error_practice_update_set (void);

/* Generate a word with practice characters */
void error_practice_generate_word (gunichar *word, gint max_len);

/* Get practice statistics */
void error_practice_get_stats (gint *total_practiced, gint *improved);

#endif /* ERROR_PRACTICE_H */
