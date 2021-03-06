/** jade main
    @file main.cpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu
*/

#include "MIDI/MIDIGenerator.hpp"
#include "MIDI/MIDIParser.hpp"
#include "MIDIUSB.h"
#include "pin.hpp"

#include <Arduino.h>
#include <Wire.h> // I2C
#include <array>
#include <atomic>

UsbMidiParser midiParser;

constexpr uint8_t NUM_GATE_INPUTS = 4;

enum class GateInputState : uint8_t
{
    Low = 0, ///< Nothing to do
    Rise,    ///< MIDI note on
    High,    ///< Nothing to do
    Fall     ///< MIDI note off
};

std::array<std::atomic<GateInputState>, NUM_GATE_INPUTS> gateInputStates {};

//============== Util ===========================================================================
/** [0, 127]の範囲の2つのuint8を[0, 255]とする.
    @param msb [0, 127]
    @param lsb [0, 127]
    @return constexpr uint8_t [0, 255]
    @note SysExの仕組み上128以上の数は送信できないのでI2Cの1byteを2byteに分割して送信している
*/
constexpr uint8_t mergeBytes (const uint8_t msb, const uint8_t lsb)
{
    return (msb << 4) | (lsb & 0x0F);
}

//=============== MIDI callback =========================================================================
/** SysExのコールバック関数.
    @param sysEx 受信したデータ配列 (SysEx開始, 終了の0xF0と0xF7を除く)
    @param size データ配列の長さ    
*/
void sysExCallback (const uint8_t sysEx[], const uint8_t size)
{
    // jadeに送られるSysExのバイト分割をするので必ず偶数サイズになる
    if (size % 2 != 0)
        return;

    // 最初の2バイトはUniversalSysEx非営利, JadeのSysExデバイスIDとなる
    if (sysEx[0] == SysEx::ManufacturerID::NON_COMMERCIAL && sysEx[1] == SysEx::DeviceID::JADE)
    {
        const uint8_t addr = mergeBytes (sysEx[2], sysEx[3]);
        Wire.beginTransmission (addr);
        for (uint_fast8_t i = 4; i < size; i += 2)
        {
            const uint8_t data = mergeBytes (sysEx[i], sysEx[i + 1]);
            Wire.write (data);
        }

        /* I2C送信結果
        0 : Success
        1 : Data too long
        2 : NACK on transmit of address
        3 : NACK on transmit of data
        4 : Other error
        */
        uint8_t res = Wire.endTransmission();
    }
}

/** Note on callback
    @param note input MIDI note
    @see MIDI::Note
*/
void noteOnCallback (MIDI::Note note)
{
    digitalWrite (pin::GATE_OUT, HIGH);
}

/** Note off callback
    @param note input MIDI note
    @see MIDI::Note
*/
void noteOffCallback (MIDI::Note note)
{
    digitalWrite (pin::GATE_OUT, LOW);
}

/** Control change callback
    @param cc input MIDI control change
    @see MIDI::ControlChange
*/
void controlChangeCallback (MIDI::ControlChange cc)
{
    if (cc.value > 0)
    {
        digitalWrite (pin::LED, LOW);
    }
    else
    {
        digitalWrite (pin::LED, HIGH);
    }
}

std::atomic<uint8_t> MidiClockCounter { 0 };
constexpr uint8_t MaxMidiClockCounter { 6 }; //16分音符間隔のクロック。MIDIクロックが4分音符あたりに24クロックなので、6クロック分が16分音符。

/** SystemRealtime callback    
    @param byte SystemRealtime is 1byte message.
    @see SystemRealtime
    @todo 再生再開時のMidiClockCounterの処遇の検討。クロックの頭が揃わない問題がある。
*/
void systemRealtimeCallback (uint8_t byte)
{
    switch (byte)
    {
        case SystemRealtime::TIMING_CLOCK:
            if (MidiClockCounter == 0)
                digitalWrite (pin::CLOCK_OUT, 1);

            if (MidiClockCounter == 1)
                digitalWrite (pin::CLOCK_OUT, 0);

            MidiClockCounter = MidiClockCounter + 1;
            if (MidiClockCounter == MaxMidiClockCounter)
                MidiClockCounter = 0;

            break;
        case SystemRealtime::START:
            MidiClockCounter = 0;
            break;
        case SystemRealtime::CONTINUE:
            // TODO 再生再開時のMidiClockCounterの処遇の検討。クロックの頭が揃わない問題がある。
            break;
        case SystemRealtime::STOP:
            break;
        default:
            break;
    }
}

//=============== Gate input callback =========================================================================
/** Set current gate state to gateInputStates.
    @param gate Gate input state True: high, False: low
    @param index gateInputStates's index [0, 3]
*/
inline void updateGateInState (bool gate, uint8_t index)
{
    if (gate == false && gateInputStates[index] == GateInputState::Low)
    {
        gateInputStates[index] = GateInputState::Rise;
    }
    else if (gate == true && gateInputStates[index] == GateInputState::High)
    {
        gateInputStates[index] = GateInputState::Fall;
    }
}

void gate1Changed()
{
    updateGateInState (digitalRead (pin::GATE_IN1), 0);
}

void gate2Changed()
{
    updateGateInState (digitalRead (pin::GATE_IN2), 1);
}

void gate3Changed()
{
    updateGateInState (digitalRead (pin::GATE_IN3), 2);
}

void gate4Changed()
{
    updateGateInState (digitalRead (pin::GATE_IN4), 3);
}

//==================================================================================================
void setup()
{
    // MIDI callback
    midiParser.onNoteOn = noteOnCallback;
    midiParser.onNoteOff = noteOffCallback;
    midiParser.onControlChange = controlChangeCallback;
    midiParser.onSysEx = sysExCallback;
    midiParser.onSystemRealtime = systemRealtimeCallback;

    // I2Cピンの内部プルアップ
    pinMode (pin::I2C_SCL, INPUT_PULLUP);
    pinMode (pin::I2C_SDA, INPUT_PULLUP);
    Wire.begin(); // I2C setup

    // GPIO
    pinMode (pin::CLOCK_OUT, OUTPUT);
    digitalWrite (pin::CLOCK_OUT, LOW);
    pinMode (pin::GATE_OUT, OUTPUT);
    digitalWrite (pin::GATE_OUT, LOW);
    pinMode (pin::LED, OUTPUT);
    digitalWrite (pin::LED, HIGH);

    // GATE in
    pinMode (pin::GATE_IN1, INPUT_PULLUP);
    pinMode (pin::GATE_IN2, INPUT_PULLUP);
    pinMode (pin::GATE_IN3, INPUT_PULLUP);
    pinMode (pin::GATE_IN4, INPUT_PULLUP);
    attachInterrupt (pin::GATE_IN1, gate1Changed, CHANGE);
    attachInterrupt (pin::GATE_IN2, gate2Changed, CHANGE);
    attachInterrupt (pin::GATE_IN3, gate3Changed, CHANGE);
    attachInterrupt (pin::GATE_IN4, gate4Changed, CHANGE);

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

    for (uint8_t i = 0; i < gateInputStates.size(); ++i)
    {
        if (gateInputStates[i] == GateInputState::Rise)
        {
            // MIDI note on
            MidiUSB.sendMIDI (makeNoteOn (60, 127, i));
            gateInputStates[i] = GateInputState::High;
        }
        else if (gateInputStates[i] == GateInputState::Fall)
        {
            // MIDI note off
            MidiUSB.sendMIDI (makeNoteOff (60, i));
            gateInputStates[i] = GateInputState::Low;
        }
    }
    MidiUSB.flush();

    delayMicroseconds (50);
}
