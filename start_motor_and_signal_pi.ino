#include <AccelStepper.h>
#include "IRremote.h"

int receiver = 13; // Signal Pin of IR receiver to Arduino Digital Pin 13

enum stepper_Motor {
  FUNCTION = 0, DRIVER = 1, FULL2WIRE = 2, FULL3WIRE = 3,
  FULL4WIRE = 4, HALF3WIRE = 6, HALF4WIRE = 8
};

#define sleep_pwr 4
#define pi_signal 7  // Pin to signal Raspberry Pi

AccelStepper stepper_Motor(DRIVER, 3, 2); // stepper(driver, step, dir)
IRrecv irrecv(receiver);    // create instance of 'irrecv'
decode_results results;     // create instance of 'decode_results'

int stepsPerRev = 200;
int gearRatio = 4;
int microSteps = 1;

float speedRPM = 1.0;
float timeRot = 10.0;
int nRot = 3;
float stepperAccell = 500;
int dir = -1;

int nRem = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Initialised"); 
  
  irrecv.enableIRIn(); // Start the receiver
  
  pinMode(sleep_pwr, OUTPUT);
  digitalWrite(sleep_pwr, LOW);

  pinMode(pi_signal, OUTPUT);
  digitalWrite(pi_signal, LOW); // Default LOW

  stepper_Motor.setCurrentPosition(0);
}

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    switch(results.value) {

      case 0xFF6897: // power - stop everything!
        stepper_Motor.setCurrentPosition(0);
        nRem = 0;
        break;

      case 0xFF38C7: // play/pause - switch direction manually
        stepper_Motor.setCurrentPosition(0);
        nRem = nRem + 1;

        // 🔴 Pulse to Pi to start recording
        digitalWrite(pi_signal, HIGH);
        delay(100);  // 100 ms HIGH pulse
        digitalWrite(pi_signal, LOW);
        break;

      case 0x3D9AE3F7: // up
        nRem = nRot;
        dir = -1;
        break;

      case 0xFF4AB5: // down
        nRem = nRot;
        dir = 1;
        break;

      case 0xFFA25D: // 1
        speedRPM = 60.0;
        break;
      
      case 0xFF629D: // 2
        speedRPM = 38.333;
        break;

      case 0xFFE21D: // 3
        speedRPM = 12.777;
        break;
      
      case 0xFF22DD: // 4
        speedRPM = 4.0;
        break;
      
      case 0xFF02FD: // 5
        speedRPM = 5.0;
        break;
      
      case 0xFFC23D: // 6
        speedRPM = 6.0;
        break;

      case 0xFFE01F: // 7
        speedRPM = 7.0;
        break;
      
      case 0x0FFA857: // 8
        speedRPM = 12.777;
        break;

      case 0xFF906F: // 9
        speedRPM = 38.333;
        break;
      
      case 0xFF9867: // 0
        speedRPM = 60.0;
        break;
    }
    irrecv.resume(); // receive the next value
  }

  if (stepper_Motor.distanceToGo() == 0) {
    if (nRem > 0) {
      stepper_Motor.setCurrentPosition(0);
      nRem = nRem - 1;
      dir = dir * -1;

      long nSteps = round(stepsPerRev * gearRatio * microSteps * (speedRPM / 60.0) * timeRot * dir);
      float stepperSpeed = nSteps / timeRot;

      stepper_Motor.setMaxSpeed(stepperSpeed);
      stepper_Motor.setAcceleration(stepperAccell);
      stepper_Motor.moveTo(nSteps);

      Serial.println((String)"nSteps = " + nSteps);
      Serial.println((String)"stepperSpeed = " + stepperSpeed);

      digitalWrite(sleep_pwr, HIGH);
    } else {
      digitalWrite(sleep_pwr, LOW);
    }
  }

  stepper_Motor.run();
}
