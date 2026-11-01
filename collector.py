## :: Copyright 2026
## :: Author    : Lucas Cruz
## :: Title     : MySupervisor HPC Collector (MSHPCC)
## :: Version   : 0.0.1
## :: Maintainer: Lucas Cruz
## :: Email     : lucruz@posgrad.lncc.br
## :: Type      : Research
##
##
## :: Message from maintainer : 
##      Please, if you have any questions about the implementation, don't send it to me!
##      Just God knowns what I've tried to do in here. I don't remember anything anymore.
##      ...
##      I'm just kidding! Besides, just God knowns, you can send me your questions!
__author__ = "Lucas Cruz"
__copyright__ = "Copyright 2026, MySupervisor HPC Collector (MSHPCC) (LNCC)"
__version__ = "0.0.1"
__maintainer__ = "Lucas Cruz"
__email__ = "lucruz@posgrad.lncc.br"
__status__ = "Research"

import os
import time

def get_job_cgroup_path() -> str:
    """Descobre caminho do cgroup do job atual."""

    with open('/proc/self/cgroup') as cgroupfile:
        # no cgroup v2, a linha começa com "0::"
        for line in cgroupfile:
            if line.startswith('0::'):
                p = os.path.join("/sys/fs/cgroup", line.strip().split("::")[1].lstrip("/"))
                return p.split("step_batch")[0]

    return None


def job_monitor(interval=1):
    cgroup_base_path = get_job_cgroup_path()

    if not cgroup_base_path or not os.path.exists(cgroup_base_path):
        print("Erro: Não foi possível determinar o cgroup do Slurm.")
        return

    # seleciono os arquivos com as métricas desejadas
    mem_file = os.path.join(cgroup_base_path, "memory.current")
    cpu_file = os.path.join(cgroup_base_path, "cpu.stat")

    print(f"Monitorando Cgroup: {cgroup_base_path}")

    print(f"CPUs set: {cgroup_base_path}/cpuset.cpus")
    with open(f"{cgroup_base_path}/cpuset.cpus") as f:
        print(f.readlines())

    print(f"CPUs set effective: {cgroup_base_path}/cpuset.cpus.effective")
    with open(f"{cgroup_base_path}/cpuset.cpus.effective") as f:
        rcores = [line.strip().split('-') for line in f.readlines()]
        ncores = [ int(core[1]) - int(core[0]) +1 for core in rcores ]

    print(f"CPUs set exclusive effective: {cgroup_base_path}/cpuset.cpus.exclusive.effective")
    with open(f"{cgroup_base_path}/cpuset.cpus.exclusive.effective") as f:
        print(f.readlines())

    # print(f"{'Tempo':<10} | {'CPU':<6} | {'Memória (MB)':<12}")
    print(f"{'Tempo':<10} | {'CPU':<6} | {'% Alocada':<8} | {'Memória (MB)':<12}")
    print("-" * 55)

    # leitura inicial para CPU já que a métrica é acumulativa
    with open(cpu_file) as f:
        # busco a linha com "usage_usec"
        cpu_start = int([line.split()[1] for line in f if line.startswith("usage_usec")][0])
    time_start = time.time()

    try:
        while True:
            time.sleep(interval)

            # primeira coleta do uso de memória (métrica instantânea)
            with open(mem_file) as f:
                mem_bytes = int(f.read().strip())
                mem_mb = mem_bytes # / (1024 * 1024)

            # segunda coleta do uso de CPU para calcular a diferença
            with open(cpu_file, "r") as f:
                cpu_end = int([line.split()[1] for line in f if line.startswith("usage_usec")][0])
            time_end = time.time()

            # calcula o uso de CPU (microssegundos gastos / tempo real passado em microssegundos)
            # converto primeiro os segundos para microsegundos - 1 segundo = 1000000 microssegundos
            elapsed_time_usec = (time_end - time_start) * 1_000_000
            cpu_delta = cpu_end - cpu_start

            # para o caso do job usar múltiplos cores
            cpu_cores = (cpu_delta / elapsed_time_usec)
            percentage_alocated = ( cpu_cores / sum(ncores) ) * 100

            # exibição simple dos dados (pode melhorar nas próximas versões)
            step_name = os.path.basename(cgroup_base_path)
            # print(f"{time.strftime('%H:%M:%S'):<10} | {step_name:<15} | {cpu_percentage:>6.1f}% | {mem_mb:>10.2f} MB")
            # print(f"{time.strftime('%H:%M:%S'):<10} | {cpu_percentage:>6.1f} | {mem_mb:>10.2f}")
            print(f"{time.strftime('%H:%M:%S'):<10} | {cpu_cores:>6.1f} | {percentage_alocated:>9.1f} | {mem_mb:>10.2f}")

            # atualizo as variáveis para o próximo ciclo
            cpu_start = cpu_end
            time_start = time_end

    except KeyboardInterrupt:
        print("\nMonitoramento encerrado.")

if __name__ == '__main__':
    job_monitor()