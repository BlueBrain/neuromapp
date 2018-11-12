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

#ifdef RL_HDF5

#ifndef MAPP_RL_H5P_H
#define MAPP_RL_H5P_H

#include <string.h>
#include <cstring>
#include <cstdlib>

#include "hdf5.h"

#include "replib/backends/basic.h"
#include "replib/utils/config.h"
#include "replib/utils/fileview.h"
#include "replib/mpiio_dist/rnd1b.h"
#include "replib/mpiio_dist/file1b.h"
#include "replib/mpiio_dist/fileNb.h"

#include "utils/mpi/controler.h"
#include "utils/mpi/timer.h"


namespace replib {

class H5PWriter : public replib::Writer {

    private:
        /* Private attributes, no need for getters/setters */
        size_t numElemsPerTStep_;      // Number of floats per time step
        size_t tStepsPerWrite_;        // Number of time steps written at each operation
        size_t numElemsPerWrite_;      // Number of floats to write at each operation
        size_t globalElemsPerTStep_;   // Total number of floats per time step (aggregated)
        size_t totalSimTSteps_;        // Total number of simulation time steps

        /* The following members are private and exclusive of HDF5,
         * so there is no need to implement getters or setters
         */
        hid_t  fid_;        // File ID
        hid_t  data_ds_id_; // Group ID
        hid_t  memspace_;   // Memspace
        hid_t  cplist_;     // Collective I/O property
        hid_t  iplist_;     // Independent I/O property

        hsize_t rankOffset_;                // Offset at which this rank writes wrt the beginning of current time step
        std::vector<hsize_t> dataOffset_;   // Offset at which each rank writes, per dimension
        std::vector<hsize_t> writeDims_;    // Dimensions of the data written by one rank at once
        std::vector<hsize_t> dataDims_;     // Total #elems written in the dataset, per dimension
        std::vector<hsize_t> chunkDims_;    // Dimensions of the HDF5 data chunk


    public:

        H5PWriter();
        ~H5PWriter();

        void init(replib::config &c);
        void open(char * path);
        void open(mapp::timer &t_io, const std::string &path);
        void write(float * buffer, size_t count);
        void write(mapp::timer &t_io, float * buffer, size_t count);
        void close();
        void close(mapp::timer &t_io);
        unsigned int total_bytes();

};
/** \fun H5PWriter()
    \brief create the object, no special actions required for MPI I/O
 */
H5PWriter::H5PWriter() : numElemsPerTStep_(0), tStepsPerWrite_(0), numElemsPerWrite_(0), globalElemsPerTStep_(0),
        totalSimTSteps_(0), fid_(0), data_ds_id_(0), memspace_(0), cplist_(0), iplist_(0), rankOffset_(0) {
    // Create vectors of 2 dimensions
    dataOffset_ = std::vector<hsize_t>(2);
    writeDims_ = std::vector<hsize_t>(2);
    dataDims_ = std::vector<hsize_t>(2);
    chunkDims_ = std::vector<hsize_t>(2);
}


/** \fun ~H5PWriter()
    \brief Class destructor */
H5PWriter::~H5PWriter() {}


/** \fun init(const replib::config &c)
    \brief initialize the object using the given configuration */
void H5PWriter::init(replib::config &c) {
    fileview * f;
    if (c.write() == "file1b") {
        f = file1b(c);
    } else if (c.write() == "fileNb") {
        std::cout << "fileNb: UNIMPLEMENTED" << std::endl;
        return;
        //f = fileNb(c);
    } else { // "rnd1b"
        std::cout << "rnd1b: UNIMPLEMENTED" << std::endl;
        return;
        //f = rnd1b(c);
    }

    numElemsPerWrite_ = f->length_at(1) / sizeof(float);
    tStepsPerWrite_ = c.sim_steps();
    numElemsPerTStep_ = numElemsPerWrite_ / tStepsPerWrite_;
    totalSimTSteps_ = c.sim_steps() * c.rep_steps();
    MPI_Allreduce(&numElemsPerTStep_, &globalElemsPerTStep_, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);

    // This works only for file1b
    MPI_Scan(&numElemsPerTStep_, &rankOffset_, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
    // MPI_Scan is inclusive, subtract the elements of the current rank to find out the real offset
    rankOffset_ -= numElemsPerTStep_;

    // Choose chunk dimensions of the dataset from config or by default
    // The maximum number of elements in a chunk is (2^32)-1
    // The maximum size for any chunk is 4 GB
    if (c.h5_ch_r() == 0) {
        chunkDims_[0] = std::min(totalSimTSteps_, tStepsPerWrite_* 2); // timesteps
        c.h5_ch_r() = chunkDims_[0];
    } else {
        chunkDims_[0] = c.h5_ch_r();
    }

    if (c.h5_ch_c() == 0) {
        chunkDims_[1] = std::min((size_t) 2048, globalElemsPerTStep_); // compartments
        c.h5_ch_c() = chunkDims_[1];
    } else {
        chunkDims_[1] = c.h5_ch_c();
    }
}


/** \fun open(const char * path)
    \brief Open the file with MPI I/O and set the fileview.
           Inline version to be as fast as possible */
inline void H5PWriter::open(char * report) {
    /* HDF5 File creation */
    // Set up file access property list with parallel I/O access
    MPI_Info info = MPI_INFO_NULL;
    hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
    H5Pset_fapl_mpio(plist_id, MPI_COMM_WORLD, info);

    // Create property lists for collective/independent dataset write
    cplist_ = H5Pcreate(H5P_DATASET_XFER);
    iplist_ = H5Pcreate(H5P_DATASET_XFER);
    H5Pset_dxpl_mpio(cplist_, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio(iplist_, H5FD_MPIO_INDEPENDENT);

    // Create a new file collectively
    fid_ = H5Fcreate(report, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);

    // Release property list identifier
    H5Pclose(plist_id);

    /* Create file attributes (Brion compatibility) */
    // magic
    int attr_value = 2682;
    hsize_t attr_size = 1;
    hid_t attr_space = H5Screate_simple(1, &attr_size, &attr_size);
    hid_t attr_magic_id = H5Acreate2(fid_, "magic", H5T_STD_U32LE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_magic_id, H5T_NATIVE_INT, (void*) &attr_value);
    H5Aclose (attr_magic_id);
    H5Sclose (attr_space);

    // version
    int attr_values[2];
    attr_values[0] = 0;
    attr_values[1] = 1;
    attr_size = 2;
    attr_space = H5Screate_simple(1, &attr_size, &attr_size);
    hid_t attr_version_id = H5Acreate(fid_, "version", H5T_STD_U32LE, attr_space, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_version_id, H5T_NATIVE_INT, (void*) &attr_values[0]);
    H5Aclose (attr_version_id);
    H5Sclose (attr_space);

    /* Create group /mapping (Brion compatibility) */
    hid_t group_id = H5Gcreate2(fid_, "/mapping", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);


    // Create list of GIDs, #compartments per gid, etc
    // We assume each neuron has around 1500 compartments (synapses, in fact)
    unsigned int compsLeft = numElemsPerTStep_;
    std::vector<int> numComps = std::vector<int>();
    std::vector<unsigned int> gids = std::vector<unsigned int>();
    int nextgid = (mapp::controler::getInstance().rank() + 1) * 100000;
    // Set random seed for reproducibility
    std::srand(23487);
    while (compsLeft > 0) {
        // Get a random value between -500 and 500, add it to 1500 to get a random number of compartments (or synapses)
        int nextcomps = 1500 + ((std::rand() % 1000) - 500);
        if (nextcomps > compsLeft)
            nextcomps = compsLeft;
        compsLeft -= nextcomps;

        nextgid++;

        numComps.push_back(nextcomps);
        gids.push_back(nextgid);
    }
    unsigned int numGids = gids.size();

    // Coordinate with the other ranks: global number of GIDs and rank offset
    int globalNumGids = 0;
    int offset = 0;
    MPI_Allreduce(&numGids, &globalNumGids, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Scan(&numGids, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    // Scan includes rank's value, subtract it
    offset -= numGids;

    std::vector<unsigned int> gidIndex = std::vector<unsigned int>(numGids);
    gidIndex[0] = rankOffset_;
    for (int i = 1; i < gidIndex.size(); i++)
        gidIndex[i] = gidIndex[i-1] + numComps[i-1];

    std::vector<int> sectionId = std::vector<int>(numElemsPerTStep_);
    int index = 0;
    for (int g = 0; g < numGids; g++) {
        int ncomps = numComps[g];
        for (int i = 0; i < ncomps; i++) {
            sectionId[index] = i;
            index++;
        }
    }

    std::vector<double> time = std::vector<double>(3);
    time[0] = 0.0; // tstart
    time[1] = totalSimTSteps_ * 0.1; // tstop
    time[2] = 0.1; // tstep

    // /mapping/gids dataset
    hsize_t gdims = globalNumGids;
    hid_t gdataspace = H5Screate_simple(1, &gdims, NULL);
    hid_t gdataset = H5Dcreate2(fid_, "/mapping/gids", H5T_STD_U32LE, gdataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    hsize_t off = offset;
    hsize_t dim = numGids;
    hid_t filespace = H5Dget_space(gdataset);
    hid_t memspace = H5Screate_simple(1, &dim, NULL);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &off, NULL, &dim, NULL);
    H5Dwrite(gdataset, H5T_NATIVE_INT, memspace, filespace, iplist_, (void*) &gids[0]);

    H5Sclose(memspace);
    H5Sclose(filespace);
    H5Sclose(gdataspace);
    H5Dclose(gdataset);

    // /mapping/index_pointer dataset
    hsize_t ipdims = globalNumGids;
    hid_t ipdataspace = H5Screate_simple(1, &ipdims, NULL);
    hid_t ipdataset = H5Dcreate2(fid_, "/mapping/index_pointer", H5T_STD_U64LE, ipdataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    off = offset;
    dim = numGids;
    filespace = H5Dget_space(ipdataset);
    memspace = H5Screate_simple(1, &dim, NULL);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &off, NULL, &dim, NULL);
    H5Dwrite(ipdataset, H5T_NATIVE_INT, memspace, filespace, iplist_, (void*) &gidIndex[0]);

    H5Sclose(memspace);
    H5Sclose(filespace);
    H5Sclose(ipdataspace);
    H5Dclose(ipdataset);

    // /mapping/element_id dataset
    hsize_t edims = globalElemsPerTStep_;
    hid_t edataspace = H5Screate_simple(1, &edims, NULL);
    hid_t edataset = H5Dcreate2(fid_, "/mapping/element_id", H5T_STD_U32LE, edataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    off = rankOffset_;
    dim = numElemsPerTStep_;
    filespace = H5Dget_space(edataset);
    memspace = H5Screate_simple(1, &dim, NULL);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &off, NULL, &dim, NULL);
    H5Dwrite(edataset, H5T_NATIVE_INT, memspace, filespace, iplist_, (void*) &sectionId[0]);

    H5Sclose(memspace);
    H5Sclose(filespace);
    H5Sclose(edataspace);
    H5Dclose(edataset);


    // /mapping/time dataset
    hsize_t tdims = 3;
    hid_t tdataspace = H5Screate_simple(1, &tdims, NULL);
    hid_t tdataset = H5Dcreate2(fid_, "/mapping/time", H5T_IEEE_F64LE, tdataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(tdataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &time[0]);
    H5Sclose(tdataspace);
    H5Dclose(tdataset);

    // Close /mapping group
    H5Gclose(group_id);


    // https://stackoverflow.com/questions/15379399/writing-appending-arrays-of-float-to-the-only-dataset-in-hdf5-file-in-c

    /* Create dataset '/data' */
    // Dataset dimensions
    dataDims_[0] = totalSimTSteps_;
    dataDims_[1] = globalElemsPerTStep_;
    int nDims = dataDims_.size();
    std::vector<hsize_t> maxDims = std::vector<hsize_t>(2);
    maxDims[0] = totalSimTSteps_;
    maxDims[1] = dataDims_[1];

    // Dataset chunking dimensions
    int chnDims = chunkDims_.size();

    // Create dataset with chunking
    hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_layout(dcpl, H5D_CHUNKED);
    // Create the dataspace for the dataset
    hid_t space = H5Screate_simple(nDims, &dataDims_[0], &maxDims[0]);
    hid_t err = H5Pset_chunk(dcpl, chnDims, &chunkDims_[0]);
    // Create the dataset with default properties
    data_ds_id_ = H5Dcreate2(fid_, "/data", H5T_NATIVE_FLOAT, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    // Close filespace
    H5Sclose(space);
    H5Pclose(dcpl);

    // Create initial vectors of offsets and length counts
    // Setting the appropriate offsets and counts is tricky
    // This presentation was super useful to understand how hyperslabs work:
    // http://www.speedup.ch/workshops/w37_2008/HDF5-Tutorial-PDF/PSI-HDF5-PARALLEL.pdf
    dataOffset_[0] = 0;
    dataOffset_[1] = rankOffset_;
    writeDims_[0] = tStepsPerWrite_; // #rows at a time
    writeDims_[1] = numElemsPerTStep_; // #comps at a time

    /* Create memspace for hyperslabs */
    memspace_ = H5Screate_simple(writeDims_.size(), &writeDims_[0], NULL);
}

/** \fun open(mapp::timer &t_io, const std::string & path)
    \brief Open the file with MPI I/O and set the fileview.
           Timer will be used to account the time of opening the file and setting the fileview */
void H5PWriter::open(mapp::timer &t_io, const std::string & path) {
    // Get the file path
    char * report = strdup(path.c_str());

    //Open the file
    t_io.tic();
    open(report);
    t_io.toc();
}


/** \fun write(float * buffer, size_t count)
    \brief Write to file. Inline version to be as fast as possible */
inline void H5PWriter::write(float * buffer, size_t count) {
    /* Extend the dataset */
    // We could extend the dataset at each write, but this is not efficient at all!
    //dataDims_[0] += tStepsPerWrite_;
    //dataDims_[1] = globalElemsPerTStep_; --> Value doesn't change
    //H5Dset_extent(data_ds_id_, &dataDims_[0]);

    hid_t filespace = H5Dget_space(data_ds_id_);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, &dataOffset_[0], NULL, &writeDims_[0], NULL);

    /* Write the dataset */
    H5Dwrite(data_ds_id_, H5T_NATIVE_FLOAT, memspace_, filespace, cplist_, (void*) buffer);
    H5Sclose(filespace);

    dataOffset_[0] += tStepsPerWrite_;
}

/** \fun write(mapp::timer &t_io, float * buffer, size_t count)
    \brief Write to file. Since we need to measure the writing time, this function receives
           the timer that should be used to surround the write call */
void H5PWriter::write(mapp::timer &t_io, float * buffer, size_t count) {
    MPI_Status status;
    // Consider whether t_io should only count time spent in H5Dwrite to compute I/O statistics
    t_io.tic();
    write(buffer, count);
    t_io.toc();
}

/** \fun close()
    \brief Close the file. Inline version to be as fast as possible */
inline void H5PWriter::close() {
    H5Sclose(memspace_);
    H5Pclose(cplist_);
    H5Pclose(iplist_);
    H5Dclose(data_ds_id_);
    H5Fclose(fid_);
}

/** \fun close(mapp::timer &t_io)
    \brief Close the file.
           Timer will be used to account the time of closing the file */
void H5PWriter::close(mapp::timer &t_io) {
    t_io.tic();
    close();
    t_io.toc();
}

/** \fun total_bytes()
    \brief Return the total amount of data (in bytes) written at each reporting step */
unsigned int H5PWriter::total_bytes() {
    return numElemsPerWrite_ * sizeof(float);
}

} // end namespace

#endif // MAPP_RL_H5P_H
#endif // RL_HDF5
