#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void solver_print_usage() {
    printf("usage: solver --data [string]\n");
    printf("details: \n");
    printf("                 --data [path to the input] \n");
    exit(1);
}


struct input_parameters{
    char * d; // data set
};


int solver_help(int argc, char* argv[], struct input_parameters * p)
{
  int c;

  p->d = "";

  while (1)
  {
      static struct option long_options[] =
      {
          {"help", no_argument, 0, 'h'},
          {"data",  required_argument,     0, 'd'},

          {0, 0, 0, 0}
      };
      /* getopt_long stores the option index here. */
      int option_index = 0;

      c = getopt_long (argc, argv, "d:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
      {
          case 'd': p->d = optarg;
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

