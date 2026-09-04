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
 * Error logging and Pareto analysis for Klavaro
 * Tracks detailed error statistics and identifies top 80% error characters
 */

#ifndef ERROR_LOGGER_H
#define ERROR_LOGGER_H

#include <glib.h>

#define ERROR_LOG_FILE "error_detail.log"
#define PARETO_LOG_FILE "pareto_analysis.log"
#define PARETO_THRESHOLD 0.80  /* 80% of errors */
#define MAX_ERROR_CHARS 256

/* Structure to store detailed error information */
typedef struct {
    gunichar uchr;           /* Unicode character */
    gchar utf8[8];           /* UTF-8 representation */
    gulong wrong_count;      /* Number of times typed wrong */
    gulong correct_count;    /* Number of times typed correctly */
    gulong total_attempts;   /* Total attempts (wrong + correct) */
    gdouble error_rate;      /* wrong_count / total_attempts */
    gboolean is_pareto_top;  /* TRUE if this char is in top 80% errors */
} ErrorCharDetail;

/* Structure for Pareto analysis results */
typedef struct {
    ErrorCharDetail chars[MAX_ERROR_CHARS];
    gint count;              /* Number of unique error characters */
    gulong total_errors;     /* Total number of all errors */
    gulong pareto_errors;    /* Errors from top Pareto characters */
    gdouble pareto_coverage; /* Percentage of errors covered by Pareto chars */
} ParetoAnalysis;

/* Initialize the error logging system */
void error_logger_init (void);

/* Log a detailed error occurrence */
void error_log_error (gunichar expected, gunichar typed, const gchar *context);

/* Log a correct typing occurrence */
void error_log_correct (gunichar typed);

/* Save error log to file */
void error_logger_save (void);

/* Load error log from file */
void error_logger_load (void);

/* Perform Pareto analysis on accumulated errors */
ParetoAnalysis * error_pareto_analyze (void);

/* Get the list of top error characters (80% coverage) */
gint error_pareto_get_top_chars (ErrorCharDetail **top_chars);

/* Save Pareto analysis results to file */
void error_pareto_save (ParetoAnalysis *analysis);

/* Generate practice text focusing on top error characters */
gchar * error_generate_practice_text (gint length, const gchar *language);

/* Free ParetoAnalysis structure */
void error_pareto_free (ParetoAnalysis *analysis);

/* Get error statistics for a specific character */
gboolean error_get_char_stats (gunichar uchr, gulong *wrong, gulong *correct);

/* Reset all error statistics */
void error_logger_reset (void);

#endif /* ERROR_LOGGER_H */
