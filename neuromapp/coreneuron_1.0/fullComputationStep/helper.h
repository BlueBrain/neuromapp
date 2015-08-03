#pragma once

struct input_parameters{
    char * d; // data set
    int th; // number of thread
    char * name; // name
};

void fullComputationStep_print_usage();
int fullComputationStep_help(int argc, char * const argv[], struct input_parameters * p);