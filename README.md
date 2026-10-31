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

- [ ] Implement energy collection
- [ ] Implement GPU collection
- [ ] Implement MPI calls collections