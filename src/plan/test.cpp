#include "target.h"
#include "selector.h"
#include "status.h"
#include <stdio.h>
#include <stdlib.h>
#include <libnova/libnova.h>
#include "../utils/config.h"
#include "../utils/objectcheck.h"
#include "../db/db.h"
#include "median.h"

void
print_jd (double JD, struct ln_lnlat_posn *obs)
{
  struct ln_date date;
  struct ln_equ_posn posn;
  struct ln_hrz_posn hrz;

  ln_get_solar_equ_coords (JD, &posn);
  ln_get_hrz_from_equ (&posn, obs, JD, &hrz);

  ln_get_date (JD, &date);
  printf ("%i/%i/%i %i:%i:%02.3f %02.2f", date.years, date.months, date.days,
	  date.hours, date.minutes, date.seconds, hrz.alt);
}

void
print_rst (struct ln_rst_time *rst, struct ln_lnlat_posn *obs)
{
  print_jd (rst->rise, obs);
  printf ("|");
  print_jd (rst->transit, obs);
  printf ("|");
  print_jd (rst->set, obs);
  printf ("\n");
}

int
main (int argc, char **argv)
{
  ParTarget *target;
  struct ln_par_orbit orbit;
  Target *plan = new Target (NULL, NULL);
  double values[200];
  time_t t;
  if (argc == 2)
    t = atoi (argv[1]);
  else
    time (&t);
  double jd = ln_get_julian_from_timet (&t);
  read_config (CONFIG_FILE);
  orbit.q = 0.16752;
  orbit.w = 332.7622;
  orbit.omega = 222.8061;
  orbit.i = 63.1662;
  orbit.JD = 2453112.629;
  target = new ParTarget (NULL, NULL, &orbit);
  struct ln_equ_posn pos;
  printf ("Date: %s\n", ctime (&t));
  target->getPosition (&pos, jd);
  printf ("gsid: %f\n", ln_get_mean_sidereal_time (jd));
  printf ("lsid: %f\n",
	  ln_get_mean_sidereal_time (jd) - get_double_default ("longtitude",
							       0) / 15.0);
  printf ("RA: %f DEC: %f\n", pos.ra, pos.dec);
  struct ln_lnlat_posn observer;
  observer.lng = get_double_default ("longtitude", 0);
  observer.lat = get_double_default ("latitude", 0);
  struct ln_hrz_posn hrz;
  ln_get_hrz_from_equ (&pos, &observer, jd, &hrz);
  printf ("Alt: %f Az: %f\n", hrz.alt, hrz.az);
  for (int i = 0; i < 5; i++)
    {
      struct ln_rst_time rst;
      target->getRST (&observer, &rst, jd);
      print_rst (&rst, &observer);
      jd++;
    }

  db_connect ();

  ObjectCheck *checker = new ObjectCheck ("/etc/rts2/horizont");
  Selector *selector = new Selector (checker, NULL, &observer);

  jd = get_double_default ("planc_selector", SELECTOR_ELL);

  if (selector->get_next_plan
      (plan, (int) jd,
       &t, 2, 30, SERVERD_NIGHT, observer.lng, observer.lat, 1))
    {
      printf ("Error making plan\n");
      fflush (stdout);
      exit (EXIT_FAILURE);
    }
  printf ("...plan made\n");
  printf ("t->tar_id: %i\n", plan->next->id);

  srandom (time (NULL));

  // test median & quicksort algo..
  for (int i = 0; i < 199; i++)
    {
      values[i] = random () / 200;
    }

  printf ("median: %f\n", get_median (values, 199));

  db_disconnect ();
}
