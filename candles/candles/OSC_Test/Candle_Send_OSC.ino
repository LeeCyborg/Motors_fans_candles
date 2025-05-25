// https://github.com/CNMAT/OSC/tree/master
// https://pypi.org/project/python-osc/
#include "Arduino.h"
#include "WiFi.h"
#include <OSCMessage.h>
#include <OSCBundle.h>
WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
int LED_BUILTIN = 2;
IPAddress ip;
char ssid[] = "MirrorMotors"; // your network SSID (name)
char pass[] = "StrobeMotorArray";  // your network password
unsigned int localPort = 8888; // local port to listen for OSC packets
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Start");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Loading");
  }
  ip = WiFi.localIP();
  Serial.println(ip);
  Udp.begin(localPort);
}

void ledtoggle(OSCMessage &msg, int addrOffset) {
  Serial.println(msg.getInt(0));
  switch (msg.getInt(0)) {
  case 0:
    digitalWrite(2, LOW);
    break;
  case 1:
    digitalWrite(2, HIGH);
    break;
  }
}

void receiveMessage() {
  OSCBundle bundleIN;
   int size;
 
   if( (size = Udp.parsePacket())>0)
   {
     while(size--)
       bundleIN.fill(Udp.read());
      if(!bundleIN.hasError())
        bundleIN.route("/led", ledtoggle);
   }
}
void loop() {
  receiveMessage();
}
