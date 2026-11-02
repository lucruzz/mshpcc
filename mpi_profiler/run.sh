#!/bin/bash

PROFILER=$(realpath libmshpcc2.so)
LOG_DIR=$(realpath logs)

mkdir -p logs

# mpirun -n 2 env LD_PRELOAD="$PROFILER" MSHPCC_MPI_LOG_DIR="$LOG_DIR" ../samples/mpi/test_mpi

export LD_PRELOAD="$PROFILER"
export MSHPCC_MPI_LOG_DIR="$LOG_DIR"

mpirun --oversubscribe -n 8 ../samples/mpi/test_mpi
