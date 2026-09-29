# Copyright
__author__ = "Lucas Cruz"
__copyright__ = "Copyright 2026, MySupervisor HPC Collector (MSHPCC) (LNCC)"
__version__ = "0.0.1"
__maintainer__ = "Lucas Cruz"
__email__ = "lucruz@posgrad.lncc.br"
__status__ = "Research"

import subprocess
import threading
import argparse
import time
import sys
import os


def build_parser() -> argparse.ArgumentParser:

    parser = argparse.ArgumentParser(
        description="Executes MySupervisor HPC Collector to collectl CPU and memory."
    )

    parser.add_argument(
        "--collector",
        type=str,
        default=os.path.abspath("collector.py"),
        help="Collector script in Python to be used."
    )

    parser.add_argument(
        "--timer",
        type=float,
        default=0.1,
        help="Time to wait until the next verification whether the running application has finished."
    )

    parser.add_argument(
        "--test",
        type=bool,
        default=False,
        help="If this is a test. If it is \"True\", then collector script is not used."
    )

    parser.add_argument(
        "--manager",
        type=bool,
        default=True,
        help="It indicates if Slurm Workload Manager is beeing used. Commoly used to tests."
    )

    parser.add_argument(
        "--warmup",
        type=int,
        default=5,
        help="Stabilization time before the start of the main application."
    )

    parser.add_argument(
        "--cooldown",
        type=int,
        default=5,
        help="Stabilization time before the end of the main application."
    )

    parser.add_argument(
        "command",
        nargs=argparse.REMAINDER,
        help="Command for the application you wish to monitor. Must be preceeded by \"--\"."

    )

    return parser


def get_slurm_jobid() -> str:
    return os.environ.get("SLURM_JOB_ID") or os.environ.get("SLURM_JOBID")


def main() -> int:

    parser = build_parser()
    args = parser.parse_args()

    command = args.command
    if command and command[0] == "--":
        command = command[1:]
    if not command:
        parser.error("You must enter command for the application you wish to monitor.")

    if args.warmup < 0 or args.cooldown < 0:
        parser.error("--warmup and --cooldown can not be negatives.")

    jobid: str = None
    submit_dir: str = None
    logfile: str = None

    if not os.path.isfile(args.collector) and args.test:
        parser.error(f"Collector script not found: {args.collector}")

    if not args.manager:
        jobid = get_slurm_jobid()
        submit_dir = os.environ.get("SLURM_SUBMIT_DIR", os.getcwd)
        logfile = os.path.join(submit_dir, f"mshpcc-{jobid}.log")
    else:
        tmp = os.environ.get("PWD", os.getcwd)
        logfile = os.path.join(tmp, "mshpcc.log")

    print(f"MySupervisor HPC Collector has initialized.")

    # Vamos criar também um sinalizador de encerramento responsável pelo warm inicial e final
    # A ideia é que tenha um tempo de "aquecimento" antes de iniciar a aplicação e depois dela finalizar 
    # A ideia de usar ele é que o programa tenha uma espera interrompível
    # Porque se for usado o sleep e houver alguma interrupção do Slurm durante esse tempo
    # pode acontecer do processo ficar preso no nó. Então esse sinalizador de encerramento pode ajudar
    sinalizer = threading.Event()

    # A principal ideia aqui é iniciar um processo 2 (P2), nesse mesmo processo principal (P1)
    # Esse P2 é o coletor/monitor que vai ser o responsável por realizar toda a coleta
    # O que inclui comunicação com o Slurm e escrita do arquivo com as métricas
    # O processo será dependente (podemos dizer que "P2 é filho de P1").
    collector = subprocess.Popen(
        [sys.executable, '-u', str(args.collector)]
    )

    sinalizer.wait(args.warmup)

    # Também será iniciado um processo 3 (P3), em P1
    # que é a aplicação que se deseja coletar as métricas em questão
    # com start_new_session=True garanto que uma nova sessão seja iniciada
    # isso garante que o processo seja independente de P1.
    # Nesse caso, "P3 não é filho de P1".
    # Isso garante também que qualquer thread ou processo iniciado por P3
    # Possa ser finalizado/encerrado/morto direto por P3
    # De outra forma, se "P1 fosse pai de P3", para encerrar algo de P3
    # seria necessário encerrar P1.
    application = subprocess.Popen(command, start_new_session=True)

    # Aqui fico checando, indefinidamente, se a aplicação finalizou
    # Assim, impeço que a aplicação principal siga executando e imprima que o MSHPCC finalizou a execução
    while application.poll() is None:
        time.sleep(args.timer)

    # Caso a aplicação tenha finalizado, eu quero que:
    # 1. a aplicação finalize de fato todos os processos/threads por ele iniciados
    application.wait()

    sinalizer.wait(args.cooldown)

    # 2. que o coletor/monitor finalize também
    collector.terminate()
    collector.wait()

    print(f"MySupervisor HPC Collector has finished.")

    return sys.exit(application.returncode)


if __name__ == "__main__":
    main()