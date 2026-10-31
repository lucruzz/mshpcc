# Copyright
__author__ = "Lucas Cruz"
__copyright__ = "Copyright 2026, MySupervisor HPC Collector (MSHPCC) (LNCC)"
__version__ = "0.0.1"
__maintainer__ = "Lucas Cruz"
__email__ = "lucruz@posgrad.lncc.br"
__status__ = "Research"

import threading


def workload():
    print("Workload initialized...")

    counter = 1

    for i in range(1, 100_000_000):
        counter += i

    print("Workload finalized...")


def main():

    # thread = threading.Thread(target=workload)
    # thread.start()
    # thread.join()
    N = 24
    threads = [ threading.Thread(target=workload) for _ in range(N) ]
    start = [ thread.start() for thread in threads ]
    join = [ thread.join() for thread in threads ]

    print("Main thread finished.")

if __name__ == '__main__':
    main()