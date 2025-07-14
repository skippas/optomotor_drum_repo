import RPi.GPIO as GPIO
import time

# Use BCM pin numbering (GPIO 17 is physical pin 11)
INPUT_PIN = 17

GPIO.setmode(GPIO.BCM)
GPIO.setup(INPUT_PIN, GPIO.IN)

print("Listening for Arduino signal on GPIO 17...")

try:
    while True:
        state = GPIO.input(INPUT_PIN)
        print(f"Signal: {'HIGH' if state else 'LOW'}")
        time.sleep(0.2)
except KeyboardInterrupt:
    print("Stopped.")
finally:
    GPIO.cleanup()