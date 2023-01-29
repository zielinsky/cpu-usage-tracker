#!/usr/bin/env python3

import subprocess
import signal
import time

def sigterm_case():
    process = subprocess.Popen(["./cputracker"], stdout=subprocess.PIPE)
    time.sleep(2)
    # Wysłanie sygnału SIGTERM do procesu
    process.send_signal(signal.SIGTERM)

    # Oczekiwanie na zakończenie procesu
    process.wait()

    # Odczytywanie wyjścia programu
    output = process.stdout.read()

    if b"Error: Thread not responding... Terminating" in output:
        print("SIGTERM TEST: [OK]")
    else:
        print("SIGTERM TEST: [NO]")

def valgrind_case():
    process = subprocess.Popen(["valgrind --leak-check=full ./cputracker -s"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    time.sleep(6)
    # Wysłanie sygnału SIGTERM do procesu
    process.send_signal(signal.SIGTERM)

    # Oczekiwanie na zakończenie procesu
    process.wait()

    # Odczytywanie wyjścia programu
    output = process.stderr.read()

    if b"All heap blocks were freed -- no leaks are possible" in output:
        print("VALGRIND TEST: [OK]")
    else:
        print("VALGRIND TEST: [NO]")
        print(output)

sigterm_case()
valgrind_case()