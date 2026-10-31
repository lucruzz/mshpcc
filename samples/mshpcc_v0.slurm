#!/bin/bash
#SBATCH --cpus-per-task=1
#SBATCH --job-name=teste
#SBATCH --mem=1000M
#SBATCH --time=00:01:00

set -euo pipefail

MSHPCC_DIR="$PWD/mshpcc"
WORKDIR="${SLURM_SUBMIT_DIR}/${SLURM_JOB_ID}"

cd $MSHPCC_DIR
mkdir -p $WORKDIR

python3 mshpcc.py \
    --workdir ${WORKDIR} \
    --warmup 5
    --cooldown 5 \
    -- python3 workload.py