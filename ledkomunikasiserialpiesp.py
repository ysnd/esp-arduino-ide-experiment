import serial
import time

# Inisialisasi komunikasi serial
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
time.sleep(2)

def send_command(command):
    ser.write(command.encode())

try:
    while True:
        cmd = input("Masukkan perintah (1 untuk menyalakan LED, 0 untuk mematikan LED): ").strip()
        if cmd in ['1', '0']:
            send_command(cmd)
        else:
            print("Perintah tidak valid.")
except KeyboardInterrupt:
    ser.close()
    print("Program berhenti.")
