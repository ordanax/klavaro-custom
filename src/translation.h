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
 * This definition contains the set of languages configured for
 * localization.
 * The letters in parenthesis must agree with LL_CC,
 * where LL is the language code
 * and CC is the country code.
 * The only exception is for the generic
 * English, which uses the 'C' code.
 * Keep the alphabetic order, because it is put
 * in the interface as done here.
 */

#define LANG_SET \
"العربية (ar) [qwerty_ar]\n" \
"български език (bg) [dvorak_bg]\n" \
" বাংলা (bn) [qwerty_us]\n" \
"བོད་ཡིག (bo) [qwerty_bo]\n" \
"Català (ca) [qwerty_es]\n" \
"Čeština (cs) [qwertz_cz]\n" \
"Dansk (da) [qwerty_dk]\n" \
"Deutsch (de) [qwertz_de]\n" \
"ελληνικά (el) [qwerty_gr]\n" \
"English (C) [qwerty_us]\n" \
"English UK (en_GB) [qwerty_uk]\n" \
"Esperanto (eo) [dvorak_eo_eurokeys]\n" \
"Español (es) [qwerty_es]\n" \
"Euskara (eu) [qwerty_es]\n" \
"Suomen kieli (fi) [qwerty_se]\n"\
"Français (fr) [azerty_fr]\n" \
"Galego (gl) [qwerty_es]\n" \
"Hindi (hi) [hindi_in_kruti]\n" \
"Hrvatski (hr) [qwertz_hr]\n" \
"Magyar (hu) [qwertz_hu]\n" \
"Bahasa Indonesia (id) [qwerty_us]\n" \
"Italiano (it) [qwerty_it]\n" \
"Қазақ (kk) [jtsuken_kk]\n" \
"한국어 (ko) [dubeolsik_kr]\n" \
"Кыргызча (ky) [jtsuken_ru]\n" \
"Bokmål (nb) [qwerty_no]\n" \
"Nederlands (nl) [qwerty_us]\n" \
"ਪੰਜਾਬੀ (pa) [gumurkhi_in_jehlum]\n" \
"Polski (pl) [qwerty_pl_us]\n" \
"Português BR (pt_BR) [qwerty_br_abnt2]\n" \
"Português PT (pt_PT) [qwerty_pt]\n" \
"Русский (ru) [jtsuken_ru]\n" \
"Slovenščina (sl) [qwertz_si]\n" \
"Српски (sr) [qwertz_rs]\n" \
"Svenska (sv) [qwerty_se]\n" \
"Türkçe (tr) [qwerty_tr]\n" \
"తెలుగు (te) [qwerty_us]\n" \
"Українська (uk) [jtsuken_ua]\n" \
"اردو (ur) [qwerty_pk_crulp]\n" \
"Tiếng Việt (vi) [qwerty_us]\n" \
"Wolof (wo) [azerty_fr]\n" \
"简体字 (zh_CN) [qwerty_cn_us]"

#define LANG_NAME_MAX_LEN 60
#define KBD_NAME_MAX_LEN 20
typedef struct
{
	gchar *name;
	gchar *code;
	gchar cd[3];
	gchar *kbd;
} Lang_Name_Code;

void trans_init_lang_name_code (void);

const gchar * trans_code_to_country (gchar *code);

gchar * trans_get_default_keyboard (void);

gchar * trans_get_code (gint i);

gboolean trans_lang_is_available (gchar * test);

gboolean trans_lang_has_stopmark (void);

gboolean trans_is_tibetan (void);

FILE *trans_lang_get_similar_file (const gchar * file_end);

gchar * trans_lang_get_similar_file_name (const gchar * file_end);

void trans_init_language_env (void);

void trans_set_combo_language (void);

gchar *trans_get_current_language (void);

void trans_change_language (gchar *language);

gchar *trans_read_text (const gchar *);
