/** jade main
    @file main.cpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu
*/

#include "Adafruit_ZeroTimer.h"
#include "MIDI/MIDIParser.hpp"
#include "MIDIUSB.h"
#include "pin.hpp"

#include <Arduino.h>
#include <Wire.h> // I2C

UsbMidiParser midiParser;

// This example can have just about any frequency for the callback
// automatically calculated!
float freq = 5.0; // 1 KHz

// timer tester
Adafruit_ZeroTimer zerotimer = Adafruit_ZeroTimer (3);

void TC3_Handler()
{
    Adafruit_ZeroTimer::timerHandler (3);
}

// the timer callback
volatile bool togglepin = false;
void TimerCallback0 (void)
{
    //digitalWrite (LED_BUILTIN, togglepin);
    digitalWrite (pin::D2, togglepin);
    togglepin = ! togglepin;
}

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

    //=================
    // Set up the flexible divider/compare
    uint16_t divider = 1;
    uint16_t compare = 0;
    tc_clock_prescaler prescaler = TC_CLOCK_PRESCALER_DIV1;

    if ((freq < 24000000) && (freq > 800))
    {
        divider = 1;
        prescaler = TC_CLOCK_PRESCALER_DIV1;
        compare = 48000000 / freq;
    }
    else if (freq > 400)
    {
        divider = 2;
        prescaler = TC_CLOCK_PRESCALER_DIV2;
        compare = (48000000 / 2) / freq;
    }
    else if (freq > 200)
    {
        divider = 4;
        prescaler = TC_CLOCK_PRESCALER_DIV4;
        compare = (48000000 / 4) / freq;
    }
    else if (freq > 100)
    {
        divider = 8;
        prescaler = TC_CLOCK_PRESCALER_DIV8;
        compare = (48000000 / 8) / freq;
    }
    else if (freq > 50)
    {
        divider = 16;
        prescaler = TC_CLOCK_PRESCALER_DIV16;
        compare = (48000000 / 16) / freq;
    }
    else if (freq > 12)
    {
        divider = 64;
        prescaler = TC_CLOCK_PRESCALER_DIV64;
        compare = (48000000 / 64) / freq;
    }
    else if (freq > 3)
    {
        divider = 256;
        prescaler = TC_CLOCK_PRESCALER_DIV256;
        compare = (48000000 / 256) / freq;
    }
    else if (freq >= 0.75)
    {
        divider = 1024;
        prescaler = TC_CLOCK_PRESCALER_DIV1024;
        compare = (48000000 / 1024) / freq;
    }
    else
    {
        while (1)
            delay (10);
    }

    zerotimer.enable (false);
    zerotimer.configure (prescaler,                   // prescaler
                         TC_COUNTER_SIZE_16BIT,       // bit width of timer/counter
                         TC_WAVE_GENERATION_MATCH_PWM // frequency or PWM mode
    );

    zerotimer.setCompare (0, compare);
    zerotimer.setCallback (true, TC_CALLBACK_CC_CHANNEL0, TimerCallback0);
    zerotimer.enable (true);
    //=================

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

    pinMode (pin::D2, OUTPUT);
    digitalWrite (pin::D2, HIGH);

    pinMode (pin::LED, OUTPUT);
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
