#include <Arduino.h>
#include "MIDIUSB.h"
#include "MIDIParser.hpp"

constexpr int LED = 13;
USBMIDIParser midip;

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
    if (rx.header != 0)
    {
      // note on
      switch (rx.header)
      {
      case 0x08:
        digitalWrite(LED, HIGH);
        break;
      case 0x09:
        if (rx.byte3 > 0)
        {
          digitalWrite(LED, LOW);
        }
        else
        {
          digitalWrite(LED, HIGH);
        }
        break;
      }
    }
  } while (rx.header != 0);
}
