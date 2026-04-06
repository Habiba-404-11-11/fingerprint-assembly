import serial
import time
import sys
from datetime import datetime
import os

SERIAL_PORT = sys.argv[1] if len(sys.argv) > 1 else 'COM5'
BAUD_RATE = 9600
DOS_LOG_FILE_NAME = 'LOG.TXT' 

print(f"Starting Python Data Logger on {SERIAL_PORT}...")
script_dir = os.path.dirname(os.path.abspath(__file__))
log_file_path = os.path.join(script_dir, DOS_LOG_FILE_NAME)
print(f"Logging data to: {log_file_path}")

try:
    # TEMPORARY TEST CODE: Ensures file is written at start
    with open(DOS_LOG_FILE_NAME, 'a') as f:
        f.write(f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] TEST_START\n")
    print("LOG.TXT file successfully accessed/created.")
    
    # Open the serial port
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=5)
    ser.flushInput()
    time.sleep(2)  

    print("Listening for Arduino result codes (50-ID or 60)...")

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            
            if line.startswith('50-') or line == '60':
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                log_entry = f"[{timestamp}] {line}\n"
                
                with open(DOS_LOG_FILE_NAME, 'a') as f:
                    f.write(log_entry)
                    
                print(f"Logged entry: {line}")
        
        time.sleep(0.01)

except serial.SerialException as e:
    print(f"Error opening serial port {SERIAL_PORT}: {e}")
except KeyboardInterrupt:
    print("\nExiting Serial Bridge.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()