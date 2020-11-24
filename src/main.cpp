#include <Arduino.h>
#include <Wire.h> // I2C
#include "MIDIUSB.h"
#include "MIDIParser.hpp"
#include "pin.hpp"

UsbMidiParser midiParser;

namespace
{
  constexpr uint8_t JF_ADDR = 0x70;
}

void iiJF()
{
  Wire.beginTransmission(JF_ADDR);
  Wire.write(0x40); //適当
  Wire.write(0x11); //適当
  Wire.endTransmission();
  delay(1); //delayMicroseconds(10);に置き換えてもいいかも
}

void setup()
{
  Wire.begin(); // I2C setup
  pinMode(LED, HIGH);
  digitalWrite(LED, HIGH);
  iiJF();
}

void loop()
{
  midiEventPacket_t rx;
  iiJF();
  do
  {
    rx = MidiUSB.read();
    midiParser.parse(rx);
  } while (rx.header != 0);
  delay(5);
}
