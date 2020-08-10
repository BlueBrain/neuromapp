
H5INPUT=/home/sergiorg/Desktop/morphologies.h5
H5BMARK_DIR=/home/sergiorg/EPFL/neuromapp/neuromapp/h5benchmark
H5BMARK_BLD_DIR=$H5BMARK_DIR/build
MORPHIO_BLD_DIR=$H5BMARK_DIR/morphio/build
MORPHOKIT_BLD_DIR=$H5BMARK_DIR/morpho-kit/build
DARSHAN_DIR=$H5BMARK_DIR/../../../darshan-3.2.1
DARSHAN_RUNTIME_DIR=$DARSHAN_DIR/darshan-runtime/build
DARSHAN_UTIL_DIR=$DARSHAN_DIR/darshan-util/build
export DARSHAN_OUTPUT_DIR=$H5BMARK_BLD_DIR/darshan

BMARK=0 # Sequential Benchmark
IOAPI=0 # Default HDF5 C Library
IODRV=1 # MPI-IO

if [[ $# -eq 1 ]]
then
    IOAPI=$1
fi

OUTPUT_TS=`date +%s%3N`
OUTPUT_NAME=$DARSHAN_OUTPUT_DIR/h5benchmark_$BMARK\_$IOAPI\_$IODRV\_$OUTPUT_TS

# Compile Morphio, MorphoKit and H5Benchmark
cd $MORPHIO_BLD_DIR && make install && \
cd $MORPHOKIT_BLD_DIR && make install && \
cd $H5BMARK_BLD_DIR && make || exit

mkdir -p $DARSHAN_OUTPUT_DIR 2>/dev/null

export LD_PRELOAD=$DARSHAN_RUNTIME_DIR/lib/libdarshan.so
time mpirun -n 1 $H5BMARK_BLD_DIR/bin/h5benchmark 0 $IOAPI 1 0.0625 $H5INPUT

mv $DARSHAN_OUTPUT_DIR/*_h5benchmark_*.darshan $OUTPUT_NAME.darshan
$DARSHAN_UTIL_DIR/bin/darshan-job-summary.pl \
    --output $OUTPUT_NAME.pdf \
    $OUTPUT_NAME.darshan && \
    atril $OUTPUT_NAME.pdf && \
    echo -e "\nOutput PDF:\n  \"$OUTPUT_NAME.pdf\""
