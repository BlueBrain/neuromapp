/*
 * Neuromapp - args.cpp, Copyright (c), 2015,
 * Kai Langen - Swiss Federal Institute of technology in Lausanne,
 * kai.langen@epfl.ch,
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
 * @file neuromapp/test/nest/connector.cpp
 *  Test on the nest connector module
 */

#define BOOST_TEST_MODULE ConnectorTest
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "utils/error.h"


#include "nest/synapse/models/tsodyks2.h"
#include "nest/synapse/event.h"
#include "nest/synapse/drivers/synapse.h"
#include "nest/synapse/connector_base.h"
#include "nest/synapse/scheduler.h"

#include "coreneuron_1.0/common/data/helper.h" // common functionalities

using nest::ConnectorBase;
using nest::Connector;
using nest::vector_like;
using nest::tsodyks2;

BOOST_AUTO_TEST_CASE(nest_static_connector_size1_test)
{
    //create a connector with one tsodyks2 connection
    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2());
    BOOST_CHECK(conn->get_size() == 1);
}

BOOST_AUTO_TEST_CASE(nest_grow_static_connector_test)
{
    unsigned int K = 2;
    //create a connector with one tsodyks2 connection
    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2());
    bool correct_size = true;
    for(unsigned int i = 1; i < K; ++i){
        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2());
        if((conn->get_size() - 1) != i){
            correct_size = false;
            std::cerr<<"size is "<<conn->get_size()<<". Should be: "<<i<<std::endl;
        }
    }
    BOOST_CHECK(correct_size);
}

/* When K >= K_CUTOFF (3), connector becomes a dynamic container.
 Behaviour should stay the same.
 */
BOOST_AUTO_TEST_CASE(nest_grow_dynamic_connector_test)
{
    unsigned int K = 10;
    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2());
    bool correct_size = true;
    for(unsigned int i = 1; i < K; ++i){
        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2());
        if((conn->get_size() - 1) != i){
            correct_size = false;
            std::cerr<<"size is "<<conn->get_size()<<". Should be: "<<i<<std::endl;
        }
    }
    BOOST_CHECK(correct_size);
}

// no need for erase functionality
//BOOST_AUTO_TEST_CASE(nest_grow_and_shrink_dynamic_test)
//{
//    unsigned int K = 10;
//    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2(0));
//    /* Grow */
//    for(unsigned int i = 1; i < K; ++i){
//        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2(i));
//    }
//
//    /* Shrink */
//    bool correct_size = true;
//    for(unsigned int i = K - 1; i < 0; --i){
//        conn = &((vector_like<tsodyks2>*)conn)->erase(i);
//        if(conn->get_size() != i){
//            correct_size = false;
//            std::cerr<<"size is "<<conn->get_size()<<". Should be: "<<i<<std::endl;
//        }
//    }
//    BOOST_CHECK(correct_size);
//}
//BOOST_AUTO_TEST_CASE(nest_grow_and_shrink_static_test)
//{
//    unsigned int K = K_CUTOFF - 1;
//    ConnectorBase* conn = new Connector<1, tsodyks2>(tsodyks2(0));
//    /* Grow */
//    for(unsigned int i = 1; i < K; ++i){
//        conn = &((vector_like<tsodyks2>*)conn)->push_back(tsodyks2(i));
//    }
//
//    /* Shrink */
//    bool correct_size = true;
//    for(unsigned int i = K - 1; i < 1; --i){
//        conn = &((vector_like<tsodyks2>*)conn)->erase(i);
//        if(conn->get_size() != i){
//            correct_size = false;
//            std::cerr<<"size is "<<conn->get_size()<<". Should be: "<<i<<std::endl;
//        }
//    }
//    BOOST_CHECK(correct_size);
//}

BOOST_AUTO_TEST_CASE(nest_connector_test){
    std::vector<std::string> command_v;
    int error(mapp::MAPP_OK);
    command_v.push_back("connector_execute"); // dummy argument to be compliant with getopt
    command_v.push_back("--iterations");
    command_v.push_back("3");
    error = mapp::execute(command_v,nest::connector_execute);
    BOOST_CHECK(error==mapp::MAPP_OK);
}
