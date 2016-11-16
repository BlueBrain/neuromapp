/*
 * Neuromapp - config.cpp, Copyright (c), 2015,
 * Judit Planas - Swiss Federal Institute of technology in Lausanne,
 * judit.planas@epfl.ch,
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
 * GNU General Public License for more details. See
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/replib/config.cpp
 *  Test on the replib miniapp config class
 */

#define BOOST_TEST_MODULE replibConfigTEST

#include <boost/test/unit_test.hpp>
#include "replib/utils/config.h"
#include "utils/argv_data.h"

//Performs MPI init/finalize
#include "test/tools/mpi_helper.h"

BOOST_AUTO_TEST_CASE(config_constructor_default_test){
    replib::config c;

    int         procs = 1;
    int         id = 0;
    std::string write = "rnd1b";
    std::string input_dist = "";
    std::string output_report = "";
    bool        invert = false;
    int         numcells = 10;
    int         sim_steps = 15;
    int         rep_steps = 1;
    int         elems_per_step = 0;
    int         sim_time_ms = 100;
    bool        check = false;
    bool        passed = false;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    BOOST_CHECK_EQUAL(c.procs(), procs);
    BOOST_CHECK_EQUAL(c.id(), id);
    BOOST_CHECK_EQUAL(c.write(), write);
    BOOST_CHECK_EQUAL(c.input_dist(), input_dist);
    BOOST_CHECK_EQUAL(c.output_report(), output_report);
    BOOST_CHECK_EQUAL(c.invert(), invert);
    BOOST_CHECK_EQUAL(c.numcells(), numcells);
    BOOST_CHECK_EQUAL(c.sim_steps(), sim_steps);
    BOOST_CHECK_EQUAL(c.rep_steps(), rep_steps);
    BOOST_CHECK_EQUAL(c.elems_per_step(), elems_per_step);
    BOOST_CHECK_EQUAL(c.sim_time_ms(), sim_time_ms);
    BOOST_CHECK_EQUAL(c.check(), check);
    BOOST_CHECK_EQUAL(c.passed(), passed);
}


BOOST_AUTO_TEST_CASE(config_constructor_test){
    replib::config c;

    std::string write = "file1b";
    std::string input_dist = "./input.txt";
    std::string output_report = "./output.txt";
    bool        invert = true;
    int         numcells = 200;
    int         sim_steps = 6;
    int         rep_steps = 19;
    int         elems_per_step = 256;
    int         sim_time_ms = 170;
    bool        check = true;
    bool        passed = true;

    c.write() = write;
    c.input_dist() = input_dist;
    c.output_report() = output_report;
    c.invert() = invert;
    c.numcells() = numcells;
    c.sim_steps() = sim_steps;
    c.rep_steps() = rep_steps;
    c.elems_per_step() = elems_per_step;
    c.sim_time_ms() = sim_time_ms;
    c.check() = check;
    c.passed() = passed;

    BOOST_CHECK_EQUAL(c.write(), write);
    BOOST_CHECK_EQUAL(c.input_dist(), input_dist);
    BOOST_CHECK_EQUAL(c.output_report(), output_report);
    BOOST_CHECK_EQUAL(c.invert(), invert);
    BOOST_CHECK_EQUAL(c.numcells(), numcells);
    BOOST_CHECK_EQUAL(c.sim_steps(), sim_steps);
    BOOST_CHECK_EQUAL(c.rep_steps(), rep_steps);
    BOOST_CHECK_EQUAL(c.elems_per_step(), elems_per_step);
    BOOST_CHECK_EQUAL(c.sim_time_ms(), sim_time_ms);
    BOOST_CHECK_EQUAL(c.check(), check);
    BOOST_CHECK_EQUAL(c.passed(), passed);
}

BOOST_AUTO_TEST_CASE(config_constructor_argv_test){
    std::string s[17]={"binary", "-w", "fileNb", "-o", "/tmp/currents.bbp", "-f", "/tmp/input.txt",
            "-i", "-c", "1024", "-s", "7", "-r", "100", "-t", "50", "-v"};

    int narg=sizeof(s)/sizeof(s[0]);

    mapp::argv_data A(s,s+narg);

    int argc=A.argc();
    char * const *argv=A.argv();

    replib::config c(argc, argv);

    c.procs() = 4;
    c.id() = 3;

    BOOST_CHECK_EQUAL(c.procs(), 4);
    BOOST_CHECK_EQUAL(c.id(), 3);
    BOOST_CHECK_EQUAL(c.write(), "fileNb");
    BOOST_CHECK_EQUAL(c.input_dist(), "/tmp/input.txt");
    BOOST_CHECK_EQUAL(c.output_report(), "/tmp/currents.bbp");
    BOOST_CHECK_EQUAL(c.invert(), true);
    BOOST_CHECK_EQUAL(c.numcells(), 1024);
    BOOST_CHECK_EQUAL(c.sim_steps(), 7);
    BOOST_CHECK_EQUAL(c.rep_steps(), 100);
    BOOST_CHECK_EQUAL(c.sim_time_ms(), 50);
    BOOST_CHECK_EQUAL(c.check(), true);
    BOOST_CHECK_EQUAL(c.passed(), false);
}
