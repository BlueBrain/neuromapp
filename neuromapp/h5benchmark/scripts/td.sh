#!/bin/sh

set -xe

if [[ $# -ne 6 ]]
then
    echo "Error: The number of parameters is incorrect!"
    exit -1
fi

export NUM_PROCS=$1
export TOUCH_DETECTOR_BIN=$2
export MORPHPATH=$3 # /gpfs/bbp.cscs.ch/project/proj16/spykfunc/circuits/O1.v6a/morphologies/h5
export POP=$4 # One, Centi, Deci
export SUF=$5 # one, centi, deci
export PREFIX=$6
export CIRCUIT=O1
export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export MV2_ENABLE_AFFINITY=0
export INPUT_DIR=/gpfs/bbp.cscs.ch/home/matwolf/work/bldtc-183
export DATA_DIR=/gpfs/bbp.cscs.ch/project/proj16/spykfunc/circuits/${CIRCUIT}.v6a
#export OUT_DIR=/gpfs/bbp.cscs.ch/data/scratch/proj16/srivas/touchdetector_output
export NODE="${INPUT_DIR}/tau/edges_O1.h5"
#export NODES="${INPUT_DIR}/circuits/nodes_O1_${SUF}.h5"
export NODES="${DATA_DIR}/circuit.mvd3"
export RECIPE="$DATA_DIR/builderRecipeAllPathways.xml"

if [ -n "${JOBSIZE}" ]; then
    JOBSIZE="--jobsize ${JOBSIZE}"
fi

#mkdir -p "${OUT_DIR}"

echo "Using touchdetector binary:" "$TOUCH_DETECTOR_BIN"

# run application under Darshan
$PREFIX srun -n ${NUM_PROCS} "$TOUCH_DETECTOR_BIN" \
    --output="$OUT_DIR" \
    --touchspace=axodendritic \
    --from "${NODE}" One \
    --to "${NODES}" ${POP} \
    ${JOBSIZE} \
    "${RECIPE}" \
    "${MORPHPATH}" \
    2>&1 | tee output.txt

echo "Finished!"

