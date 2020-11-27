#include <Arduino.h>
#include <Wire.h> // I2C
#include "MIDIUSB.h"
#include "MIDIParser.hpp"
#include "pin.hpp"
#include "constants.hpp"

UsbMidiParser midiParser;

void jfSetup()
{
  Wire.beginTransmission(ii::JF_ADDR);
  Wire.write(ii::JF_MODE);
  Wire.write(1); //JF mode 1
  Wire.endTransmission();
  delay(1000); //delayMicroseconds(10);に置き換えてもいいかも
}

void iiJF()
{
  Wire.beginTransmission(ii::JF_ADDR);
#if 1
  Wire.write(ii::JF_VOX);
  Wire.write(0); //0: all output
#else
  Wire.write(JF_NOTE);
#endif
  Wire.write(0x00); //Pitch high
  Wire.write(0xFF); //Pitch low
  Wire.write(0xFF); //Vol High
  Wire.write(0xFF); //Vol Low
  Wire.endTransmission();
  delay(1); //delayMicroseconds(10);に置き換えてもいいかも
}

void setup()
{
  SerialUSB.begin(9600);
  Wire.begin(); // I2C setup
  pinMode(LED, HIGH);
  digitalWrite(LED, HIGH);
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
  delay(1);
}
