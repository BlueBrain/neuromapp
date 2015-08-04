#pragma once

struct input_parameters{
    char * d; // data set
    char * name; // name
};

int solver_print_usage();
int solver_help(int argc, char* const argv[], struct input_parameters * p);
