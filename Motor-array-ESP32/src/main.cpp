#include <Arduino.h>

#include <Wire.h>
// Adafruit servo driver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoOSCWiFi.h>

// Analog read pin for frequency
const int freq_pot = A3;

// timings
#define SERVO_FREQ 50            // Analog servos run at ~50 Hz updates

#define USMIN 1000
#define USMAX 2000

#define SERVO_PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO_PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

// number of servos
#define NUM_SERVOS 3

// motor array state
int motor_enable[NUM_SERVOS] = {};

#define OSC_ONLY

// objects - servo
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

constexpr char *ssid = "Gul De Sac";
constexpr char *password = "D33pSpac39";
constexpr int bind_port = 8888;

void on_motor_enable(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    for (size_t ind = 0; ind < m.size(); ++ind)
    {
        if (ind < NUM_SERVOS)
        {
            motor_enable[ind] = m.arg<int>(ind);
        }

        Serial.print(ind);
        Serial.print(":");
        Serial.print(m.arg<int>(ind));
        Serial.print(" ");
    }
    Serial.println();
}

void on_motor_mode(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    Serial.print(m.arg<String>(0));
    Serial.println();
}

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    delay(500);
    Serial.println(WiFi.localIP());

    // subscribe osc packet and directly bind to variable
    OscWiFi.subscribe(bind_port, "/mot/enable", on_motor_enable);
    OscWiFi.subscribe(bind_port, "/mot/mode", on_motor_mode);

#ifndef OSC_ONLY
    // Servo setup
    if (!pwm.begin())
    {
        while(true)
        {
            Serial.println("Failed to initialize i2c servo breakout");
            delay(2000);
        }
    }
    // for servo example see https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/blob/master/examples/servo/servo.ino
    // pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates

    for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    {
        pwm.setPWM(mot_id, 0, SERVO_PULSE_MIN);
    }
    delay(1000);
#endif

}

void loop()
{
    // int input = map(analogRead(POT_PIN), 10, 1000, USMIN, USMAX);
    // int pulse = constrain(input, USMIN, USMAX);
    // pwm.writeMicroseconds(SERVO_ID0, pulse);
    // delay(100);

    #ifndef OSC_ONLY
    const int read_freq = map(analogRead(freq_pot), 10, 1000, USMIN, USMAX);
    const int microsec = constrain(read_freq, USMIN, USMAX);

    for (int mot_id = 0; mot_id < NUM_SERVOS; ++mot_id)
    {
        pwm.writeMicroseconds(mot_id, microsec);
    }
    #endif

    OscWiFi.update(); // should be called

    // Drive each servo one at a time using writeMicroseconds(), it's not precise due to calculation rounding!
    // The writeMicroseconds() function is used to mimic the Arduino Servo library writeMicroseconds() behavior.
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
