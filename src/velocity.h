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

#define MAX_WORDS 12123

/*
 * Interface functions
 */
gchar *velo_get_dict_name (void);

void velo_reset_dict (void);

/*
 * Auxiliar functions
 */
void velo_init (void);

void velo_init_dict (gchar *);

void velo_draw_random_words (void);

gchar *velo_filter_utf8 (gchar * text);

void velo_text_write_to_file (gchar * text_raw, gboolean overwrite);

void velo_create_dict (gchar * file_name, gboolean overwrite);

void velo_comment (gdouble accuracy, gdouble velocity);
