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
#include <atomic>

namespace timer
{
constexpr uint32_t CLOCK = 48000000;
constexpr uint32_t PRESCALER = 1;
} // namespace timer

UsbMidiParser midiParser;

std::atomic<bool> gateIn[4];

/** 16bitハードウェアタイマーペリフェラル4と5を組み合わせて32bitタイマーを作らせる.
    @attention 32bitタイマーは偶数のタイマーでしかつくれない。SeeeduinoXIAOはタイマー3, 4, 5しかないのでタイマー4でしか作れないことになる。    
*/
Adafruit_ZeroTimer timer4 = Adafruit_ZeroTimer (4);

/** Hardware timer peripheeral handlers.
    @attention これがないとAdafruit_ZeroTimer.hがうごかない
*/
void TC4_Handler()
{
    Adafruit_ZeroTimer::timerHandler (4); //Clear timer flag
}

/** タイマーのユーザーコールバック関数.
    @todo Single Cycle IOBUSでGPIOトグルの高速化を検討する
*/
std::atomic<bool> logicD2 = { false };
void timer4Callback (void)
{
    digitalWrite (pin::CLOCK_OUT, logicD2);
    logicD2 = ! logicD2;
}

/** Calculate hardware timer compare value from frequency.
    @param frequency Hz
    @return constexpr uint32_t 
*/
constexpr uint32_t getCompare (float frequency)
{
    return static_cast<uint32_t> ((timer::CLOCK / timer::PRESCALER) / frequency);
}

/** [0, 127]の範囲の2つのuint8を[0, 255]とする.
    @param msb [0, 127]
    @param lsb [0, 127]
    @return constexpr uint8_t [0, 255]
    @note SysExの仕組み上128以上の数は送信できないのでI2Cの1byteを2byteに分割して送信している
*/
constexpr uint8_t margeBytes (const uint8_t msb, const uint8_t lsb)
{
    return (msb << 4) | (lsb & 0x0F);
}

/** SysExのコールバック関数.
    @param sysEx 受信したデータ配列 (SysEx開始, 終了の0xF0と0xF7を除く)
    @param size データ配列の長さ
    @todo jade用SysExフォーマットに合わせて書き直す. 先頭2byteが非営利、デバイスIDになる。その後はmonome ii用の分割済データ
*/
void sysExCallback (const uint8_t sysEx[], const uint8_t size)
{
    // jadeに送られるSysExのバイト分割をするので必ず偶数サイズになる
    if (size % 2 == 0)
        return;

    if (sysEx[0] == SysEx::ManufacturerID::NON_COMMERCIAL && sysEx[1] == SysEx::DeviceID::JADE)
    {
        const uint8_t addr = margeBytes (sysEx[2], sysEx[3]);
        Wire.beginTransmission (addr);

        for (uint_fast8_t i = 4; i < size; i += 2)
            Wire.write (margeBytes (sysEx[i], sysEx[i + 1]));

        Wire.endTransmission();
    }
}

void setup()
{
    SerialUSB.begin (115200);

    // Timer
    timer4.enable (false);
    timer4.configure (TC_CLOCK_PRESCALER_DIV1,     // prescaler 1
                      TC_COUNTER_SIZE_32BIT,       // 32bitタイマー
                      TC_WAVE_GENERATION_MATCH_PWM // PWM mode
    );
    uint32_t compare3 = getCompare (5.0f); //Timerの周波数(Hz)
    timer4.setCompare (0, compare3);
    timer4.setCallback (true, TC_CALLBACK_CC_CHANNEL0, timer4Callback);
    timer4.enable (true);

    // MIDI
    midiParser.onNoteOn = [] (MIDI::Note note) {
        digitalWrite (pin::GATE_OUT, HIGH);
    };

    midiParser.onNoteOff = [] (MIDI::Note note) {
        digitalWrite (pin::GATE_OUT, LOW);
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

    // GPIO
    pinMode (pin::CLOCK_OUT, OUTPUT);
    digitalWrite (pin::CLOCK_OUT, HIGH);
    pinMode (pin::GATE_OUT, OUTPUT);
    digitalWrite (pin::GATE_OUT, HIGH);
    pinMode (pin::LED, OUTPUT);
    digitalWrite (pin::LED, HIGH);

    // GATE in
    pinMode (pin::GATE_IN1, INPUT_PULLUP);
    pinMode (pin::GATE_IN2, INPUT_PULLUP);
    pinMode (pin::GATE_IN3, INPUT_PULLUP);
    pinMode (pin::GATE_IN4, INPUT_PULLUP);

    attachInterrupt (
        pin::GATE_IN1,
        [] { gateIn[0] = true; },
        FALLING);
    attachInterrupt (
        pin::GATE_IN2,
        [] { gateIn[1] = true; },
        FALLING);
    attachInterrupt (
        pin::GATE_IN3,
        [] { gateIn[2] = true; },
        FALLING);
    attachInterrupt (
        pin::GATE_IN4,
        [] { gateIn[3] = true; },
        FALLING);
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

    // Gate Input
    for (uint8_t i = 0; i < 4; ++i)
    {
        if (gateIn[i] == true)
        {
            gateIn[i] = false;
            SerialUSB.println (i);
        }
    }
    delayMicroseconds (50);
}
