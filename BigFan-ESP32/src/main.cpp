#include <Arduino.h>
#include <Wire.h>
#include <ArduinoOSCWiFi.h>

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
constexpr char *ssid = "Gul De Sac";
constexpr char *password = "D33pSpac39";
constexpr int bind_port = 8888;

// timer
uint32_t timer_prev = 0;
constexpr uint32_t timer_delta = 50000U;

void on_motor_set(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    int speed_preset = m.arg<int>(0);
    speed_preset = constrain(speed_preset, 0, 100);
    const uint32_t target_duty = map(speed_preset, 0, 100, 0, pwm_duty_max);

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

// void enable_tacho_task(void * /* parameters */)
// {
//     // Set tacho pin as input
//     pinMode(feedback_tacho_pin, INPUT);
//     // initial values
//     tacho_time = micros();
//     // attach
//     attachInterrupt(feedback_tacho_pin, tacho_time_isr, RISING);
//     vTaskDelete(NULL);
// }

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

    // Set tacho pin as input
    pinMode(feedback_tacho_pin, INPUT);
    // initial values
    tacho_time = micros();
    // attach
    attachInterrupt(feedback_tacho_pin, tacho_time_isr, RISING);

    // subscribe osc packet and directly bind to variable
    OscWiFi.subscribe(bind_port, "/bigmot/set", on_motor_set);

    // wait before running
    delay(500);
}

// ===== LOOP =====
// ================

void loop()
{
    // read tacho feedback
    uint32_t feedback_rpm = read_tacho_rpm();

    // read knob
    // const uint16_t knob_read_raw = analogRead(knob_pin);
    // const uint16_t knob_read_val = constrain(knob_read_raw, knob_val_min, knob_val_max);
    // const uint32_t command_pulse_duty = map(knob_read_val, knob_val_min, knob_val_max, 0, pwm_duty_max);

    // set command
    if (!ledcWrite(command_pwm_pin, command_pulse_duty));
    {
        Serial.println("Error: Failed to write to ledc");
    }
    Serial.print(command_pulse_duty);
    Serial.print(",");
    Serial.println(feedback_rpm);
    delay(200);
}
