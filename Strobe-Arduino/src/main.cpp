#include <Arduino.h>
#include "TimerOne.h"

// CONFIGURATION
// =============

// initial pwm values
constexpr uint32_t initial_period_micros = 1000000 / 50;
constexpr unsigned int initial_duty = 5;

// Strobe LED pin
const int led = 15;
// Analog read pin for frequency
const int freq_pot = 33;
// Analog read pin for ON duration (duty cycle)
const int duty_pot = 35;

// frequency configuration
// scaled frequency from pot between 0 and 100
const int freq_read_min = 0;
const int freq_read_max = 100;
// Output frequency in Hz
const int freq_output_min = 20;
const int freq_output_max = 120;

// duty cycle configuration
// scaled duty cycle from pot between 0 and 100
const int duty_read_min = 0;
const int duty_read_max = 100;
// Duty cycle range to send to Timer1.setPwmDuty()
const int duty_output_min = 0;
const int duty_output_max = 500;

// STATE
// =====

// period and duty cycle state
int state_freq = 0;
int state_duty = 0;

// SETUP
// =====

void setup() {
  Serial.begin(115200);

  // Set up pwm output on pin
  pinMode(led, OUTPUT);

  // set initial values
  Timer1.initialize(initial_period_micros);
  Timer1.pwm(led, initial_duty);
}

// RUN
// ===

void loop(void)
{

  // Read Pots
  const int read_freq_analog = map(analogRead(freq_pot), 10, 800, freq_read_min, freq_read_max);
  const int read_freq = constrain(read_freq_analog, freq_read_min, freq_read_max);

  const int read_duty_analog = map(analogRead(duty_pot), 10, 800, duty_read_min, duty_read_max);
  const int read_duty = constrain(read_duty_analog, duty_read_min, duty_read_max);

  // update period when changed
  if (read_freq != state_freq)
  {
    state_freq = read_freq;
    const int output_freq = map(state_freq, freq_read_min, freq_read_max, freq_output_min, freq_output_max);
    // adjust period
    const uint32_t output_period = 1000000 / output_freq;
    // output duty is from state, needed to set pwm properly
    const unsigned int output_duty = map(state_duty, duty_read_min, duty_read_max, duty_output_min, duty_output_max);
    Timer1.pwm(led, output_duty, output_period);
    // feedback
    Serial.print("(f, d) = (");
    Serial.print(output_freq);
    Serial.print(", ");
    Serial.print(state_duty);
    Serial.println(")");
  }

  // update duty when changed
  if (read_duty != state_duty)
  {
    state_duty = read_duty;
    const unsigned int output_duty = map(state_duty, duty_read_min, duty_read_max, duty_output_min, duty_output_max);
    // adjust duty cycle
    Timer1.setPwmDuty(led, output_duty);
    // feedback
    Serial.print("duty = ");
    Serial.println(state_duty);
  }

  delay(100);
}
