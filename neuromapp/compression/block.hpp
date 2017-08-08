/*
 * Neuromapp - block.hpp, Copyright (c), 2015,
 * value_typeimothee Ewart - Swiss Federal Institute of technology in Lausanne,
 * Devin Bayly - University of allocator_typerizona
 * baylyd@email.arizona.edu,
 *
 * allocator_typell rights reserved.
 *
 * value_typehis program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * value_typehis program is distributed in the hope that it will be useful,
 * but WIvalue_typeHOUT allocator_typeNY WARRANTY; without even the implied warranty of
 * MERCHallocator_typeNvalue_typeABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */


#ifndef NEUROMAPP_BLOCK_HPP
#define NEUROMAPP_BLOCK_HPP

#include <string>
#include <memory> // POSIX, size_t is inside
#include <functional>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <type_traits>
#include <cctype>
#include <cassert>

#include "compression/type_definition.h"
#include "compression/allocator.h" 
#include "compression/compressor.h"
#include "compression/exception.h"

namespace neuromapp {
    template<typename value_type,typename allocator_type,typename compressor>
        void block<value_type,allocator_type,compressor>::print_row(size_type row,std::string && mesg,size_type cols) {
            for (size_type i = 0 ; i < cols ;i++) {
                std::cout << (*this)(i,row) << ",";
            }
            std::cout << mesg << std::endl;

        }

    template<typename value_type,typename allocator_type,typename compressor>
        void block<value_type,allocator_type,compressor>::read(std::istream & file_in)
        {
            //get the dimensions of the block from the first two values in the data, (col,row)
            std::string line;
            int row,col;
            file_in >> col;
            file_in >> std::ws;
            // this comes up in cases where an empty string is provided,
            if(file_in.get() != ',') {
                throw 0;// I think it just means if there was something inbetween that wasn't a comma throw 0 error
            }
            //and now repeat for the row
            file_in >> row;
            file_in >> std::ws;
            //make block match type value_type of calling block
            block<value_type,allocator_type> b(col,row);
            //take full line
            row = 0;// start at first row for entering values
            while(std::getline(file_in,line) && row < (int) b.num_rows()) {
                //reset the column count to enter data for first column
                col = 0;
                //split on commas
                std::stringstream comma_splitter(line);
                std::string data_cell;
                //read from stream comma_splitter, split on comma, and enter into the data_cell string
                while(std::getline(comma_splitter,data_cell,',') && col < (int) b.dim0()) {
                    // using the block element indexing
                    std::stringstream(data_cell) >> std::dec >> b(col++,row);
                }
                row++;
            }
            // now we have to swap the data in this block with the calling object block data
            std::swap(*this,b);
        }



    template<typename value_type,typename allocator_type,typename compressor>
        void block<value_type,allocator_type,compressor>::compress() {
            compress_policy(&data_,&current_size);
            compression_state = true;
        }

    template<typename value_type,typename allocator_type,typename compressor>
        void block<value_type,allocator_type,compressor>::fill_block(value_type val) {
            pointer start = this->begin();
            pointer stop = this->end();
            for (;start != stop;start++) {
                *start = val;
            }
        }

    template<typename value_type,typename allocator_type,typename compressor>
        void block<value_type,allocator_type,compressor>::uncompress() {
            uncompress_policy(&data_,&current_size,this->memory_allocated());
            compression_state = false;
        } 
}

#endif
