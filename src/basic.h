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

#define	MAX_BASIC_LESSONS 256

/*
 * Interface functions
 */
gint basic_get_lesson (void);

void basic_set_lesson (gint lesson);

gunichar *basic_get_char_set (void);

gboolean basic_get_lesson_increased (void);

void basic_set_lesson_increased (gboolean state);

/*
 * Auxiliar functions
 */
void basic_init (void);

gint basic_init_char_set (void);

void basic_save_lesson (gchar * charset);

void basic_draw_lesson (void);

void basic_comment (gdouble accuracy);
