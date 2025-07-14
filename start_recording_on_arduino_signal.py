import RPi.GPIO as GPIO
import time
from datetime import datetime
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder
from picamera2.outputs import FileOutput

# --- Config ---
GPIO_PIN = 17              # GPIO pin where Arduino sends signal
VIDEO_LENGTH = 5           # seconds
SAVE_DIR = "/home/pi/videos"

# --- Setup GPIO ---
GPIO.setmode(GPIO.BCM)
GPIO.setup(GPIO_PIN, GPIO.IN)

# --- Setup Camera ---
picam2 = Picamera2()
video_config = picam2.create_video_configuration(main={"size": (1920, 1080)})
picam2.configure(video_config)
picam2.start()

print("Waiting for signal from Arduino...")

try:
    while True:
        if GPIO.input(GPIO_PIN) == GPIO.HIGH:
            print("Signal received. Starting recording...")

            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            filename = f"{SAVE_DIR}/recording_{timestamp}.h264"

            encoder = H264Encoder()
            picam2.start_recording(encoder, FileOutput(filename))
            time.sleep(VIDEO_LENGTH)
            picam2.stop_recording()

            print(f"Recording saved to: {filename}")

            # Wait for signal to go LOW again before looping
            while GPIO.input(GPIO_PIN) == GPIO.HIGH:
                time.sleep(0.01)

        time.sleep(0.01)

except KeyboardInterrupt:
    print("Exiting...")

finally:
    GPIO.cleanup()
    picam2.close()
