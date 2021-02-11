/** jade main
    @file main.cpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu
*/

#include "MIDI/MIDIParser.hpp"
#include "MIDIUSB.h"
#include "pin.hpp"

#include <Arduino.h>
#include <Wire.h> // I2C

UsbMidiParser midiParser;

/** 
    [0, 127]の範囲の2つのuint8を[0, 255]とする.
    @param msb [0, 127]
    @param lsb [0, 127]
    @return constexpr uint8_t [0, 255]
    @note SysExの仕組み上128以上の数は送信できないのでI2Cの1byteを2byteに分割して送信している
*/
constexpr uint8_t margeBytes (const uint8_t msb, const uint8_t lsb)
{
    return (msb << 4) | (lsb & 0x0F);
}

void sysExCallback (const uint8_t sysEx[], const uint8_t size)
{
    const uint8_t addr = margeBytes (sysEx[0], sysEx[1]);
    Wire.beginTransmission (addr);

    for (uint_fast8_t i = 2; i < size; i += 2)
    {
        Wire.write (margeBytes (sysEx[i], sysEx[i + 1]));
    }
    Wire.endTransmission();
}

void setup()
{
    SerialUSB.begin (115200);

    midiParser.onNoteOn = [] (MIDI::Note note) {
        digitalWrite (pin::LED, LOW);
    };

    midiParser.onNoteOff = [] (MIDI::Note note) {
        digitalWrite (pin::LED, HIGH);
    };

    midiParser.onControlChange = [] (MIDI::ControlChange cc) {
        if (cc.value > 0)
        {
            digitalWrite (pin::LED, LOW);
        }
        else
        {
            digitalWrite (pin::LED, HIGH);
        }
    };

    midiParser.onSysEx = sysExCallback;

    // I2Cピンの内部プルアップ
    pinMode (pin::I2C_SCL, INPUT_PULLUP);
    pinMode (pin::I2C_SDA, INPUT_PULLUP);

    Wire.begin(); // I2C setup

    pinMode (pin::LED, HIGH);
    digitalWrite (pin::LED, HIGH);
    delay (1000);
}

void loop()
{
    midiEventPacket_t rx;
    do
    {
        rx = MidiUSB.read();
        midiParser.parse (rx);
    } while (rx.header != 0);
    delayMicroseconds (50);
}
