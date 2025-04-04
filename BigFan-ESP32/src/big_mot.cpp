#include <Arduino.h>
#include <Wire.h>
#include <ArduinoOSCWiFi.h>
#include "MotLerp.h"

// ===== CONFIGURE PINS =====
// ==========================

// feedback tachometer
constexpr uint8_t feedback_tacho_pin = 17;

// command speed pwm
constexpr uint8_t command_pwm_pin = 33;
// Command speed PWM pin
constexpr uint32_t pwm_freq = 5000;
constexpr uint8_t pwm_resolution = 12;
constexpr uint32_t pwm_duty_max = 4095;

// // Input pot
// constexpr uint8_t knob_pin = 4;
// // Analog read pin for frequency
// constexpr uint32_t knob_val_min = 10;
// constexpr uint32_t knob_val_max = 4085;

// ===== WIFI OSC =====
// =====================

// network wifi
constexpr char *ssid = "MirrorMotors";
constexpr char *password = "StrobeMotorArray";
constexpr int bind_port = 8888;

// timer
uint32_t timer_prev = 0;
constexpr uint32_t timer_delta = 50000U;

// motor lerp
MotLerp motlerp = {};

// motor lerp
void on_motor_set(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    int speed_preset = m.arg<int>(0);
    speed_preset = constrain(speed_preset, 0, 100);
    const uint32_t target_duty = map(speed_preset, 0, 100, 0, pwm_duty_max);
    motlerp.set_target(target_duty);

    Serial.print(speed_preset);
    Serial.print(" ");
    Serial.println();
}

// ===== TACHOMETER =====
// ======================

// output time when pin is triggered
volatile uint32_t tacho_time = 0;
// time since last pin was triggered
volatile uint32_t tacho_delta = 0;

// tacho interrupts
static void IRAM_ATTR tacho_time_isr()
{
    const uint32_t tacho_trip_time = micros();
    tacho_delta = tacho_trip_time - tacho_time;
    tacho_time = tacho_trip_time;
}

void enable_tacho(void)
{
    // Set tacho pin as input
    pinMode(feedback_tacho_pin, INPUT);
    // initial values
    tacho_time = micros();
    // attach
    attachInterrupt(feedback_tacho_pin, tacho_time_isr, RISING);
    // vTaskDelete(NULL);
}

// read tacho
uint32_t read_tacho_rpm()
{
    const uint32_t delta = tacho_delta;
    if (delta == 0)
    {
        return 0;
    }
    else
    {
        return 60000000U / delta;
    }
}

// ===== SETUP =====
// =================

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
    Serial.println("Connected!");
    Serial.print(WiFi.localIP());
    Serial.print(":");
    Serial.println(bind_port);

    // attach input pwm
    const bool pwm_ok = ledcAttach(command_pwm_pin, pwm_freq, pwm_resolution);
    if (!pwm_ok)
    {
        // run forever
        while (true)
        {
            Serial.println("Error: Failed to enable ledc");
            delay(500);
        }
    }
    // initial 0 command
    ledcWrite(command_pwm_pin, 0);

    // subscribe osc packet and directly bind to variable
    OscWiFi.subscribe(bind_port, "/bigmot/set", on_motor_set);

    // wait before running
    delay(500);
}

// ===== LOOP =====
// ================

void loop()
{
    OscWiFi.update();

    const uint32_t now = micros();
    if ((now - timer_prev) > timer_delta)
    {
        timer_prev = now;

        const int command_pulse_duty = motlerp.evaluate();
        if (!ledcWrite(command_pwm_pin, command_pulse_duty));
        {
            // Serial.println("Error: Failed to write to ledc");
        }

        // read tacho feedback
        // uint32_t feedback_rpm = read_tacho_rpm();
        // Serial.print(command_pulse_duty);
        // Serial.print(",");
        // Serial.println(feedback_rpm);
    }
}
