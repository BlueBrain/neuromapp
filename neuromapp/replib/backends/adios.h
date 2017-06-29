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
#include <adios.h>
namespace {
  int comm = MPI_COMM_WORLD;
}

namespace replib {


  class ADIOSWriter : public replib::Writer {
    private:
      uint64_t            iteration_;
      uint64_t            total_size;
      int                 rank_;
      int64_t             adios_handle_;
      char*               report_;
      replib::fileview*   f_;
      replib::config*     c_;
    public:

      ADIOSWriter();
      ~ADIOSWriter();

      void           init     (replib::config &c);
      void           open     (char * path);
      void           open     (mapp::timer &t_io, const std::string &path);
      void           write    (float * buffer, size_t count);
      void           write    (mapp::timer &t_io, float * buffer, size_t count);
      void           close    ();
      void           close    (mapp::timer &t_io);
      void          finalize  ();
      unsigned int  total_bytes();

  };

  /** \fun ADIOSWriter()
    \brief create the object, no special actions required for MPI I/O */
  ADIOSWriter::ADIOSWriter() : iteration_(0), rank_(-1), f_(NULL) {
  }


  /** \fun ~ADIOSWriter()
    \brief Class destructor */
  ADIOSWriter::~ADIOSWriter() {
    adios_finalize (rank_);
    delete(f_);
  }


  /** \fun init(const replib::config &c)
    \brief initialize the object using the given configuration */
  void ADIOSWriter::init(replib::config &c) {
    if (c.write() == "file1b") {
      f_ = file1b(c);
    }else if (c.write() == "rnd1b"){
      f_ = rnd1b(c);
    }else {
      f_ = fileNb(c);
    }
    rank_ = c.id();
    c_ = &c;
  }

  /** \fun finalize()
    \brief call ADIOS finalize  */
  void ADIOSWriter::finalize () {
    adios_finalize (rank_);
  }


  /** \fun open(const char * path)
    \brief Open the file.
    Inline version to be as fast as possible */
  inline void ADIOSWriter::open(char * report) {
    std::cerr << "OPEN THIS F FILE !!!" << std::endl;
    report_ = strdup(report);
    std::cerr << "report name = " << report_ << std::endl;
    adios_init_noxml    (comm);
    int64_t adios_group_id;
    adios_declare_group (&adios_group_id,"report", "", adios_stat_no);
    adios_select_method (adios_group_id, "MPI",    "verbose=4", "");
    adios_define_var    (adios_group_id, "global_size",  "", adios_integer, 0,0,0);
/*
    for (int i =0; i < f_->wr_blocks(); i++) {
      adios_define_var (adios_group_id, "batch_size", "", adios_integer, 0,0,0);
      adios_define_var (adios_group_id, "offset",     "", adios_integer, 0,0,0);
      adios_define_var (adios_group_id, "data",       "", adios_real,    "batch_size", "global_size", "offset");
    }
*/

//    adios_open  (&adios_handle_, "report", report, "w", MPI_COMM_WORLD);
  }

  /** \fun open(mapp::timer &t_io, const std::string & path)
    \brief Open the file.
    Timer will be used to account the time of opening the file and setting the fileview */
  void ADIOSWriter::open(mapp::timer &t_io, const std::string & path) {
    // Get the file path

    //Open the file
    t_io.tic();
    open((char*) path.c_str());
    t_io.toc();
  }


  /** \fun write(float * buffer, size_t count)
   \brief Write to file. Inline version to be as fast as possible */
  inline void ADIOSWriter::write(float * buffer, size_t count) {
    size_t global_size = f_->total_bytes() / sizeof(float);
    std::cerr << "START TO WRITE" << std::endl;
    adios_open ( &adios_handle_, "report", report_, (iteration_ == 0 ? "w" :"a"), comm);
    adios_write(  adios_handle_, "global_size", &global_size);
/*
    if (iteration_ == 0)
    for (int i =0; i < f_->wr_blocks(); i++) {
      size_t offset = f_->disp_at(i+1);
      size_t batch_size = f_->length_at(i+1);
      adios_write( adios_handle_, "batch_size" , &batch_size);
      adios_write( adios_handle_, "offset"     , &offset);
      adios_write( adios_handle_, "data"       , &buffer[batch_size*i]); // we assume all our data are aligned in memory
    }
*/
    adios_close( adios_handle_);
    std::cerr << "FINISH WRITE" << std::endl;
    MPI_Barrier (comm);
    iteration_++;
  }

  /** \fun write(mapp::timer &t_io, float * buffer, size_t count)
    \brief Write to file. Since we need to measure the writing time, this function receives
    the timer that should be used to surround the write call */
  void ADIOSWriter::write(mapp::timer &t_io, float * buffer, size_t count) {
    t_io.tic();
    write (buffer, count);
    t_io.toc();
  }

  /** \fun close()
    \brief Close the file. Inline version to be as fast as possible */
  inline void ADIOSWriter::close() {
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
