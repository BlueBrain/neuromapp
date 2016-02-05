/*
 * Neuromapp - helper.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * francesco.cremonesi@epfl.ch
 * sam.yates@epfl.ch
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/coreneuron_1.0/kernel/kernel.c
 * \brief Implements the helper function of the kernel miniapp
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include "coreneuron_1.0/kernel/helper.h"
#include "utils/error.h"

int kernel_print_usage() {
    printf("Usage: kernel --mechanism [string] --function [string] --data [string] --numthread [int] --name [string]\n");
    printf("Details: \n");
    printf("                 --mechanism [Na, ProbAMPANMDA or Ih] \n");
    printf("                 --function [state or current] \n");
    printf("                 --data [path to the input] \n");
    printf("                 --numthread [threadnumber] \n");
    printf("                 --name [to internally reference the data, default name coreneuron_1.0_kernel_data] \n");
    return MAPP_USAGE;
}

int kernel_help_function(const char* f)
{
    int error = MAPP_OK;
    if((strncmp(f,"state",5) != 0) && (strncmp(f,"current",7) != 0))
       error = MAPP_BAD_ARG;

    return error;
}

int kernel_help_mechanism(const char* m)
{
    int error = MAPP_OK;
    if((strncmp(m,"Na",2) != 0) && (strncmp(m,"Ih",3) != 0) && (strncmp(m,"ProbAMPANMDA",12) != 0))
        error = MAPP_BAD_ARG;
    return error;
}

int kernel_help(int argc, char * const argv[], struct input_parameters * p)
{
  int c;

  p->m = "Na"; // default
  p->f = "state"; // default
  p->d = "";
  p->th = 1; // one omp thread by default
  p->name = "coreneuron_1.0_kernel_data";

  optind = 0;

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
          case 'm':
              if(kernel_help_mechanism(optarg) != MAPP_OK)
                  return MAPP_BAD_ARG;
              p->m = optarg;
              break;
          case 'f':
              if(kernel_help_function(optarg) != MAPP_OK)
                  return MAPP_BAD_ARG;
              p->f = optarg;
              break;
          case 'd':
              if(access(optarg, F_OK ) == -1 )
                  return MAPP_BAD_DATA;
              p->d = optarg;
              break;
          case 't':
              p->th = atoi(optarg);
              break;
          case 'n':
              p->name = optarg;
              break;
          case 'h':
              return kernel_print_usage();
              break;
          default:
              return kernel_print_usage ();
	      break;
      }
  }
  return 0 ;
}
