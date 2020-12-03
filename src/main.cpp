#include <Arduino.h>
#include <Wire.h> // I2C
#include "MIDIUSB.h"
#include "MIDIParser.hpp"
#include "pin.hpp"
#include "constants.hpp"

UsbMidiParser midiParser;

void setup()
{
  SerialUSB.begin(115200);

  // I2Cピンの内部プルアップ
  pinMode(pin::I2C_SCL, INPUT_PULLUP);
  pinMode(pin::I2C_SDA, INPUT_PULLUP);

  Wire.begin(); // I2C setup

  pinMode(pin::LED, HIGH);
  digitalWrite(pin::LED, HIGH);
  delay(1000);
}

void loop()
{
  midiEventPacket_t rx;
  do
  {
    rx = MidiUSB.read();
    midiParser.parse(rx);
  } while (rx.header != 0);
  delayMicroseconds(50);
}
