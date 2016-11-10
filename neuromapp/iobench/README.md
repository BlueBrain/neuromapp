# iobench Description #

This mini-app can be used to benchmark the performance of key/value stores applied 
to neuroscience use cases. It creates a given number of K/V pairs in the form of 
(key(char *), value(char *)). The key represents the ID of a neuron (GID) and the 
value represents an array of floats (compartment voltages). Then, it inserts all 
the generated K/V pairs in the K/V store and computes the bandwidth and IOPS 
delivered by the K/V store. Finally, it retrieves the inserted values, computes 
the bandwidth and IOPS delivered and checks the retrieved values for correctness.

## iobench Steps ##

iobench supports multi-process (MPI) and multi-thread (OMP) if available. The 
steps performed by the mini-app can be described as follows:

1) Initialization

2) Write test:

```
    for "i" iterations {
        #pragma omp parallel {
            for "n" pairs {
               insert(kv_pair)
            }
            if (asynchronous) wait_for_insertions_to_complete()
        }
    }
```

3) Read test:

```
    for 1 iteration {
        #pragma omp parallel {
            for "n" pairs {
                retrieve(kv_pair)
            }
            if (asynchronous) wait_for_retrievals_to_complete()
        }
    }
```

4) Check retrieved values match inserted values

5) Finalization

## iobench Parameters ##

This mini-app has several parameters that can be configured in order to tune it. 
Passing the '-h' parameter will print a help message listing the possible 
parameters and a short description.

Here's a more detailed description:
* --help, -h: Produce the help message
* --backend, -b [string]: K/V backend to use. Supported backends are: map (STL map, 
    default), leveldb (LevelDB), cassandra (Cassandra)
* --pairs, -n, [unsigned int]: Number of k/v pairs to insert/retrieve per thread 
    and per iteration (default, 1024)
* --its, -i [unsigned int]: Number of iterations per thread (default, 10)
* --skip, -s [unsigned int]: Number of initial iterations to discard. Use this parameter 
    for warm-up purposes (default, 0)
* --numproc, -p [unsigned int]: Number of MPI processes to use. Use 0 to disable MPI 
    (default, 0)
* --threads, -t [unsigned int]: Number of OpenMP threads to use (default, 1)
* --keySize, -k [size_t]: Size of each key in bytes (default, 64)
* --valSize, -v [size_t]: Size of each value in bytes (default, 1024)
* --compress, -c [0/1]: If offered by the K/V backend, compress data (default, 1)
* --read, -r [0/1]: Perform read test (default, 1)
* --write, -w [0/1]: Perform write test (default, 1)
* --rrd [0/1]: Retrieve k/v pairs in random order (default, 1)
* --rwr [0/1]: Insert k/v pairs in random order (default, 1)

## Other Useful Information ##

Some other relevant information for this mini-app:

* If '--skip S' is used, iobench will run 'S' additional iterations at the beginning, 
  but the result of these iterations will not be taken into account to compute bandwidth 
  and IOPS.  
* The total amount of data inserted/retrieved per thread in one iteration is: 
  ``` valSize * pairs ```
* The total amount of data inserted per thread is: ``` valSize * pairs * its ```. Please, 
  remember that if 'skip' is used, more data will be inserted 
  (``` valSize * pairs * (its + skip) ```), but 'skip' iterations are discarded in order 
  to compute bandwidth and IOPS.
* The total amount of data inserted is (not accounting for skip iterations): 
  ``` valSize * pairs * its * threads * numproc ```
* The mini-app is prepared to support asynchronous operations. By now, this behavior is fixed 
  according to the backend: map (no asynchronous operations), LevelDB (no asynchronous 
  operations because there is no way to synchronize afterwards), Cassandra (asynchronous 
  operations issued, and then all synchronized at once, as described in mini-app steps).
