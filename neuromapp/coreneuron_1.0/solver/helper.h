
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void solver_print_usage() {
    printf("usage: solver --data [string] --name [string]\n");
    printf("details: \n");
    printf("                 --data [path to the input] \n");
    printf("                 --name [to internally reference the data] \n");
    exit(1);
}


struct input_parameters{
    char * d; // data set
    char * name; // name
};


int solver_help(int argc, char* const argv[], struct input_parameters * p)
{
  int c=0;

  p->d = "";
  p->name = "coreneuron_1.0_solver_data";

  optind = 1;

  while (1)
  {
      static struct option long_options[] =
      {
          {"help", no_argument, NULL, 'h'},
          {"data", required_argument,     NULL, 'd'},
          {"name", required_argument,     NULL, 'n'},
          {NULL, 0, NULL, 0}
      };
      /* getopt_long stores the option index here. */
      int option_index = 0;
      c = getopt_long (argc, argv, "d:n:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
      {
          case 'd': p->d = optarg;
              break;
          case 'n': p->name = optarg;
              break;
          case 'h':
              solver_print_usage();
              break;
          default:
              solver_print_usage ();
	      break;
      }
  }
  return 0 ;
}

