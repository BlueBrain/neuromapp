#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>


void kernel_print_usage() {
    printf("Usage: kernel --mechanism [string] --function [string] --data [string] --numthread [int] --name [string]\n");
    printf("Details: \n");
    printf("                 --mechanism [Na, ProbAMPANMDA or Ih] \n");
    printf("                 --function [state or current] \n");
    printf("                 --data [path to the input] \n");
    printf("                 --numthread [threadnumber] \n");
    printf("                 --name [to internally reference the data] \n");
    exit(1);
}

struct input_parameters{
    char * m; // mechanism
    char * f; // function state/current
    char * d; // data set
    int th; // number of thread
    char * name; // name
};

int kernel_help(int argc, char * const argv[], struct input_parameters * p)
{
  int c;

  p->m = "Na"; // default
  p->f = "state"; // default
  p->d = "";
  p->th = 1; // one omp thread by default
  p->name = "coreneuron_1.0_kernel_data";

  while (1)
  {
      static struct option long_options[] =
      {
          {"help", no_argument, 0, 'h'},
          {"mechanism", required_argument, 0, 'm'},
          {"function",  required_argument, 0, 'f'},
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
          case 'm': p->m = optarg;
              break;
          case 'f': p->f = optarg;
              break;
          case 'd': p->d = optarg;
              break;
          case 't': p->th = atoi(optarg);
              break;
          case 'n': p->name = optarg;
              break;
          case 'h':
              kernel_print_usage();
              break;
          default:
              kernel_print_usage ();
	      break;
      }
  }

  return 0 ;


}
