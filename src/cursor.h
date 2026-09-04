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
 * Time for cursor blinkings, in miliseconds
 */
#define TIME_CURSOR_ON 800
#define TIME_CURSOR_OFF 400

/*
 * Interface functions
 */
gboolean cursor_get_blink (void);

void cursor_set_blink (gboolean status);

/*
 * Auxilaiar functions
 */
void cursor_paint_char (gchar * color_tag_name);

gint cursor_advance (gint n);

gunichar cursor_get_char (void);

gboolean cursor_init (gpointer data);

gboolean cursor_on (gpointer data);

gboolean cursor_off (gpointer data);

void cursor_switch_on (void);

void cursor_switch_off (void);
