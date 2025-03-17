#include <Arduino.h>
#include <Wire.h>
// Adafruit servo driver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoOSCWiFi.h>

constexpr char *ssid = "Gul De Sac";
constexpr char *password = "D33pSpac39";
constexpr int bind_port = 8888;
Adafruit_PWMServoDriver pwm_b0 = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS);
int num_lights = 0;
#define LIGHT_FREQ 50            // Analog servos run at ~50 Hz updates
#define USMIN 1000
#define USMAX 2000
#define PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

void setup() {
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
  OscWiFi.subscribe(bind_port, "/LED", on_motor_mode);
  const bool b0_ok = pwm_b0.begin();
  Adafruit_PWMServoDriver pwm_b0 = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS);
}
void loop(void){
  OscWiFi.update();
}
void set_light(const OscMessage& m) {
  const int light_val = m.arg<int>(0);
  print(light_val)
  for(size_t i = 0; i < num_lights; i++){
    pwm_b0.writeMicroseconds(i, light_val);
  }
}
void init(){
  for(size_t i = 0; i < num_lights; i++){
    pwm_b0.setPWM(i, 0, SERVO_PULSE_MIN);
  }
}
