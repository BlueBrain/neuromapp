/*
 * Neuromapp - benchmark.h, Copyright (c), 2015,
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
 * GNU General Public License for more details. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

#ifndef MAPP_RL_ADIOS_H
#define MAPP_RL_ADIOS_H

//#include <mpi.h>
//#include <unistd.h>
#include <string.h>
#include <cstring>

#include "replib/backends/basic.h"
#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/rnd1b.h"
#include "replib/mpiio_dist/file1b.h"
#include "replib/mpiio_dist/fileNb.h"

#include "utils/mpi/controler.h"
#include "utils/mpi/timer.h"
//#include "utils/mpi/error.h"

namespace replib {

  // NOTE: its final 
class ADIOSWriter : public replib::Writer {

    private:
        /* Fileview of the file, it is private, so no need for getters/setters */
        replib::fileview * f_;

        /* The following members are private and exclusive of MPI I/O,
         * so there is no need to implement getters or setters
         */
        MPI_Info info_;
        MPI_File fh_;


    public:

        ADIOSWriter();
        ~ADIOSWriter();

        void init(replib::config &c);
        void open(char * path);
        void open(mapp::timer &t_io, const std::string &path);
        void write(float * buffer, size_t count);
        void write(mapp::timer &t_io, float * buffer, size_t count);
        void close();
        void close(mapp::timer &t_io);
        unsigned int total_bytes();

};

/** \fun ADIOSWriter()
    \brief create the object, no special actions required for MPI I/O
 */
ADIOSWriter::ADIOSWriter() : f_(NULL) {}


/** \fun ~ADIOSWriter()
    \brief Class destructor */
ADIOSWriter::~ADIOSWriter() {
    delete(f_);
}


/** \fun init(const replib::config &c)
    \brief initialize the object using the given configuration */
void ADIOSWriter::init(replib::config &c) {
    if (c.write() == "file1b") {
        f_ = file1b(c);
    } else if (c.write() == "fileNb") {
        f_ = fileNb(c);
    } else { // "rnd1b"
        f_ = rnd1b(c);
    }

    // Use MPI_Info to disable collective buffers if using IME backend
    MPI_Info_create(&info_);
    std::string ime("ime:/");
    if (c.output_report().compare(0, ime.length(), ime) == 0) {
        MPI_Info_set (info_, "romio_ds_write", "disable");
        MPI_Info_set (info_, "romio_cb_write", "disable");
    }
}


/** \fun open(const char * path)
    \brief Open the file with MPI I/O and set the fileview.
           Inline version to be as fast as possible */
inline void ADIOSWriter::open(char * report) {
    //Open the file
    int error = MPI_File_open(MPI_COMM_WORLD, report, MPI_MODE_WRONLY | MPI_MODE_CREATE, info_, &fh_);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error opening file: " << report << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 911);
    }

    f_->set_fileview(&fh_);
}

/** \fun open(mapp::timer &t_io, const std::string & path)
    \brief Open the file with MPI I/O and set the fileview.
           Timer will be used to account the time of opening the file and setting the fileview */
void ADIOSWriter::open(mapp::timer &t_io, const std::string & path) {
    // Get the file path
    char * report = strdup(path.c_str());

    //Open the file
    t_io.tic();
    open(report);
    t_io.toc();
}


/** \fun write(float * buffer, size_t count)
    \brief Write to file. Inline version to be as fast as possible */
inline void ADIOSWriter::write(float * buffer, size_t count) {
    MPI_Status status;
    int error = MPI_File_write_all(fh_, buffer, count, MPI_FLOAT, &status);

    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error writing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 915);
    }
}

/** \fun write(mapp::timer &t_io, float * buffer, size_t count)
    \brief Write to file. Since we need to measure the writing time, this function receives
           the timer that should be used to surround the write call */
void ADIOSWriter::write(mapp::timer &t_io, float * buffer, size_t count) {
    MPI_Status status;
    // t_io only counts time spent in MPI_File_write_all to compute I/O statistics
    t_io.tic();
    int error = MPI_File_write_all(fh_, buffer, count, MPI_FLOAT, &status);
    t_io.toc();

    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error writing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 915);
    }
}

/** \fun close()
    \brief Close the file. Inline version to be as fast as possible */
inline void ADIOSWriter::close() {
    int error = MPI_File_close(&fh_);
    if (error != MPI_SUCCESS) {
        std::cout << "[" << mapp::controler::getInstance().rank() << "] Error closing file" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 912);
    }
}

/** \fun close(mapp::timer &t_io)
    \brief Close the file.
           Timer will be used to account the time of closing the file */
void ADIOSWriter::close(mapp::timer &t_io) {
    t_io.tic();
    close();
    t_io.toc();
}

/** \fun total_bytes()
    \brief Return the total amount of data (in bytes) written at each reporting step */
unsigned int ADIOSWriter::total_bytes() {
    return f_->total_bytes();
}

} // end namespace

#endif // MAPP_RL_ADIOS_H
