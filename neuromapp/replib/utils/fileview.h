/*
 * Neuromapp - fileview.h, Copyright (c), 2015,
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

/**
 * @file neuromapp/replib/utils/fileview.h
 * \brief basic shell for MPI file view (writing distribution)
 */

#ifndef MAPP_FILEVIEW_H
#define MAPP_FILEVIEW_H

#include <mpi.h>
#include <string>
#include <vector>
#include <iostream>

#include "utils/mpi/controler.h"

namespace replib {

class fileview {
private:
    int                         wr_blocks_;
    int                         extra_elems_;
    size_t                      total_bytes_;
    std::vector<int>            lengths_;
    std::vector<MPI_Aint>       displacements_;
    std::vector<MPI_Datatype>   data_types_;
    MPI_Datatype                filetype_; //file's view of frame


public:
    /** \fn fileview(int wb)
        \brief initialize vectors with the appropriate size */
    explicit fileview (int wb = 0) :
            wr_blocks_(wb), extra_elems_(2), total_bytes_(0), lengths_(),
            displacements_(), data_types_(), filetype_() {
        lengths_.reserve(wr_blocks_ + extra_elems_);
        displacements_.reserve(wr_blocks_ + extra_elems_);
        data_types_.reserve(wr_blocks_ + extra_elems_);
    }

    ~fileview() {
        MPI_Type_free(&filetype_);
    }

    /**
    \brief return the number of writing blocks, read only
    */
    inline int wr_blocks() const {
        return wr_blocks_;
    }

    /**
     \brief return the number of writing blocks, write only
     */
    inline int& wr_blocks() {
        return wr_blocks_;
    }

    /**
    \brief return the total number of bytes contained in the lengths_ vector, read only
    */
    inline size_t total_bytes() const {
        return total_bytes_;
    }

    /**
    \brief return the total number of bytes contained in the lengths_ vector, write only
     */
    inline size_t& total_bytes() {
        return total_bytes_;
    }

    /**
    \brief return the position at index of lengths_ vector, read only
    */
    inline int length_at(int index) const {
        return lengths_[index];
    }

    /**
     \brief return the position at index of lengths_ vector, write only
     */
    inline int& length_at(int index) {
        return lengths_[index];
    }

    /**
     \brief add element at the end of the lengths_ vector (push_back), write only
     keeps total_bytes_ count consistent with the elements that are pushed with this function
     ignores first and last values of the vector
     */
    inline void add_length(int elem) {
        if (lengths_.size() > 1) total_bytes_ += lengths_.back();
        lengths_.push_back(elem);
    }

    /**
    \brief return the position at index of displacements_ vector, read only
    */
    inline MPI_Aint disp_at(int index) const {
        return displacements_[index];
    }

    /**
     \brief return the position at index of displacements_ vector, write only
     */
    inline MPI_Aint& disp_at(int index) {
        return displacements_[index];
    }

    /**
     \brief add element at the end of the displacements_ vector (push_back), write only
     */
    inline void add_disp(MPI_Aint elem) {
        displacements_.push_back(elem);
    }

    /**
    \brief return the position at index of data_types_ vector, read only
    */
    inline MPI_Datatype dtype_at(int index) const {
        return data_types_[index];
    }

    /**
     \brief return the position at index of data_types_ vector, write only
     */
    inline MPI_Datatype& dtype_at(int index) {
        return data_types_[index];
    }

    /**
     \brief add element at the end of the data_types_ vector (push_back), write only
     */
    inline void add_dtype(MPI_Datatype elem) {
        data_types_.push_back(elem);
    }


    /**
     \brief call MPI_File_set_view with the given file handler
     */
    inline void set_fileview(MPI_File *fh) {
        // Apply the view
        MPI_Type_struct( wr_blocks_ + extra_elems_, &lengths_[0], &displacements_[0], &data_types_[0], &filetype_ );
        MPI_Type_commit( &filetype_ );

        // No header data in miniapp
        MPI_Offset position_to_write = 0;

        char native[10] = "native";
        int error = MPI_File_set_view(*fh, position_to_write, MPI_FLOAT, filetype_, native, MPI_INFO_NULL);
        if (error != MPI_SUCCESS) {
            std::cout << "[" << mapp::controler::getInstance().rank() << "] Error setting file view" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 913);
        }
    }

    /** \brief the print function, I do not like friend function */
    void print(std::ostream& out) const {
        // Print contents of buffers
          std::cout << mapp::mpi_filter_all();
          std::cout <<  "[" << mapp::controler::getInstance().rank() << "] :: len :: ";
          for (int j = 1; j < lengths_.size() - 1; j++) {
              std::cout << lengths_[j] << " ";
          }
          std::cout << " :: disp :: ";
          for (int j = 1; j < displacements_.size() - 1; j++) {
              std::cout << displacements_[j] << " ";
          }
          std::cout << std::endl;
          std::cout << mapp::mpi_filter_master();
    }
};


/** \brief basic overload the ostream operator to print the fileview class */
inline std::ostream &operator<<(std::ostream &out, fileview const& fv) {
     fv.print(out);
     return out;
}

} //end namespace

#endif /* fileview_h */
