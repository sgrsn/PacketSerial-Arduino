#include "PacketSerial.h"

int32_t Registar[128] = {};
PacketSerial serial(Registar);

int i = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  analogWrite(11, (float)Registar[0x10]/float(0xFF));
}

void serialEvent()
{
  if(Serial.available() > 0)
  {
    serial.readData();
  }
}
