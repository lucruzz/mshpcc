import threading
import time

done = False


def worker():
    counter = 0
    while not done:
        time.sleep(1)
        counter += 1
        print(counter)


worker()
