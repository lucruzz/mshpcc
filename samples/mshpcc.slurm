#!/bin/bash
#SBATCH --partition=lncc-h100_dev
#SBATCH --nodelist=sdumont2nd2018
#SBATCH --nodes=1
#SBATCH --tasks=1
#SBATCH --time=00:10:00
#SBATCH --gres=gpu:1
#SBATCH --job-name=teste

module load python/3.14.0-gcc-11.4.1-6heccnp

WORKDIR="$SLURM_SUBMIT_DIR/$SLURM_JOB_ID"

mkdir -p $WORKDIR
cd $WORKDIR

mshpcc srun python3 $SLURM_SUBMIT_DIR/old/samples/parallel-workload.py