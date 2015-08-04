#pragma once

struct input_parameters{
    char * m; // mechanism
    char * f; // function state/current
    char * d; // data set
    int th; // number of thread
    char * name; // name
};

int kernel_print_usage();
int kernel_help(int argc, char * const argv[], struct input_parameters * p);
int kernel_help_mechanism(const char* m);
int kernel_help_function(const char* f);