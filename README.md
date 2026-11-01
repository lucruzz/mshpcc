# MySupervisor HPC Collector

⚙️`on development`

MySupervisor HPC Collector (MSHPCC) is a tool for monitoring CPU, GPU, energy, memory and MPI calls on HPC jobs on systems that use Slurm Workload Manager. It may serves researchers who do not have admin access and want to collect the behavior of jobs during execution time. 

--- 

## Requirements

- Python 3.11+

## How to install

```bash
./install.sh install
```

MSHPCC will be installed on `~/.opt/apps/mshpcc` and a wrapper will be created on `~/.local/bin`, called by `mshpcc`.

## Running

```bash
mshpcc <your-command-application>
```

## TODO

- [x] CPU data collection
- [x] Memory data collection
- [ ] Energy data collection
- [ ] GPU data collection
- [ ] Change the entire implementation to C++
- [ ] Blocked communication: intercepting MPI_Send and MPI_Recv, including rank, source/destination, size, and duration
- [ ] Collective communication: add MPI_Bcast, MPI_Reduce, MPI_Allreduce, and MPI_Alltoall
- [ ] Non-blocking communication: adding MPI_Isend, MPI_Irecv, and completion mechanisms
- [ ] Integration: correlate MPI records with CPU and memory metrics collected by mshpcc
- [ ] Transform output to CSV format
- [ ] Implement a complete PDF report