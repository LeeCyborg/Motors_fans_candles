#include <Arduino.h>
#include <Wire.h>
// #include <freertos/FreeRTOS.h>
// Adafruit servo driver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoOSCWiFi.h>

#include "MotLerp.h"

// Analog read pin for speed
constexpr uint8_t knob_pin = 33;
constexpr uint16_t knob_val_min = 10;
constexpr uint16_t knob_val_max = 4085;

// timings
// Analog servos run at ~50 Hz updates
#define SERVO_FREQ 50

#define USMIN 1000
#define USMAX 2000

#define SERVO_PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO_PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

#define NUM_SERVOS_B0 16
#define NUM_SERVOS_B1 6
#define NUM_SERVOS (NUM_SERVOS_B0 + NUM_SERVOS_B1)

// objects - servo breakout boards
Adafruit_PWMServoDriver pwm_b0 = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS);
Adafruit_PWMServoDriver pwm_b1 = Adafruit_PWMServoDriver(0x41);

// network wifi
constexpr char *ssid = "MirrorMotors";
constexpr char *password = "StrobeMotorArray";
constexpr int bind_port = 8888;

// timer
uint32_t timer_prev = 0;
constexpr uint32_t timer_delta = 50000U;

struct Motor {
    bool enable = false;
    int id = 0;
    int index = 0;
    int offset = 0;
    MotLerp lerp = {};
    Adafruit_PWMServoDriver* board = nullptr;
};

// array holding all motor data
Motor motor_set[NUM_SERVOS];

// offset to make all motors spin at similar speeds
constexpr int motor_offsets[NUM_SERVOS] = {
    0, 0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

// call this once during setup after initializing pwm boards
void setup_motors() {
    for(int i = 0; i < NUM_SERVOS; i++) {
        motor_set[i].enable = true;
        motor_set[i].id = i;
        motor_set[i].offset = motor_offsets[i];
        motor_set[i].lerp.reset(1000);
        if (i < NUM_SERVOS_B0) {
            motor_set[i].index = i;
            motor_set[i].board = &pwm_b0;
        } else {
            motor_set[i].index = i-NUM_SERVOS_B0;
            motor_set[i].board = &pwm_b1;
        }
    }
    for (int index = 0; index < NUM_SERVOS; ++index) {
        motor_set[index].board->writeMicroseconds(motor_set[index].index, motor_set[index].lerp.value());
    }
}

// call this every loop to update lerp and send out motor pwm
void loop_motors() {
    for (int index = 0; index < NUM_SERVOS; ++index) {
        motor_set[index].board->writeMicroseconds(motor_set[index].index, motor_set[index].lerp.evaluate());
    }
}

// set same lerp target for all motors
void set_all_motors(const int microsec)
{
    for (int index = 0; index < NUM_SERVOS; ++index) {
        motor_set[index].lerp.set_target(microsec);
    }
}

void on_motor_set_array(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    for (size_t ind = 0; ind < m.size(); ++ind) {
        int speed_percent = m.arg<int>(ind);
        if (speed_percent > 0)
        {
            speed_percent = speed_percent + motor_set[ind].offset;
        }
        speed_percent = constrain(speed_percent, 0, 100);
        const int mot_microsec = map(speed_percent, 0, 100, USMIN, USMAX);
        motor_set[ind].lerp.set_target(mot_microsec);

        Serial.print(speed_percent);
        Serial.print(" ");
    }
    Serial.println();
}

void on_motor_set_all(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    int speed_percent = m.arg<int>(0);
    speed_percent = constrain(speed_percent, 0, 100);
    const int mot_microsec = map(speed_percent, 0, 100, USMIN, USMAX);
    set_all_motors(mot_microsec);

    Serial.print(speed_percent);
    Serial.print(" ");
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

void setup(){

    Serial.begin(SERIAL_BAUDRATE);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    delay(500);
    Serial.println("Connected!");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(bind_port);

    // Servo setup
    const bool b0_ok = pwm_b0.begin();
    const bool b1_ok = pwm_b1.begin();
    // do not continue if failed to initialize
    if (!b0_ok || !b1_ok) {
        Serial.println("Failed to initialize i2c servo breakout");
    } else {
        // Set base servo frequency
        pwm_b0.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
        pwm_b1.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
        // start at 0 for all motors
        setup_motors();
    }

    // subscribe osc packet and directly bind to variable
    OscWiFi.subscribe(bind_port, "/mot/setarr", on_motor_set_array);
    OscWiFi.subscribe(bind_port, "/mot/setall", on_motor_set_all);
    OscWiFi.subscribe(bind_port, "/mot/mode", on_motor_mode);

    delay(1000);
}

void loop(){
    OscWiFi.update();

    const uint32_t now = micros();
    if ((now - timer_prev) > timer_delta)
    {
        timer_prev = now;
        loop_motors();
    }

}
