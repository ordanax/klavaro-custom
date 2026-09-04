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

#ifdef G_OS_UNIX
# define UNIX_OK TRUE
# define LESSON_FONT "Monospace 14"
# define NORMAL_FONT "Sans 14"
# define DIR_PERM (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#else
# define UNIX_OK FALSE
# define LESSON_FONT "Courier Bold 14"
# define NORMAL_FONT "Comic Sans MS 12"
# undef PACKAGE_LOCALE_DIR
# define PACKAGE_LOCALE_DIR "..\\share\\locale"
# undef PACKAGE_DATA_DIR
# define PACKAGE_DATA_DIR "..\\share"
# define DIR_PERM (0xFFFF)
#endif

# define LESSON_FONT_BO "Tibetan Machine Uni 16"

#define LOCAL TRUE
#define GLOBAL FALSE

/*
 *Interface
 */
gchar * main_path_user (void);
gchar * main_path_stats (void);
gchar * main_path_data (void);
gchar * main_path_score (void);

gboolean main_curl_ok (void);
gboolean main_velo_txt (void);

gboolean main_preferences_exist (gchar *group, gchar *key);
void     main_preferences_remove (gchar *group, gchar *key);
gchar *  main_preferences_get_string (gchar *group, gchar *key);
void     main_preferences_set_string (gchar *group, gchar *key, gchar *value);
gint     main_preferences_get_int (gchar *group, gchar *key);
void     main_preferences_set_int (gchar *group, gchar *key, gint value);
gboolean main_preferences_get_boolean (gchar *group, gchar *key);
void     main_preferences_set_boolean (gchar *group, gchar *key, gboolean value);

gboolean main_altcolor_exist (gchar *group, gchar *key);
gchar *  main_altcolor_get_string (gchar *group, gchar *key);
gboolean main_altcolor_get_boolean (gchar *group, gchar *key);
void     main_altcolor_set_boolean (gchar *group, gchar *key, gboolean value);

void main_preferences_save (void);
void main_window_pass_away (void);

