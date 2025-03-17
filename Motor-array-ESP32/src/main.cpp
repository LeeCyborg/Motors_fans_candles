#include <Arduino.h>
#include <Wire.h>
// Adafruit servo driver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_PWMServoDriver.h>
#include <ArduinoOSCWiFi.h>

// Analog read pin for speed
constexpr uint8_t knob_pin = 33;
constexpr uint16_t knob_val_min = 10;
constexpr uint16_t knob_val_max = 4085;

// timings
#define SERVO_FREQ 50            // Analog servos run at ~50 Hz updates

#define USMIN 1000
#define USMAX 2000

#define SERVO_PULSE_MIN 204 // This is the 'minimum' pulse length count (out of 4096)
#define SERVO_PULSE_MAX 410 // This is the 'maximum' pulse length count (out of 4096)

#define NUM_SERVOS_B0 16
#define NUM_SERVOS_B1 6
#define NUM_SERVOS (NUM_SERVOS_B0 + NUM_SERVOS_B1)


// motor array
int motor_array[NUM_SERVOS_B0 + NUM_SERVOS_B1] = {};

// objects - servo breakout boards
Adafruit_PWMServoDriver pwm_b0 = Adafruit_PWMServoDriver(PCA9685_I2C_ADDRESS);
Adafruit_PWMServoDriver pwm_b1 = Adafruit_PWMServoDriver(0x41);

// network wifi
constexpr char *ssid = "Gul De Sac";
constexpr char *password = "D33pSpac39";
constexpr int bind_port = 8888;

struct Motor{ 
    int id;
    int offset;
    int enable;
    int value;
    Adafruit_PWMServoDriver board;
};
// int offset_list[][]; 
Motor motor_set[NUM_SERVOS_B0 + NUM_SERVOS_B1];

void initialize_motors()
{
    for (int mot_id = 0; mot_id < NUM_SERVOS_B0; ++mot_id)
    {
        pwm_b0.setPWM(mot_id, 0, SERVO_PULSE_MIN);
    }
    for (int mot_id = 0; mot_id < NUM_SERVOS_B1; ++mot_id)
    {
        pwm_b1.setPWM(mot_id, 0, SERVO_PULSE_MIN);
    }
}

void set_all_motors(const uint32_t microsec)
{
    for (int mot_id = 0; mot_id < NUM_SERVOS_B0; ++mot_id)
    {
        pwm_b0.writeMicroseconds(mot_id, microsec);
    }
    for (int mot_id = 0; mot_id < NUM_SERVOS_B1; ++mot_id)
    {
        pwm_b1.writeMicroseconds(mot_id, microsec);
    }
}

void set_motor_microsec(const uint8_t board_index, const uint8_t motor_index, uint32_t microsec)
{
    if (board_index == 0)
    {
        pwm_b0.writeMicroseconds(motor_index, microsec);
    }
    else if (board_index == 1)
    {
        pwm_b1.writeMicroseconds(motor_index, microsec);
    }
}

// OSC Callbacks
void make_wave(const OscMessage& m){
    for (float pos = 0; pos < 2*PI; pos+0.1){
        float val = cos(pos);
        val = constrain(map(val, -1, 1, 1000, 2000), 1000, 2000);
        pwm_b1.writeMicroseconds(1, val);
        delay(100);
    }
    for (float pos = 2*PI; pos > 0; pos-0.1){
        float val = cos(pos);
        val = constrain(map(val, -1, 1, 1000, 2000), 1000, 2000);
        pwm_b1.writeMicroseconds(1, val);
        delay(100);
    }
}

void on_motor_set_array(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    for (size_t ind = 0; ind < m.size(); ++ind)
    {
        const int mot_val = m.arg<int>(ind);
        int mot_index = ind;
        int board_index = -1;

        int mot_microsec = USMIN;
        if (mot_val == 2)
        {
            mot_microsec = 1200;
        }
        else if (mot_val == 3)
        {
            mot_microsec = 1400;
        }
        else if (mot_val == 4)
        {
            mot_microsec = 1600;
        }
        else if (mot_val == 5)
        {
            mot_microsec = 2000;
        }

        if (ind < NUM_SERVOS_B0)
        {
            board_index = 0;
            pwm_b0.writeMicroseconds(mot_index, mot_microsec);
        }
        else if (ind < (NUM_SERVOS_B0 + NUM_SERVOS_B1))
        {
            board_index = 1;
            mot_index = ind - NUM_SERVOS_B0;
            pwm_b1.writeMicroseconds(mot_index, mot_microsec);
        }

        Serial.print(mot_val);
        Serial.print(" ");
    }
    Serial.println();
}

void on_motor_set_all(const OscMessage& m) {
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");

    const int mot_val = m.arg<int>(0);
    if (mot_val == 1)
    {
        set_all_motors(USMIN);
    }
    else if (mot_val == 2)
    {
        set_all_motors(1050);
    }
    else if (mot_val == 3)
    {
        set_all_motors(1100);
    }
    else if (mot_val == 4)
    {
        set_all_motors(1200);
    }
    else if (mot_val == 5)
    {
        set_all_motors(1500);
    }

    Serial.print(mot_val);
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
    // subscribe osc packet and directly bind to variable
    OscWiFi.subscribe(bind_port, "/mot/setarr", on_motor_set_array);
    OscWiFi.subscribe(bind_port, "/mot/setall", on_motor_set_all);
    OscWiFi.subscribe(bind_port, "/mot/mode", on_motor_mode);
    // Servo setup
    const bool b0_ok = pwm_b0.begin();
    const bool b1_ok = pwm_b1.begin();
    // do not continue if failed to initialize
    if (!b0_ok || !b1_ok){
        Serial.println("Failed to initialize i2c servo breakout");
    }else{
        // Set base servo frequency
        pwm_b0.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
        pwm_b1.setPWMFreq(SERVO_FREQ); // Analog servos run at ~50 Hz updates
        // start at 0 for all motors
        initialize_motors();
    }
    delay(1000);
}

void loop(){
    OscWiFi.update();

    // read knob
    // const uint16_t knob_read_raw = analogRead(knob_pin);
    // const uint16_t knob_read_val = constrain(knob_read_raw, knob_val_min, knob_val_max);
    // const uint32_t microsec = map(knob_read_val, knob_val_min, knob_val_max, USMIN, USMAX);
    // set_all_motors(microsec);

    // debug : show
    // Serial.println(microsec);

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

void setup_motors(){
    for(int i = 0; i < NUM_SERVOS_B0 + NUM_SERVOS_B1; i++){
        motor_set[i].offset = 0;
        motor_set[i].enable = 1;
        motor_set[i].value = 1000;
        if(i < NUM_SERVOS_B0){
            motor_set[i].id = i;
            motor_set[i].board = pwm_b0;
        } else { 
            motor_set[i].id = i-NUM_SERVOS_B0;
            motor_set[i].board = pwm_b1;
        }
    }
    for (int index = 0; index < NUM_SERVOS_B0+NUM_SERVOS_B1; ++index){
        motor_set[index].board.writeMicroseconds(motor_set[index].id, SERVO_PULSE_MIN);
    }
}