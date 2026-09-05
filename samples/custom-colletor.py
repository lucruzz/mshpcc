import time

print(f"{'Tempo':<10}")
print("---------------")
while True:
    print(f"{time.strftime('%H:%M:%S')}")
    time.sleep(1)