import serial
import time

# Inisialisasi komunikasi serial
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=0.5)
time.sleep(2) 

def send_command(command):
    ser.write(command.encode())

try:
    while True:
        if ser.in_waiting > 0:
            ir_state = ser.readline().decode().strip()
            print(f"IR State: {ir_state}")
            if ir_state == '0':  # Objek terdeteksi
                send_command('1')  # Nyalakan LED
            else:  # Tidak ada objek
                send_command('0')  # Matikan LED
        time.sleep(0.05)  
except KeyboardInterrupt:
    ser.close()
    print("Program berhenti.")
