#include <Arduino.h>
#include <Wire.h>
// Adafruit servo driver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_PWMServoDriver.h>
// #include <ArduinoOSCWiFi.h>

constexpr char *ssid = "Gul De Sac";
constexpr char *password = "D33pSpac39";
constexpr int bind_port = 8888;

Adafruit_PWMServoDriver pwm_b0 = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS);

#define NUM_LIGHTS 2

// #define LIGHT_FREQ 50            // Analog servos run at ~50 Hz updates
#define USMIN 1000
#define USMAX 2000
#define PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

// void set_light(const OscMessage& m);

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi ..");
  // while (WiFi.status() != WL_CONNECTED) {
  //     Serial.print('.');
  //     delay(1000);
  // }
  // delay(500);
  // Serial.println("Connected!");
  // Serial.print(WiFi.localIP());
  // Serial.print(":");
  // Serial.println(bind_port);
  // OscWiFi.subscribe(bind_port, "/LED", set_light);
  const bool b0_ok = pwm_b0.begin();
  if (b0_ok) {
    pwm_b0.setPWMFreq(50);
    for (size_t i = 0; i < NUM_LIGHTS; i++) {
      pwm_b0.setPWM(i, 0, 0);
    }
    Serial.println("Ready");
  } else {
    Serial.println("Failed to start pwm");
  }
  delay(500);
}

void loop(void){
  // OscWiFi.update();

  for (size_t i = 0; i < NUM_LIGHTS; i++) {
    pwm_b0.setPWM(i, 0, 200);
  }
  delay(1000);
  for (size_t i = 0; i < NUM_LIGHTS; i++) {
    pwm_b0.setPWM(i, 0, 3000);
  }
  delay(1000);
}

// void set_light(const OscMessage& m) {
//   int light_freq = m.arg<int>(0);
//   light_freq = constrain(light_freq, 0, 1000);
//   const int light_pwm_freq = map(light_freq, 0, 1000, 20, 100);

//   int light_duty = m.arg<int>(1);
//   light_duty = constrain(light_duty, 0, 1000);
//   const int light_pwm_duty = map(light_duty, 0, 1000, 0, 2048);

//   pwm_b0.setPWMFreq(light_pwm_freq);
//   for (size_t i = 0; i < NUM_LIGHTS; i++) {
//     pwm_b0.setPWM(i, 0, light_pwm_duty);
//   }

//   Serial.print(light_pwm_freq);
//   Serial.print(",");
//   Serial.println(light_pwm_duty);
// }
