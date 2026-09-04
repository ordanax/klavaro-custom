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
#include <math.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "main.h"
#include "auxiliar.h"
#include "keyboard.h"
#include "adaptability.h"
#include "accuracy.h"
#include "plot.h"

/* Touch errors
 */
struct
{
	gunichar uchr;
	gulong wrong;
	gulong correct;
} terror[MAX_CHARS_EVALUATED];
gint terror_n = 0;

/* Touch times
 */
struct
{
	gunichar uchr;
	gdouble dt[MAX_TT_SAVED + 2];
	gint idx;
} ttime[MAX_CHARS_EVALUATED];
gint ttime_n = 0;

/* Simple reset
 */
void
accur_terror_reset ()
{
	memset (&terror, 0, sizeof (terror));
	terror_n = 0;
}

void
accur_ttime_reset ()
{
	memset (&ttime, 0, sizeof (ttime));
	ttime_n = 0;
}

void
accur_reset ()
{
	accur_terror_reset ();
	accur_ttime_reset ();
}

gint
accur_terror_n_get (void)
{
	return (terror_n);
}

gint
accur_ttime_n_get (void)
{
	return (ttime_n);
}

gchar *
accur_terror_char_utf8 (gint i)
{
	gchar *utf8;
	gint n;

	if (i < 0 || i >= terror_n)
		return (g_strdup (" "));

	utf8 = g_malloc (UTF8_BUFFER);
	n = g_unichar_to_utf8 (terror[i].uchr, utf8);
	if (n < 1)
		return (g_strdup (" "));
	utf8[n] = '\0';

	return (utf8);
}

gchar *
accur_ttime_char_utf8 (gint i)
{
	gchar *utf8;
	gint n;

	if (i < 0 || i >= ttime_n)
		return (g_strdup (" "));

	utf8 = g_malloc (UTF8_BUFFER);
	n = g_unichar_to_utf8 (ttime[i].uchr, utf8);
	if (n < 1)
		return (g_strdup (" "));
	utf8[n] = '\0';

	return (utf8);
}

gulong
accur_wrong_get (gint i)
{
	if (i < 0 || i >= terror_n)
		return -1;

	return (terror[i].wrong);
}

void
accur_ttime_print (void)
{
	gint i, j;

	g_printf ("\n");
	for (i = 0; i < ttime_n; i++)
	{
		g_printf ("%2i - %C - (%02i)", i, ttime[i].uchr, ttime[i].idx);
		for (j = 0; j < ttime[i].idx; j+=1)
			g_printf (" %2i - %.2lf", j, ttime[i].dt[j]);
		g_printf ("\n");
	}
}

/**********************************************************************
 * Open the accuracy accumulators
 */
void
accur_init ()
{
	gint i, j;
	gchar *tmp;
	gchar tmpchr[UTF8_BUFFER];
	gchar *kb_name;
	gchar *data;
	gchar *dtp;
	gint nok;
	gboolean success;
	gunichar uchr;
	gulong wrong;
	gulong correct;
	gdouble dt, dummy;
	gsize len;

	accur_reset ();

	kb_name = g_strdup (keyb_get_name ());
	for (i=0; kb_name[i]; i++)
		kb_name[i] = (kb_name[i] == ' ') ? '_' : kb_name[i];

	/*
	 * First, the accuracy log
	 */
	tmp = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, ACCUR_LOG_FILE, "_", kb_name, NULL);
	success = g_file_get_contents (tmp, &data, &len, NULL);
	if (!success)
		g_message ("Empty accuracy log: %s", tmp);
	else if (len > 0)
	{
		dtp = data;
		for (i = 0; i < MAX_CHARS_EVALUATED; i++)
		{
			if (3 != sscanf (dtp, "%6s\t%lu\t%lu\n", tmpchr, &wrong, &correct)) 
			{
				g_warning ("Accuracy file input error!");
				break;
			}
			uchr = g_utf8_get_char_validated (tmpchr, -1);
			if (uchr == (gunichar)-1 || uchr == (gunichar)-2) 
			{
				g_warning ("Accuracy file UTF-8 input error!");
				break;
			}

			if (1234 > wrong && wrong > 0 && correct <= ERROR_INERTIA)
			{
				terror[i].uchr = uchr;
				terror[i].wrong = wrong;
				terror[i].correct = correct;
				terror_n = i + 1;
			}
			else
				break;
			while (dtp[0] != '\0' && dtp[0] != '\n') dtp++;
			if (dtp[0] == '\n') dtp++;
			if (dtp[0] == '\0') break;
		}
		g_free (data);
	}
	g_free (tmp);

	/*
	 * Second, the proficiency log
	 */
	tmp = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, PROFI_LOG_FILE, "_", kb_name, NULL);
	success = g_file_get_contents (tmp, &data, &len, NULL);
	if (!success)
		g_message ("Empty proficiency log: %s", tmp);
	else if (len > 0)
	{
		dtp = data;
		for (i = 0; i < MAX_CHARS_EVALUATED; i++)
		{
			if (3 != sscanf (dtp, "%6s\t%lf\t%lf\n", tmpchr, &dt, &dummy)) 
			{
				g_warning ("Proficiency file input error!");
				break;
			}

			uchr = g_utf8_get_char_validated (tmpchr, -1);
			if (uchr == (gunichar)-1 || uchr == (gunichar)-2) 
			{
				g_warning ("Proficiency file UTF-8 input error!");
				break;
			}

			if (dt > 0)
			{
				ttime[i].uchr = uchr;
				for (j = 0; j < 3; j++)
					ttime[i].dt[j] = dt;
				ttime[i].idx = 3;
				ttime_n = i + 1;
			}
			else
				break;

			while (dtp[0] != '\0' && dtp[0] != '\n') dtp++;
			if (dtp[0] == '\n') dtp++;
			if (dtp[0] == '\0') break;
		}
		g_free (data);
	}
	g_free (tmp);
	g_free (kb_name);
	/* accur_ttime_print (); */
}

/**********************************************************************
 * Accumulates correctly typed characters
 */
void
accur_correct (gunichar uchr, double touch_time)
{
	gint i, j;

	if (uchr == L' ' || uchr == UPSYM || uchr == L'\t' || uchr == L'\b')
		return;
	if (!keyb_is_inset (uchr))
		return;

	/*
	 * First, accuracy
	 */
	for (i = 0; i < terror_n; i++)
	{
		if (uchr == terror[i].uchr)
		{
			if (terror[i].correct > ERROR_INERTIA)
			{
				terror[i].wrong -= (terror[i].wrong == 0 ? 0 : 1);
				terror[i].correct = 1;
			}
			else
				terror[i].correct++;
			break;
		}
	}

	/*
	 * Second, proficiency
	 */
	if (touch_time < 0.001)
		return;

	/* If uchar is in the pool, add the touch time to it and return */
	uchr = g_unichar_tolower (uchr);
	for (i = 0; i < ttime_n; i++)
	{
		if (uchr == ttime[i].uchr)
		{
			ttime[i].dt[ttime[i].idx] = touch_time;
			ttime[i].idx++;
			if (ttime[i].idx >= (MAX_TT_SAVED - 1))
				ttime[i].idx = 0;
			return;
		}
	}

	/* else, if there is room, */ 
	if (ttime_n >= MAX_CHARS_EVALUATED)
		return;

	/* include a new key in the pool */
	ttime[i].uchr = uchr;
	ttime[i].dt[0] = touch_time;
	for (j = 1; j < MAX_TT_SAVED; j++)
		ttime[i].dt[j] = 0.0;
	ttime[i].idx = 1;
	ttime_n++;
}

/**********************************************************************
 * Accumulates mistyped characters
 */
void
accur_wrong (gunichar uchr)
{
	gint i;
	gint i_min = -1;
	gdouble correct_min = 1e9;
	gdouble hlp;

	if (uchr == L' ' || uchr == UPSYM || uchr == L'\t' || uchr == L'\b')
		return;
	if (!keyb_is_inset (uchr))
		return;

	/*
	 * Only for accuracy
	 */
	for (i = 0; i < terror_n; i++)
	{
		if (uchr == terror[i].uchr)
		{
			terror[i].wrong++;
			return;
		}
		hlp = ((gdouble) terror[i].wrong) / ((gdouble) terror[i].correct + terror[i].wrong + 1);
		if (hlp <= correct_min)
		{
			correct_min = hlp;
			i_min = i;
		}
	}

	if (terror_n < MAX_CHARS_EVALUATED)
	{
		i = terror_n;
		terror_n++;
	}
	else
	{
		i = (i_min > -1 ? i_min : terror_n - 1);
	}

	terror[i].uchr = uchr;
	terror[i].wrong = 1;
	terror[i].correct = 1;
}

gulong
accur_error_total ()
{
	gint i;
	gulong n = 0;

	for (i = 0; i < terror_n; i++)
		n += (terror[i].wrong < 12345 ? terror[i].wrong : 0);

	return n;
}

gdouble
accur_profi_aver (gint idx)
{
	gint i, n;
	gdouble sum;

	if (idx < 0)
		return -10;

	/* Simple average */
	n = 0; sum = 0;
	for (i = 0; i < MAX_TT_SAVED; i++)
	{
		if (ttime[idx].dt[i] > 0.0 && ttime[idx].dt[i] < 3.0)
		{
			sum += ttime[idx].dt[i];
			n++;
		}
	}
	if (n == 0)
		return 0.0;
	
	return (sum / n);
}

gint
accur_profi_aver_norm (gint idx)
{
	gint norm;

	if (idx < 0)
		return -1;
	if (accur_profi_aver (ttime_n-1) == 0)
		return -3;

	norm = rint (accur_profi_aver (idx) / accur_profi_aver (ttime_n-1));
	
	return norm;
}

/*******************************************************************************
 * Sorting first: decreasing wrongness; second: increasing correctness
 */
void
accur_terror_sort ()
{
	gint i, j;
	gunichar uchr;
	gulong correct;
	gulong wrong;

	for (i = 1; i < terror_n; i++)
	{
		for (j = i; j > 0; j--)
		{
			if (terror[j].correct < terror[j-1].correct)
			{
				uchr = terror[j].uchr;
				terror[j].uchr = terror[j-1].uchr;
				terror[j-1].uchr = uchr;

				wrong = terror[j].wrong;
				terror[j].wrong = terror[j-1].wrong;
				terror[j-1].wrong = wrong;

				correct = terror[j].correct;
				terror[j].correct = terror[j-1].correct;
				terror[j-1].correct = correct;
			}
		}
	}
	for (i = 1; i < terror_n; i++)
	{
		for (j = i; j > 0; j--)
		{
			if (terror[j].wrong > terror[j-1].wrong)
			{
				uchr = terror[j].uchr;
				terror[j].uchr = terror[j-1].uchr;
				terror[j-1].uchr = uchr;

				wrong = terror[j].wrong;
				terror[j].wrong = terror[j-1].wrong;
				terror[j-1].wrong = wrong;

				correct = terror[j].correct;
				terror[j].correct = terror[j-1].correct;
				terror[j-1].correct = correct;
			}
		}
	}
}

/*******************************************************************************
 * Decreasing order, touch time 
 */
void
accur_ttime_sort ()
{
	gint i, j, k;
	gunichar uchr;
	gdouble dt;
	gint idx;

	for (i = 1; i < ttime_n; i++)
	{
		for (j = i; j > 0; j--)
		{
			if (accur_profi_aver (j) > accur_profi_aver (j-1))
			{
				uchr = ttime[j].uchr;
				ttime[j].uchr = ttime[j-1].uchr;
				ttime[j-1].uchr = uchr;

				idx = ttime[j].idx;
				ttime[j].idx = ttime[j-1].idx;
				ttime[j-1].idx = idx;

				for (k = 0; k < MAX_TT_SAVED; k++)
				{
					if (ttime[j].dt[k] == ttime[j-1].dt[k])
						continue;
					dt = ttime[j].dt[k];
					ttime[j].dt[k] = ttime[j-1].dt[k];
					ttime[j-1].dt[k] = dt;
				}
			}
		}
	}

	for (i = ttime_n - 1; i > -1; i--)
	{
		if (accur_profi_aver (i) < 0.001)
		{
			ttime[i].uchr = 0;
			for (j = 0; j < MAX_TT_SAVED; j++)
				ttime[i].dt[j] = 0;
			ttime[i].idx = 0;
			ttime_n--;
		}
		else
			break;
	}
	/* accur_ttime_print (); */
}

void
accur_sort ()
{
	accur_terror_sort ();
	accur_ttime_sort ();
}

/*******************************************************************************
 * Creates a random weird word based on error profile
 */
gboolean
accur_create_word (gunichar word[MAX_WORD_LEN + 1])
{
	gint i, j;
	gint ind;
	gint n;
	gunichar vowels[20];
	gunichar last = 0;
	gint vlen;
	gboolean ptype_terror;
	gboolean ptype_ttime;
	static gint profile_type = 0;

	if (terror_n < 10 && ttime_n < 5)
		return FALSE;

	ptype_terror = accur_error_total () >= ERROR_LIMIT;
	if (ttime_n < 40)
		ptype_ttime = 0;
	else
		ptype_ttime = ((accur_profi_aver(9)/accur_profi_aver(39)) > PROFI_LIMIT);
	vlen = keyb_get_vowels (vowels);
	n = rand () % (MAX_WORD_LEN) + 1;
	for (i = 0; i < n; i++)
	{
		if (profile_type == 0)
			profile_type = ptype_ttime ? 1 : 0;
		else
			profile_type = ptype_terror ? 0 : 1;

		if (profile_type == 0) /* Error */
		{
			for (j = 0; j < 100; j++)
			{
				ind = rand () % terror_n;
				if (terror[ind].uchr == last)
					continue;
				if (rand () % terror[0].wrong < terror[ind].wrong)
					break;
			}
			word[i] = terror[ind].uchr;
		}
		else /* Time */
		{
			for (j = 0; j < 100; j++)
			{
				ind = rand () % ttime_n;
				if (ttime[ind].uchr == last)
					continue;
				if ((rand () % accur_profi_aver_norm (0)) < accur_profi_aver_norm (ind))
					break;
			}
			word[i] = ttime[ind].uchr;
		}
		last = word[i];

		/* Avoid double diacritics
		 */
		if (i > 0)
			if (keyb_is_diacritic (word[i - 1]) && keyb_is_diacritic (word[i]))
			{
				word[i] = vowels[rand () % vlen];
				last = word[i];
			}
	}
	/*
	 * Null terminated unistring
	 */
	word[n] = L'\0';

	return TRUE;
}

/*******************************************************************************
 * Saves the accuracy accumulator
 */
void
accur_close ()
{
	gint i;
	gchar *kb_name;
	gchar *tmp;
	gchar *utf8;
	FILE *fh;

	accur_sort ();

	kb_name = g_strdup (keyb_get_name ());
	for (i=0; kb_name[i]; i++)
		kb_name[i] = (kb_name[i] == ' ') ? '_' : kb_name[i];

	/*
	 * First, the accuracy log
	 */
	tmp = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, ACCUR_LOG_FILE, "_", kb_name, NULL);
	fh = g_fopen (tmp, "wb");
	g_free (tmp);
	if (fh)
	{
		for (i = 0; i < terror_n; i++)
		{
			if (terror[i].wrong == 0 || terror[i].correct > ERROR_INERTIA)
				continue;

			utf8 = accur_terror_char_utf8 (i);
			g_fprintf (fh, "%s\t%2lu\t%2lu\n", utf8, terror[i].wrong, terror[i].correct);
			g_free (utf8);
		}
		fclose (fh);
	}
	else
		g_message ("Could not save an accuracy log file at %s", main_path_stats ());

	/*
	 * Second, the proficiency log
	 */
	tmp = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, PROFI_LOG_FILE, "_", kb_name, NULL);
	fh = g_fopen (tmp, "wb");
	g_free (tmp);
	if (fh)
	{
		for (i = 0; i < ttime_n; i++)
		{
			utf8 = accur_ttime_char_utf8 (i);
			g_fprintf (fh, "%s", utf8);
			//g_printf ("%s", utf8);
			g_free (utf8);
			g_fprintf (fh, "\t%0.6f\t%0.4f\n",
					accur_profi_aver (i),
					accur_profi_aver (i) / accur_profi_aver (ttime_n-1));
			//g_printf ("\t%0.6f\t%0.4f\n", accur_profi_aver (i), accur_profi_aver (i) / accur_profi_aver (ttime_n-1));
		}
		fclose (fh);
	}
	else
		g_message ("Could not save a proficiency log file at %s", main_path_stats ());

	g_free (kb_name);
}
