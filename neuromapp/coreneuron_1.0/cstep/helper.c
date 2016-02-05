/*
 * Neuromapp - cstep.c, Copyright (c), 2015,
 * Timothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Bruno Magalhaes - Swiss Federal Institute of technology in Lausanne,
 * Cremonesi Francesco - Swiss Federal Institute of technology in Lausanne,
 * Sam Yates - Swiss Federal Institute of technology in Lausanne,
 * timothee.ewart@epfl.ch,
 * bruno.magalhaes@epfl.ch
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
 * @file neuromapp/coreneuron_1.0/cstep/helper.c
 * \brief Implements the helper of the cstep miniapp
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#include "coreneuron_1.0/cstep/helper.h"
#include "utils/error.h"

int cstep_print_usage() {
    printf("Usage: cstep --data <input path> [--numthread int] [--name string]\n");
    printf("Details: \n");
    printf("                 --data [path to the input]\n");
    printf("                 --numthread <threadnumber>\n");
    printf("                 --name [to internally reference the data, default name coreneuron_1.0_cstep_data] \n");
    return MAPP_USAGE;
}

int cstep_help(int argc, char * const argv[], struct input_parameters * p)
{
  int c;
  p->d = "";
  p->th = 1; // one omp thread by default
  p->name = "coreneuron_1.0_cstep_data";

  optind = 0;

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

      c = getopt_long (argc, argv, "d:t:n:",
                       long_options, &option_index);
      /* Detect the end of the options. */
      if (c == -1)
        break;

      switch (c)
      {
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
              return cstep_print_usage();
              break;
          default:
              return cstep_print_usage();
              break;
      }
  }
  return 0 ;
}