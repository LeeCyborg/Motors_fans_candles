#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define POT_PIN A4

// timings
#define SERVO_FREQ 50            // Analog servos run at ~50 Hz updates
#define SERVO_CONTROL_STEP_MS 20 // Controller running at 50Hz

#define USMIN 1000
#define USMAX 2000

#define SERVO_PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO_PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

// number of servos
#define NUM_SERVOS 17
const int freq_pot = A3;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setup()
{
    Serial.begin(9600);
    // Servo setup
    pwm.begin();
    // for servo example see https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/examples/servo/servo.ino
    // pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
    for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    {
        pwm.setPWM(mot_id, 0, SERVO_PULSE_MIN);
    }
    delay(1000);
}

void loop()
{


    for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    {
        const int read_freq = map(analogRead(freq_pot), 10, 1000, USMIN, USMAX);
        const int microsec = constrain(read_freq, USMIN, USMAX);
        Serial.println(read_freq);
        if(read_freq > 1500){
            if(mot_id%2){
                pwm.writeMicroseconds(mot_id, USMAX);
            } else { 
                pwm.writeMicroseconds(mot_id, USMIN);
            }
        } else { 
            if(mot_id%2){
                pwm.writeMicroseconds(mot_id, USMIN);
            } else { 
                pwm.writeMicroseconds(mot_id, USMAX);
            }
        }
    }
    delay(100);
    // // int input = map(analogRead(POT_PIN), 10, 1000, USMIN, USMAX);
    // // int pulse = constrain(input, USMIN, USMAX);
    // // pwm.writeMicroseconds(SERVO_ID0, pulse);
    // // delay(100);

    // for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    // {
    //     pwm.setPWM(mot_id, 0, SERVO_PULSE_MIN);
    // }
    // delay(100);

    // // Drive each servo one at a time using writeMicroseconds(), it's not precise due to calculation rounding!
    // // The writeMicroseconds() function is used to mimic the Arduino Servo library writeMicroseconds() behavior.
    // for (uint16_t microsec = USMIN; microsec < USMAX; microsec++) {
    //     for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    //     {
    //         pwm.writeMicroseconds(mot_id, microsec);
    //     }
    // }

    // delay(500);
    // for (uint16_t microsec = USMAX; microsec > USMIN; microsec--) {
    //     for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    //     {
    //         pwm.writeMicroseconds(mot_id, microsec);
    //     }
    // }

}
