/*
 * Neuromapp - test.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/test/spike/test.cpp
 *  Test on the Spike Miniapp.
 */

#define BOOST_TEST_MODULE SpikeTest
#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/filesystem.hpp>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <numeric>

#include "test/coreneuron_1.0/helper.h"
#include "spike/algos.hpp"
#include "spike/spike.h"
#include "spike/mpispikegraph.h"
#include "utils/error.h"

namespace bfs = ::boost::filesystem;

static MPI_Datatype mpi_spikeItem;

struct MPIInitializer {
	MPIInitializer(){
		MPI::Init();
		mpi_spikeItem = createMpiItemType(mpi_spikeItem);
	}
	~MPIInitializer(){
	    MPI_Type_free(&(mpi_spikeItem));
		MPI::Finalize();
	}
};

BOOST_GLOBAL_FIXTURE(MPIInitializer);

BOOST_AUTO_TEST_CASE(graph_setup_test){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	int numIn = rand()%(size*numOut);

	MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	sg.setup();
	BOOST_CHECK(sg.inputPresyns_.size() == numIn);
	BOOST_CHECK(sg.outputPresyns_.size() == numOut);
}

BOOST_AUTO_TEST_CASE(distributed_setup_test){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	int numIn = rand()%(size*numOut);

	DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	dsg.setup();
	BOOST_CHECK(dsg.inNeighbors_.size() <= numIn);
	BOOST_CHECK(dsg.outNeighbors_.size() <= size);
	BOOST_CHECK(dsg.inputPresyns_.size() == numIn);
	BOOST_CHECK(dsg.outputPresyns_.size() == numOut);
}

BOOST_AUTO_TEST_CASE(distributed_one_inpresyn){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	int eventsPer = 10;
	int numOut = 2;
	int numIn = 1;

	DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	dsg.setup();
	//with one input presyn per node, inNeighbors.size() == inputPresyns.size()
	BOOST_CHECK(dsg.inNeighbors_.size() == numIn);
}

BOOST_AUTO_TEST_CASE(graph_create_spike){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	int numIn = rand()%(size*numOut);

	MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	sg.setup();

	SpikeItem spike = sg.create_spike();
	//check that the new spike's dest field is set to one of sg's outputPresyns
	bool isValid = false;
	for(int i = 0; i < sg.outputPresyns_.size(); ++i){
		if(spike.dst_ == sg.outputPresyns_[i]){
			isValid = true;
			break;
		}
	}
	BOOST_CHECK(isValid);

}

BOOST_AUTO_TEST_CASE(graph_matches_test){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	//numIn is some value in the range [1, total number of output Presyns]
	int numIn = (rand()%(size*numOut-1)) + 1;

	MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	sg.setup();

	//A spike I create should not match one of my input presyns
	SpikeItem spike = sg.create_spike();
	BOOST_CHECK(!sg.matches(spike));

	int index = rand()%(sg.inputPresyns_.size());
	SpikeItem spike2;
	spike2.t_ = 0.0;
	spike2.dst_ = sg.inputPresyns_[index];
	BOOST_CHECK(sg.matches(spike2));
}

BOOST_AUTO_TEST_CASE(algos_allgather){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	int numIn = rand()%(size*numOut);
	MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	sg.setup();

	allgather(sg);

	int sum = std::accumulate(sg.sizeBuf.begin(), sg.sizeBuf.end(), 0);

	BOOST_CHECK(sum == (size * eventsPer));
	BOOST_CHECK(sg.sizeBuf.size() == size);
	BOOST_CHECK(sg.sendBuf.size() == eventsPer);


	DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	dsg.setup();

	allgather(dsg);

	sum = std::accumulate(dsg.sizeBuf.begin(), dsg.sizeBuf.end(), 0);

	BOOST_CHECK(sum == (dsg.inNeighbors_.size() * eventsPer));
	BOOST_CHECK(dsg.sizeBuf.size() == dsg.inNeighbors_.size());
	BOOST_CHECK(dsg.sendBuf.size() == eventsPer);
}

BOOST_AUTO_TEST_CASE(algos_allgatherv){
    int size = MPI::COMM_WORLD.Get_size();
	int rank = MPI::COMM_WORLD.Get_rank();

	srand(time(NULL) + rank);

	int eventsPer = 10;
	int numOut = 2;
	int numIn = rand()%(size*numOut);
	MpiSpikeGraph sg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	sg.setup();

	allgather(sg);
	allgatherv(sg);

	BOOST_CHECK(sg.recvBuf.size() == (eventsPer * size));


	DistributedSpikeGraph dsg(size, rank, numOut, numIn, eventsPer, mpi_spikeItem);
	dsg.setup();

	allgather(dsg);
	allgatherv(dsg);

	BOOST_CHECK(dsg.recvBuf.size() == (eventsPer * dsg.inNeighbors_.size()));
}
