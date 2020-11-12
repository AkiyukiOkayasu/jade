#include <Arduino.h>
#include "MIDIUSB.h"
#include "MIDIParser.hpp"
#include "pin.hpp"

UsbMidiParser midiParser;

void noteOn(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED, HIGH);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  digitalWrite(LED, LOW);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(LED, HIGH);
  digitalWrite(LED, HIGH);
}

void loop()
{
  midiEventPacket_t rx;
  do
  {
    rx = MidiUSB.read();
    midiParser.parse(rx);
  } while (rx.header != 0);
}
