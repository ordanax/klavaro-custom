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

#define MAX_PARAGRAPHS 100
#define MAX_PAR_LEN 2001

/*
 * Interface functions
 */
gchar *fluid_get_paragraph_name (void);

void fluid_reset_paragraph (void);

gchar *get_par (gint index);

/*
 * Auxiliar functions
 */
void fluid_init (void);

void fluid_init_paragraph_list (gchar * list_name);

void fluid_draw_random_paragraphs (void);

gchar *fluid_filter_utf8 (gchar * text);

void fluid_text_write_to_file (gchar * text_raw);

void fluid_copy_text_file (gchar * file_name);

void fluid_comment (gdouble accuracy, gdouble velocity, gdouble fluidness);
