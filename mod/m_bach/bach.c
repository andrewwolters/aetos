/*
 * Bach module for aetos
 * $Id: bach.c,v 1.2 2002/11/01 20:50:01 semprini Exp $
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include "pgsql/libpq-fe.h"

#define MOD_NAME bach

#include "../mod.h"

static efun_tbl efuns;
static int fd;
PGconn *db_conn;

int leap_year (int year)
{ return (year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0));
}

/* Returns the yday that Easter is */
int easter (int year)
{ int r1, r2, r3, r4, r5;
  r1 = year % 19;
	r2 = year % 4;
	r3 = year % 7;
	r4 = (19 * r1 + 24) % 30;
	r5 = (2 * r2 + 4 * r3 + 6 * r4 + 5) % 7;
  if (leap_year (year))
    return r4 + r5 + 81;
	return r4 + r5 + 80; 
}

void find_cantata (char *sql_expression, int *bwv, char **name)
{ char *query;
  PGresult *res;
	int nr;
	*bwv = -1;
	asprintf (&query, "SELECT bwv, name FROM cantatas WHERE day = '%s'",
           sql_expression);
	res = PQexec (db_conn, query);
	if (PQresultStatus (res) == PGRES_TUPLES_OK)
	{ if (PQntuples (res) > 0)
	  { nr = random () % PQntuples (res);
	    *bwv = atoi (PQgetvalue (res, nr, 0));
		  *name = strdup (PQgetvalue (res, nr, 1));
    }
		else
		{ *bwv = -1;
		}
	}
	PQclear (res);
	free (query);
}

static void do_privmsg (event_t event)
{
	ircmsg_evt *ircmsg;
	char *dest, *argv[1], *message = NULL, *name = NULL; 
	int bwv = -1, easter_d, year, state = 0;
  time_t tl = time (NULL);
  struct tm *t = localtime (&tl);

	efuns -> get_from_event (event, (void **) &ircmsg);
	dest = efuns -> source_privmsg (*ircmsg);
	efuns -> tokenize_string (ircmsg -> param, argv, 2);
  if (!(efuns -> is_command (argv [1], "cantata")))
    return;
  year = t -> tm_year + 1900;
  easter_d = easter (year);
  while ((state == 0) || (state == 1))
  { // Fixed holidays, in order
    // Dec 25-27: Christmas
    if ((t -> tm_mon == 11) && ((t -> tm_mday >= 25) && (t -> tm_mday <= 27)))
    { asprintf (&message, "Am %d. Weihnachtstag", t -> tm_mday - 24);
      find_cantata (message, &bwv, &name);
    }
    // Jan 1: New Year
    else if ((t -> tm_mon == 0) && (t -> tm_mday == 1))
    { PGresult *res;
  	  int nr;
  	  res = PQexec (db_conn, "SELECT bwv, name FROM cantatas WHERE day = "
                             "'Am Neujahrstag' OR day = 'Am Feste der "
                             "Beschneidung Christi'");
  	  if (PQresultStatus (res) == PGRES_TUPLES_OK)
  	  { if (PQntuples (res) > 0)
  	    { nr = random () % PQntuples (res);
  	      bwv = atoi (PQgetvalue (res, nr, 0));
  		    name = strdup (PQgetvalue (res, nr, 1));
        }
  		}
			PQclear (res);
    }
    // Jan 6: Epiphany
    else if ((t -> tm_mon == 0) && (t -> tm_mday == 6))
    { if (t -> tm_wday == 0)
        find_cantata ("Am Sonntag Epiphanias", &bwv, &name);
      else
        find_cantata ("Am Feste Epiphanias", &bwv, &name);
    }
    // Feb 2: Cleansing(?) of Mary
    else if ((t -> tm_mon == 1) && (t -> tm_mday == 2))
      find_cantata ("Am Feste Mariae Reinigung", &bwv, &name);
    // Mar 25: Annunciation to Mary
    else if ((t -> tm_mon == 2) && (t -> tm_mday == 25))
      find_cantata ("Zu Mariae Verkuendigung", &bwv, &name);
    // June 24: St. John the Baptist
    else if ((t -> tm_mon == 5) && (t -> tm_mday == 24))
      find_cantata ("Am Feste Johannis des Taeufers", &bwv, &name);
    // September 29: Michaelmas
    else if ((t -> tm_mon == 8) && (t -> tm_mday == 29))
      find_cantata ("Am Michaelisfest", &bwv, &name);
    // October 31: Reformation Day
    else if ((t -> tm_mon == 9) && (t -> tm_mday == 31))
      find_cantata ("Am Reformationsfest", &bwv, &name);
    else
    { // If our Sunday is after Christmas (day 361), it's 'Sunday after CM'
      if (t -> tm_yday > 361)
        find_cantata ("Am Sonntag nach Weihnachten", &bwv, &name);
      // If our Sunday is before Epiphany (day 5), it's 'Sunday after NY'
      else if (t -> tm_yday < 5)
        find_cantata ("Am Sonntag nach Neujahr", &bwv, &name);
      // If we are 28 days or less before Christmas, it's Advent
      else if ((t -> tm_yday >= 332) ||
               (!leap_year (year) && (t -> tm_yday >= 331)))
      { int days_before_cm = (t -> tm_yday) - (leap_year (year) ? 325 : 324);
        asprintf (&message, "Am %d. Advent", days_before_cm / 7);
        find_cantata (message, &bwv, &name);
      }
      else
      { if ((t -> tm_yday >= easter_d) && (t -> tm_yday <= (easter_d + 2)))
        { asprintf (&message, "Am %d. Ostertag", (t -> tm_yday) - easter_d + 1);
          find_cantata (message, &bwv, &name);
        }
        else if (t -> tm_yday == (easter_d - 7))
          find_cantata ("Am Palmensonntag", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 14))
          find_cantata ("Am Sonntag Judica", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 21))
          find_cantata ("Am Sonntag Laetare", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 28))
          find_cantata ("Am Sonntag Oculi", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 35))
          find_cantata ("Am Sonntag Reminiscere", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 42))
          find_cantata ("Am Sonntag Invocavit", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 49))
          find_cantata ("Am Sonntag Estomihi", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 56))
          find_cantata ("Am Sonntag Sexagesimae", &bwv, &name);
        else if (t -> tm_yday == (easter_d - 63))
          find_cantata ("Am Sonntag Septuagesimae", &bwv, &name);
        else if ((t -> tm_yday < easter_d) && (t -> tm_yday > 5))
        { int first_sunday_after_epi = easter_d - 70;
          while (first_sunday_after_epi > 12)
            first_sunday_after_epi -= 7;
          asprintf (&message, "Am %d. Sonntag nach Epiphanias",
                   (((t -> tm_yday) - first_sunday_after_epi) / 7) + 1);
          find_cantata (message, &bwv, &name);
        }
        else if (t -> tm_yday == (easter_d + 7))
          find_cantata ("Am Sonntag Quasimodogeniti", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 14))
          find_cantata ("Am Sonntag Misericordias Domini", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 21))
          find_cantata ("Am Sonntag Jubilate", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 28))
          find_cantata ("Am Sonntag Cantate", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 35))
          find_cantata ("Am Sonntag Rogate", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 39))
          find_cantata ("Am Feste der Himmelfahrt Christi", &bwv, &name);
        else if (t -> tm_yday == (easter_d + 42))
          find_cantata ("Am Sonntag Exaudi", &bwv, &name);
        else if ((t -> tm_yday >= (easter_d + 49)) &&
                 (t -> tm_yday <= (easter_d + 51)))
        { asprintf (&message, "Am %d. Pfingsttag",
					          (t -> tm_yday) - easter_d - 48);
          find_cantata (message, &bwv, &name);
        }
        else if (t -> tm_yday == (easter_d + 56))
          find_cantata ("Am Sonntag Trinitatis", &bwv, &name);
        else
        { asprintf (&message, "Am %d. Sonntag nach Trinitatis",
					          ((t -> tm_yday) - easter_d - 56) / 7);
          find_cantata (message, &bwv, &name);
        }
      }
    }
    if (bwv > 0)
      state = 2; 
    if (bwv == -1)
    { if ((state == 0) && (t -> tm_wday != 0))
      { efuns -> send_message (fd, dest, "There is no cantata for today.");
        efuns -> send_message (fd, dest,
				                       "I'll give you the one for next Sunday.");
        t -> tm_yday += (7 - (t -> tm_wday));
        state = 1;
      }
      else 
        state = 2;
    }
  }
  if (bwv > 0)
  { asprintf (&message, "Try `%s' (BWV %d).", name, bwv);
    efuns -> send_message (fd, dest, message);
  }
  else
    efuns -> send_message (fd, dest, "No cantata could be found.");
	efuns -> tfree (message);
	efuns -> tfree (name);
}

char *bach_init (efun_tbl table, int argc, char **argv)
{ gs_table gst; 
	efuns = table;
	gst = efuns -> get_gst ();
	fd = gst -> serversocket;
	efuns -> mod_initialize ("bach", 1685, 1750);
	efuns -> add_callback (EvtPrivmsgMask, do_privmsg);
	efuns -> send_message (fd, gst -> channelname, "Jauchzet, frohlocket!");
  db_conn = PQconnectdb ("host=postgres.kerguelen.org dbname=bach user=aetos "
	                       "password=aetos");
  if (PQstatus (db_conn) != CONNECTION_OK)
	{ char *errmsg;
	  asprintf (&errmsg, "Database error: %s", PQerrorMessage (db_conn));
		efuns -> send_message (fd, gst -> channelname, errmsg);
		free (errmsg);
		return "Ruhe sanfte, sanfte ruh";
	}
	srandom (time (NULL));
	efuns -> mod_mainloop ();
	return NULL;
}
