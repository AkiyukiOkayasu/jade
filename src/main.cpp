#include <Arduino.h>
#include <Wire.h> // I2C
#include "MIDIUSB.h"
#include "MIDIParser.hpp"
#include "pin.hpp"
#include "constants.hpp"

UsbMidiParser midiParser;

uint8_t noteCounter;

void jfSetup()
{
  Wire.beginTransmission(ii::JF_ADDR);
  Wire.write(ii::JF_MODE);
  Wire.write(1); //適当
  Wire.endTransmission();
  delay(1000); //delayMicroseconds(10);に置き換えてもいいかも
}

void iiJF()
{
  Wire.beginTransmission(ii::JF_ADDR);
#if 1
  Wire.write(ii::JF_VOX);
  Wire.write(noteCounter); //all channel
#else
  Wire.write(JF_NOTE);
#endif
  Wire.write(0x00); //Pitch high
  Wire.write(0xFF); //Pitch low
  Wire.write(0xFF); //Vol High
  Wire.write(0xFF); //Vol Low
  Wire.endTransmission();
  noteCounter++;
  if (noteCounter > 6)
  {
    noteCounter = 1;
  }
  delay(1); //delayMicroseconds(10);に置き換えてもいいかも
}

void setup()
{
  SerialUSB.begin(9600);
  noteCounter = 1;
  Wire.begin(); // I2C setup
  pinMode(LED, HIGH);
  digitalWrite(LED, HIGH);
  //jfSetup();
  SerialUSB.println("hoge");
  delay(1000);
}

void loop()
{
  midiEventPacket_t rx;
  //iiJF();
  do
  {
    rx = MidiUSB.read();
    midiParser.parse(rx);
  } while (rx.header != 0);
  delay(1);
}
