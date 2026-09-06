import subprocess
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

    print(f"MySupervisor HPC Collector has initialized...")

    collector = subprocess.Popen(
        [sys.executable, '-u', str(args.collector)]
    )

    application = subprocess.Popen(command)

    while application.poll() is None:
        time.sleep(1)


    if not application.returncode:
        collector.terminate()

    print(f"MySupervisor HPC Collector has finished...")

if __name__ == "__main__":
    main()