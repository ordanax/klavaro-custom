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
 * Error logging and Pareto analysis implementation
 * Tracks detailed error statistics and identifies top 80% error characters
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <time.h>

#include "main.h"
#include "error_logger.h"

/* Global error statistics storage */
static struct {
    ErrorCharDetail chars[MAX_ERROR_CHARS];
    gint count;
    gboolean initialized;
} error_stats = {0};

/* Compare function for qsort - sort by wrong_count descending */
static gint
compare_by_wrong_count (gconstpointer a, gconstpointer b)
{
    const ErrorCharDetail *ca = (const ErrorCharDetail *)a;
    const ErrorCharDetail *cb = (const ErrorCharDetail *)b;
    
    if (cb->wrong_count > ca->wrong_count) return 1;
    if (cb->wrong_count < ca->wrong_count) return -1;
    return 0;
}

/* Compare function for qsort - sort by error_rate descending */
static gint
compare_by_error_rate (gconstpointer a, gconstpointer b)
{
    const ErrorCharDetail *ca = (const ErrorCharDetail *)a;
    const ErrorCharDetail *cb = (const ErrorCharDetail *)b;
    
    if (cb->error_rate > ca->error_rate) return 1;
    if (cb->error_rate < ca->error_rate) return -1;
    return 0;
}

/* Find or create entry for a character */
static ErrorCharDetail *
find_or_create_entry (gunichar uchr)
{
    gint i;
    gchar utf8[8];
    gint n;
    
    /* Search for existing entry */
    for (i = 0; i < error_stats.count; i++)
    {
        if (error_stats.chars[i].uchr == uchr)
            return &error_stats.chars[i];
    }
    
    /* Create new entry if space available */
    if (error_stats.count >= MAX_ERROR_CHARS)
        return NULL;
    
    i = error_stats.count;
    error_stats.count++;
    
    /* Initialize new entry */
    error_stats.chars[i].uchr = uchr;
    n = g_unichar_to_utf8 (uchr, utf8);
    utf8[n] = '\0';
    strcpy (error_stats.chars[i].utf8, utf8);
    error_stats.chars[i].wrong_count = 0;
    error_stats.chars[i].correct_count = 0;
    error_stats.chars[i].total_attempts = 0;
    error_stats.chars[i].error_rate = 0.0;
    error_stats.chars[i].is_pareto_top = FALSE;
    
    return &error_stats.chars[i];
}

/* Initialize the error logging system */
void
error_logger_init (void)
{
    if (error_stats.initialized)
        return;
    
    memset (&error_stats, 0, sizeof(error_stats));
    error_stats.initialized = TRUE;
    
    /* Try to load existing error log */
    error_logger_load ();
}

/* Log a detailed error occurrence */
void
error_log_error (gunichar expected, gunichar typed, const gchar *context)
{
    ErrorCharDetail *entry;
    
    if (!error_stats.initialized)
        error_logger_init ();
    
    /* We log the EXPECTED character (the one that should have been typed) */
    entry = find_or_create_entry (expected);
    if (entry)
    {
        entry->wrong_count++;
        entry->total_attempts = entry->wrong_count + entry->correct_count;
        if (entry->total_attempts > 0)
            entry->error_rate = (gdouble)entry->wrong_count / (gdouble)entry->total_attempts;
    }
}

/* Log a correct typing occurrence */
void
error_log_correct (gunichar typed)
{
    ErrorCharDetail *entry;
    
    if (!error_stats.initialized)
        error_logger_init ();
    
    entry = find_or_create_entry (typed);
    if (entry)
    {
        entry->correct_count++;
        entry->total_attempts = entry->wrong_count + entry->correct_count;
        if (entry->total_attempts > 0)
            entry->error_rate = (gdouble)entry->wrong_count / (gdouble)entry->total_attempts;
    }
}

/* Save error log to file */
void
error_logger_save (void)
{
    FILE *fh;
    gchar *tmp_name;
    gchar *kb_name;
    gint i;
    
    if (!error_stats.initialized || error_stats.count == 0)
        return;
    
    kb_name = g_strdup (keyb_get_name ());
    for (i = 0; kb_name[i]; i++)
        kb_name[i] = (kb_name[i] == ' ') ? '_' : kb_name[i];
    
    tmp_name = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, 
                           ERROR_LOG_FILE, "_", kb_name, NULL);
    
    fh = g_fopen (tmp_name, "w");
    if (fh)
    {
        /* Write header */
        fprintf (fh, "Klavaro Error Detail Log\n");
        fprintf (fh, "Keyboard: %s\n", kb_name);
        fprintf (fh, "Date: %s\n", ctime(&(time_t){time(NULL)}));
        fprintf (fh, "\n");
        
        /* Write character statistics */
        fprintf (fh, "Char\tUTF8\tWrong\tCorrect\tTotal\tErrorRate\n");
        for (i = 0; i < error_stats.count; i++)
        {
            fprintf (fh, "%C\t%s\t%lu\t%lu\t%lu\t%.4f\n",
                    error_stats.chars[i].uchr,
                    error_stats.chars[i].utf8,
                    error_stats.chars[i].wrong_count,
                    error_stats.chars[i].correct_count,
                    error_stats.chars[i].total_attempts,
                    error_stats.chars[i].error_rate);
        }
        
        fclose (fh);
        g_message ("Error detail log saved to: %s", tmp_name);
    }
    else
    {
        g_warning ("Could not save error detail log to: %s", tmp_name);
    }
    
    g_free (tmp_name);
    g_free (kb_name);
}

/* Load error log from file */
void
error_logger_load (void)
{
    FILE *fh;
    gchar *tmp_name;
    gchar *kb_name;
    gchar line[256];
    gchar utf8[8];
    gulong wrong, correct, total;
    gdouble error_rate;
    gint i;
    
    if (!error_stats.initialized)
        return;
    
    kb_name = g_strdup (keyb_get_name ());
    for (i = 0; kb_name[i]; i++)
        kb_name[i] = (kb_name[i] == ' ') ? '_' : kb_name[i];
    
    tmp_name = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, 
                           ERROR_LOG_FILE, "_", kb_name, NULL);
    
    fh = g_fopen (tmp_name, "r");
    if (fh)
    {
        /* Skip header lines */
        fgets (line, sizeof(line), fh);  /* "Klavaro Error Detail Log" */
        fgets (line, sizeof(line), fh);  /* "Keyboard: ..." */
        fgets (line, sizeof(line), fh);  /* "Date: ..." */
        fgets (line, sizeof(line), fh);  /* empty line */
        fgets (line, sizeof(line), fh);  /* "Char\tUTF8\tWrong\t..." */
        
        /* Read data lines */
        while (fgets (line, sizeof(line), fh) && error_stats.count < MAX_ERROR_CHARS)
        {
            if (sscanf (line, "%s\t%s\t%lu\t%lu\t%lu\t%lf", 
                       &error_stats.chars[error_stats.count].uchr,
                       utf8, &wrong, &correct, &total, &error_rate) == 6)
            {
                error_stats.chars[error_stats.count].wrong_count = wrong;
                error_stats.chars[error_stats.count].correct_count = correct;
                error_stats.chars[error_stats.count].total_attempts = total;
                error_stats.chars[error_stats.count].error_rate = error_rate;
                strcpy (error_stats.chars[error_stats.count].utf8, utf8);
                error_stats.count++;
            }
        }
        
        fclose (fh);
        g_message ("Loaded %d error entries from: %s", error_stats.count, tmp_name);
    }
    
    g_free (tmp_name);
    g_free (kb_name);
}

/* Perform Pareto analysis on accumulated errors */
ParetoAnalysis *
error_pareto_analyze (void)
{
    ParetoAnalysis *analysis;
    gint i;
    gulong cumulative_errors = 0;
    
    if (!error_stats.initialized || error_stats.count == 0)
        return NULL;
    
    analysis = g_new0 (ParetoAnalysis, 1);
    analysis->count = error_stats.count;
    
    /* Copy and sort characters by wrong_count (descending) */
    for (i = 0; i < error_stats.count; i++)
    {
        analysis->chars[i] = error_stats.chars[i];
        analysis->total_errors += error_stats.chars[i].wrong_count;
    }
    
    qsort (analysis->chars, analysis->count, sizeof(ErrorCharDetail), 
           compare_by_wrong_count);
    
    /* Calculate Pareto threshold */
    analysis->pareto_errors = 0;
    for (i = 0; i < analysis->count; i++)
    {
        cumulative_errors += analysis->chars[i].wrong_count;
        analysis->chars[i].is_pareto_top = TRUE;
        
        if ((gdouble)cumulative_errors / (gdouble)analysis->total_errors >= PARETO_THRESHOLD)
        {
            analysis->pareto_errors = cumulative_errors;
            break;
        }
    }
    
    /* Calculate coverage percentage */
    if (analysis->total_errors > 0)
        analysis->pareto_coverage = (gdouble)analysis->pareto_errors / 
                                   (gdouble)analysis->total_errors;
    else
        analysis->pareto_coverage = 0.0;
    
    g_message ("Pareto analysis: %d chars cover %.1f%% of %lu total errors",
              i + 1, analysis->pareto_coverage * 100, analysis->total_errors);
    
    return analysis;
}

/* Get the list of top error characters (80% coverage) */
gint
error_pareto_get_top_chars (ErrorCharDetail **top_chars)
{
    ParetoAnalysis *analysis;
    gint count = 0;
    gint i;
    
    analysis = error_pareto_analyze ();
    if (!analysis)
        return 0;
    
    /* Count Pareto characters */
    for (i = 0; i < analysis->count; i++)
    {
        if (analysis->chars[i].is_pareto_top)
            count++;
    }
    
    /* Allocate and copy */
    *top_chars = g_new (ErrorCharDetail, count);
    for (i = 0; i < count; i++)
    {
        (*top_chars)[i] = analysis->chars[i];
    }
    
    error_pareto_free (analysis);
    return count;
}

/* Save Pareto analysis results to file */
void
error_pareto_save (ParetoAnalysis *analysis)
{
    FILE *fh;
    gchar *tmp_name;
    gchar *kb_name;
    gint i;
    
    if (!analysis || analysis->count == 0)
        return;
    
    kb_name = g_strdup (keyb_get_name ());
    for (i = 0; kb_name[i]; i++)
        kb_name[i] = (kb_name[i] == ' ') ? '_' : kb_name[i];
    
    tmp_name = g_strconcat (main_path_stats (), G_DIR_SEPARATOR_S, 
                           PARETO_LOG_FILE, "_", kb_name, NULL);
    
    fh = g_fopen (tmp_name, "w");
    if (fh)
    {
        fprintf (fh, "Klavaro Pareto Analysis\n");
        fprintf (fh, "Keyboard: %s\n", kb_name);
        fprintf (fh, "Total errors: %lu\n", analysis->total_errors);
        fprintf (fh, "Pareto coverage: %.1f%%\n", analysis->pareto_coverage * 100);
        fprintf (fh, "\n");
        
        fprintf (fh, "Top Error Characters (80%% rule):\n");
        fprintf (fh, "Rank\tChar\tUTF8\tWrong\tCorrect\tErrorRate\tPareto\n");
        
        for (i = 0; i < analysis->count; i++)
        {
            fprintf (fh, "%d\t%C\t%s\t%lu\t%lu\t%.4f\t%s\n",
                    i + 1,
                    analysis->chars[i].uchr,
                    analysis->chars[i].utf8,
                    analysis->chars[i].wrong_count,
                    analysis->chars[i].correct_count,
                    analysis->chars[i].error_rate,
                    analysis->chars[i].is_pareto_top ? "TOP" : "");
        }
        
        fclose (fh);
        g_message ("Pareto analysis saved to: %s", tmp_name);
    }
    else
    {
        g_warning ("Could not save Pareto analysis to: %s", tmp_name);
    }
    
    g_free (tmp_name);
    g_free (kb_name);
}

/* Generate practice text focusing on top error characters */
gchar *
error_generate_practice_text (gint length, const gchar *language)
{
    ParetoAnalysis *analysis;
    gchar *text;
    gint i, j;
    gint top_count = 0;
    gint text_pos = 0;
    
    analysis = error_pareto_analyze ();
    if (!analysis || analysis->count == 0)
    {
        /* No error data, return generic practice text */
        return g_strdup (_("The quick brown fox jumps over the lazy dog. "));
    }
    
    /* Count top characters */
    for (i = 0; i < analysis->count; i++)
    {
        if (analysis->chars[i].is_pareto_top)
            top_count++;
        else
            break;
    }
    
    text = g_malloc0 (length * 6 + 1);  /* Max UTF-8 chars */
    
    /* Generate text with repeated top error characters */
    for (i = 0; i < length && text_pos < length * 6; i++)
    {
        /* Pick a random top character */
        if (top_count > 0)
        {
            j = rand () % top_count;
            text_pos += g_unichar_to_utf8 (analysis->chars[j].uchr, text + text_pos);
        }
        
        /* Add space or letter randomly */
        if (rand () % 5 == 0)
        {
            text[text_pos++] = ' ';
        }
        
        /* Add some normal letters to make it readable */
        if (rand () % 3 == 0 && top_count > 0)
        {
            /* Add a vowel */
            gunichar vowels[] = {'a', 'e', 'i', 'o', 'u', 
                                0x0430, 0x0435, 0x0438, 0x043E, 0x0443};  /* а,е,и,о,у */
            gint num_vowels = 10;
            
            if (g_ascii_strcasecmp (language, "ru") == 0)
                num_vowels = 5;
            else
                num_vowels = 5;
            
            j = rand () % num_vowels;
            text_pos += g_unichar_to_utf8 (vowels[j], text + text_pos);
        }
    }
    
    text[text_pos] = '\0';
    
    error_pareto_free (analysis);
    
    return text;
}

/* Free ParetoAnalysis structure */
void
error_pareto_free (ParetoAnalysis *analysis)
{
    if (analysis)
        g_free (analysis);
}

/* Get error statistics for a specific character */
gboolean
error_get_char_stats (gunichar uchr, gulong *wrong, gulong *correct)
{
    gint i;
    
    for (i = 0; i < error_stats.count; i++)
    {
        if (error_stats.chars[i].uchr == uchr)
        {
            if (wrong) *wrong = error_stats.chars[i].wrong_count;
            if (correct) *correct = error_stats.chars[i].correct_count;
            return TRUE;
        }
    }
    
    return FALSE;
}

/* Reset all error statistics */
void
error_logger_reset (void)
{
    memset (&error_stats, 0, sizeof(error_stats));
    error_stats.initialized = TRUE;
    
    g_message ("Error statistics reset");
}

/* Wrapper functions for keyboard.h dependency */
extern gchar * keyb_get_name (void);
