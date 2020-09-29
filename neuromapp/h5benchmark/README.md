# H5Benchmark
The **H5Benchmark** aims at reproducing some of the I/O patterns included in TouchDetector and other components. The small mini-application allows the user to evalute the same input file with the default C API of HDF5, the HighFive C++ wrapper, and also with MorphoKit.

### Compiling and Running H5Benchmark
The first step is to clone the Neuromapp repository and checkout the specific branch that contains the benchmark. It is important to initialize all the submodules and its dependencies, before proceeding.
```shell
git clone git@github.com:BlueBrain/neuromapp.git --branch sergiorg_h5bmark
cd neuromapp
git submodule update --init --recursive
cd neuromapp/h5benchmark
```

#### Compiling H5Benchmark
The following steps explain how to compile the benchmark, including some optional dependencies (e.g., HDF5 with v1.10 API).

##### Optional
The following libraries might be required to compile and analyze the performance. These are already available on BB5, but you might need to compile and configure them locally to ensure compatibility.

**Compile pHDF5**
```shell
cd hdf5
mkdir build && cd build
cmake -DHDF5_ENABLE_PARALLEL=ON -DHDF5_BUILD_CPP_LIB=OFF -DDEFAULT_API_VERSION="v110" -DCMAKE_INSTALL_PREFIX=$PWD ..
make install
export CMAKE_PREFIX_PATH=/path/to/hdf5
```

**Compile Darshan**
```shell
cd darshan-3.2.1/darshan-runtime
./configure --prefix=$PWD/build --with-log-path-by-env="DARSHAN_OUTPUT_DIR" --with-jobid-env=NONE CC=mpicc
make install
cd ../darshan-util
./configure --prefix=$PWD/build CC=mpicc
make install
```

##### Mandatory
The following libraries are mandatorily required to compile and utilize the benchmark. The last step is provided to compile the executable.

**Compile MorphIO**
```shell
cd morphio
mkdir build && cd build
cmake -DBUILD_BINDINGS=OFF -DCMAKE_INSTALL_PREFIX=$PWD ..
make install
```

**Compile MorphoKit**
```shell
cd morpho-kit
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_INSTALL_PREFIX=$PWD -DCMAKE_PREFIX_PATH=$PWD/../../morphio/build ..
make install
```

**Compile H5Benchmark**
```shell
mkdir build && cd build
cmake -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_CXX_FLAGS='-O2 -g' ..
make
```

#### Running H5Benchmark
After the compilation is successful, the `h5benchmark` executable will be available inside the `build/bin` folder. In order to run the benchmark, the following explains the parameters involved:
```shell
h5benchmark [test] (0=Sequential, 1=Random)
            [api] (0=HDF5, 1=HighFive, 2=MorphoKit)
            [driver] (0=POSIX, 1=MPI-IO)
            [factor] (How much data is read / From 0.0 nothing to 1.0 the whole file)
            [path] (Path to the specific H5 file)
```

Thus, if we would like to run a sequential test using 1/16 of the file, with MorphoKit and MPI-IO, the following line specifies the proper parameters:
```shell
h5benchmark 0 2 1 0.0625 /path/to/file/morphologies.h5
```
