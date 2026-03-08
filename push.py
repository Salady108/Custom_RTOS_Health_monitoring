import serial
import os
import sys
import time

# --- UPDATE THESE TO MATCH YOUR PC ---
PORT = '/dev/ttyUSB0'# E.g., 'COM3' on Windows, or '/dev/ttyUSB0' on Linux/Mac
BAUD = 115200
FILE = 'kernel8.img' # The output file from your Makefile
# -------------------------------------

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
except Exception as e:
    print(f"Could not open {PORT}. Close PuTTY/TeraTerm first!")
    sys.exit()

size = os.path.getsize(FILE)

print("Restart your Raspberry Pi now...")
while True:
    line = ser.readline().decode('utf-8', errors='ignore')
    if "Waiting" in line:
        break

print(f"Pi is ready! Sending {FILE} ({size} bytes)...")

# 1. Send magic start word
ser.write(b'R')
ser.write(b'D')
ser.write(b'Y')

# 2. Send size as 4 bytes
ser.write(size.to_bytes(4, byteorder='little'))
time.sleep(0.1)

# 3. Send the actual file
with open(FILE, "rb") as f:
    ser.write(f.read())

print("Done! Streaming live output from Pi:\n")
print("-" * 40)

# 4. Turn this script into a serial monitor to watch the output
while True:
    try:
        print(ser.read().decode('utf-8', errors='ignore'), end="")
    except KeyboardInterrupt:
        break