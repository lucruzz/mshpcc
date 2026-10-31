import threading
import signal
import time
import sys

# def signal_handler(signum, frame):
#     print("Interrupção!")
#     sys.exit()


# handler_signals = {
#     signum: signal.signal(signum, signal_handler) 
#     for signum in (signal.SIGINT, signal.SIGTERM)
# }

while True:
    print('Hello')
    time.sleep(1)

# for signum, previous in handler_signals.items():
#     signal.siganl(signum, previous)