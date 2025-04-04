#include <Arduino.h>
#include <Wire.h>
#include <ArduinoOSCWiFi.h>

#define PWM_PIN 4

constexpr char *ssid = "MirrorMotors";
constexpr char *password = "StrobeMotorArray";
constexpr int bind_port = 8888;

#define INPUT_MIN 0
#define INPUT_MAX 100

#define FREQ_MIN 20
#define FREQ_MAX 100

#define PWM_RESOLUTION 8
#define DUTY_MIN 0
#define DUTY_MAX 255

void set_light(const OscMessage &m);

void setup()
{
  Serial.begin(SERIAL_BAUDRATE);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  delay(500);
  Serial.println("Connected!");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(bind_port);
  OscWiFi.subscribe(bind_port, "/LED", set_light);

  // pwm setup
  pinMode(PWM_PIN, OUTPUT);
  analogWriteFrequency(PWM_PIN, FREQ_MAX);
  analogWrite(PWM_PIN, 0);

  delay(500);
}

void loop(void)
{
  OscWiFi.update();
}

void set_light(const OscMessage &m)
{
  int light_freq = m.arg<int>(0);
  const int light_pwm_freq = constrain(light_freq, FREQ_MIN, FREQ_MAX);

  int light_duty = m.arg<int>(1);
  light_duty = constrain(light_duty, 0, 100);
  const int light_pwm_duty = map(light_duty, 0, 100, DUTY_MIN, DUTY_MAX);

  analogWriteFrequency(PWM_PIN, light_pwm_freq);
  analogWrite(PWM_PIN, light_pwm_duty);

  Serial.print(light_pwm_freq);
  Serial.print(",");
  Serial.println(light_pwm_duty);
}
