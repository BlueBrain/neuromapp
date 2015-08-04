#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "coreneuron_1.0/cstep/helper.h"

void cstep_print_usage() {
    printf("Usage: cstep --data <input path> [--numthread int] [--name string]\n");
    printf("Details: \n");
    printf("                 --data [path to the input]\n");
    printf("                 --numthread <threadnumber>\n");
    printf("                 --name <reference to internal data> \n");
}

int cstep_help(int argc, char * const argv[], struct input_parameters * p)
{
  int c;
  p->d = "";
  p->th = 1; // one omp thread by default
  p->name = "cstep_storage_name_helper";

  while (1)
  {
      static struct option long_options[] =
      {
          {"help", no_argument, 0, 'h'},
          {"data",  required_argument,     0, 'd'},
          {"numthread",  required_argument,0, 't'},
          {"name",  required_argument,     0, 'n'},

          {0, 0, 0, 0}
      };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "m:f:d:t:n:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
      {
          case 'd': p->d = optarg;
              break;
          case 't': p->th = atoi(optarg);
              break;
          case 'n': p->name = optarg;
              break;
          case 'h':
              cstep_print_usage();
              break;
          default:
              cstep_print_usage ();
	      break;
      }
  }
  return 0 ;
}
