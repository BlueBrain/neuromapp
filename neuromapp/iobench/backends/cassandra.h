/*
 * Neuromapp - cassandra.h, Copyright (c), 2015,
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
 * GNU General Public License for more details..  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 */

/**
 * @file neuromapp/iobench/backends/cassandra.h
 * iobench Miniapp: Cassandra backend
 */

#ifndef MAPP_IOBENCH_CASS_
#define MAPP_IOBENCH_CASS_

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Get OMP header if available
#include "utils/omp/compatibility.h"
#include "iobench/backends/basic.h"

// Cassandra header
#include <cassandra.h>


/** \fn createDB()
    \brief Create the appropriate DB backend
    \param backend name of the desired DB to create
    \param db std::vector returning the initialized DBs
 */
inline void createDB (const std::string & backend, std::vector<BaseKV *> & db);


class KVStatusCass : public KVStatus {
    public:
        void * _key;
        void * _value;
        size_t _key_size;
        size_t _value_size;
        CassFuture * _future;

        bool success() { return cass_future_error_code(_future) == CASS_OK; }

        ~KVStatusCass() { cass_future_free(_future); }
};


class CassKV : public BaseKV {

    private:
        CassCluster * _cluster;
        CassSession * _session;
        const char *  _wr_query;
        const char *  _rd_query;

        void print_error(CassFuture* future);
        CassCluster* create_cluster();
        CassError connect_session(CassSession* session, const CassCluster* cluster);
        CassError execute_query(CassSession* session, const char* query);

    public:
        CassKV() : _cluster(NULL), _session(NULL), _wr_query(NULL), _rd_query(NULL) {}
        void initDB(iobench::args &a);
        void finalizeDB();
        inline void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline size_t getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size);
        inline void waitKVput(std::vector<KVStatus *> &status, int start, int end);
        inline void waitKVget(std::vector<KVStatus *> &status, int start, int end);
        void deleteDB();
        std::string getDBName() { return "cassandra"; }

        void createKVStatus(int n, std::vector<KVStatus *> &status);
};


/** \fn void CassKV::print_error(CassFuture* future)
    \brief Function to print Cassandra errors
 */
void CassKV::print_error(CassFuture* future) {
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);
    fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

/** \fn CassCluster* CassKV::create_cluster()
    \brief Create a Cassandra cluster object
 */
CassCluster* CassKV::create_cluster() {
    CassCluster* cluster = cass_cluster_new();
    cass_cluster_set_contact_points(cluster, "127.0.0.1");
    cass_cluster_set_protocol_version(cluster, 3);
    cass_cluster_set_write_bytes_high_water_mark(cluster, 1024*1024*10); // 10 MB
    cass_cluster_set_write_bytes_high_water_mark(cluster, 1024 * 4*1024 * 200); // Make sure we have enough buffer space
    return cluster;
}

/** \fn CassError CassKV::connect_session(CassSession* session, const CassCluster* cluster)
    \brief Connect to a Cassandra cluster
 */
CassError CassKV::connect_session(CassSession* session, const CassCluster* cluster) {
    CassError rc = CASS_OK;
    CassFuture* future = cass_session_connect(session, cluster);

    cass_future_wait(future);
    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }
    cass_future_free(future);

    return rc;
}

/** \fn CassError CassKV::execute_query(CassSession* session, const char* query)
    \brief Execute a Cassandra query
 */
CassError CassKV::execute_query(CassSession* session, const char* query) {
    CassError rc = CASS_OK;
    CassFuture* future = NULL;
    CassStatement* statement = cass_statement_new(query, 0);

    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    cass_future_free(future);
    cass_statement_free(statement);

    return rc;
}

/** \fn void initDB(iobench::args &a)
    \brief Init the needed data for the specific DB
 */
void CassKV::initDB(iobench::args &a)
{
    _cluster = create_cluster();
    _session = cass_session_new();

    if (connect_session(_session, _cluster) != CASS_OK) {
        cass_cluster_free(_cluster);
        cass_session_free(_session);
        return;
    }

    _wr_query = "INSERT INTO my_kv (key, value) VALUES (?, ?);";
    _rd_query = "SELECT value FROM my_kv WHERE key = ?;";

    execute_query(_session,
            "CREATE KEYSPACE IF NOT EXISTS my_keyspace WITH replication = { \
                    'class': 'SimpleStrategy', 'replication_factor': '1' };");

    execute_query(_session,
            "CREATE TABLE IF NOT EXISTS my_keyspace.my_kv (key text, \
                    value text, PRIMARY KEY (key, value));");
                    //WITH compression = { 'sstable_compression' : '' };");

    execute_query(_session, "USE my_keyspace");

    std::cout << "Cassandra connection: OK!" << std::endl;
}

/** \fn void finalizeDB()
    \brief Finalize the needed data for the specific DB
 */
void CassKV::finalizeDB()
{
    CassFuture * close_future = cass_session_close(_session);
    cass_future_wait(close_future);
    cass_future_free(close_future);

    cass_cluster_free(_cluster);
    cass_session_free(_session);
    printf("Cassandra disconnection: OK!\n");
}

/** \fn void putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Insert the given k/v pair into the DB
 */
inline void CassKV::putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    CassStatement* statement = cass_statement_new(_wr_query, 2);
    cass_statement_bind_string(statement, 0, (char *) key);
    cass_statement_bind_string(statement, 1, (char *) value);

    ((KVStatusCass *) kvs)->_future = cass_session_execute(_session, statement);

    cass_statement_free(statement);
}

/** \fn size_t putKV(KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
    \brief Retrieve from the DB the associated value to the given key. Returns retrieved value size
 */
inline size_t CassKV::getKV (KVStatus * kvs, void * key, size_t key_size, void * value, size_t value_size)
{
    CassStatement* statement = cass_statement_new(_rd_query, 1);
    cass_statement_bind_string(statement, 0, (char *) key);

    ((KVStatusCass *) kvs)->_future = cass_session_execute(_session, statement);
    ((KVStatusCass *) kvs)->_key = key;
    ((KVStatusCass *) kvs)->_value = value;
    ((KVStatusCass *) kvs)->_key_size = key_size;
    ((KVStatusCass *) kvs)->_value_size = value_size;

    cass_statement_free(statement);

    return value_size;
}

/** \fn void waitKVput(std::vector<KVStatus *> &status, int start, int end)
    \brief Wait until all the insertions associated to status are committed to the DB
 */
inline void CassKV::waitKVput(std::vector<KVStatus *> &status, int start, int end)
{
    for (int i = start; i < end; i++) {
        CassFuture* future = ((KVStatusCass *) status[i])->_future;
        cass_future_wait(future);
    }
}

/** \fn void waitKVget(std::vector<KVStatus *> &status, int start, int end)
    \brief Wait until all the queries associated to status are retrieved from the DB
 */
inline void CassKV::waitKVget(std::vector<KVStatus *> &status, int start, int end)
{
    for (int i = start; i < end; i++) {
        CassFuture* future = ((KVStatusCass *) status[i])->_future;
        cass_future_wait(future);

        const CassResult* result = cass_future_get_result(future);

        if (cass_result_row_count(result) > 0) {
            const CassRow* row = cass_result_first_row(result);

            const CassValue* column1 = cass_row_get_column(row, 0);

            if (column1 != NULL) {
                const char* string_value;
                size_t string_value_length;
                cass_value_get_string(column1, &string_value, &string_value_length);

                std::memcpy(((KVStatusCass *) status[i])->_value, string_value, std::min(((KVStatusCass *) status[i])->_value_size, string_value_length));

                //std::cout << "Row count is: " << cass_result_row_count(result) << std::endl
                //        << "Got value with strlength: " << string_value_length << " ;;; value: " << string_value << std::endl;
            } //else {
            //  std::cout << "Value NULL!" << std::endl;
            //  break;
            //}
        }
        cass_result_free(result);
    }
}

/** \fn void deleteDB()
    \brief Clear DB contents
 */
void CassKV::deleteDB()
{
    execute_query(_session,
            "DROP KEYSPACE my_keyspace");

    execute_query(_session,
            "CREATE KEYSPACE IF NOT EXISTS my_keyspace WITH replication = { \
                    'class': 'SimpleStrategy', 'replication_factor': '1' };");

    execute_query(_session,
            "CREATE TABLE IF NOT EXISTS my_keyspace.my_kv (key text, \
                    value text, PRIMARY KEY (key, value));");
                    //WITH compression = { 'sstable_compression' : '' };");

    execute_query(_session, "USE my_keyspace");
}

/** \fn void createKVStatus(int n, std::vector<KVStatus *> &status)
    \brief Create the needed structures to handle asynchronous insertions. Opaque class from the outside
 */
void CassKV::createKVStatus(int n, std::vector<KVStatus *> &status)
{
    for (int i = 0; i < n; i++) {
        status.push_back(new KVStatusCass());
    }
}


#endif // MAPP_IOBENCH_CASS_
