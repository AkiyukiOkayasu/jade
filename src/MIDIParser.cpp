#include "MIDIParser.hpp"
#include <Wire.h>
#include "constants.hpp"

void UsbMidiParser::parse(midiEventPacket_t p)
{
    const uint_fast8_t cin = codeIndexNumber(p.header);
    const uint_fast8_t cn = cableNumber(p.header);
    const uint_fast8_t statusByte = p.byte1;
    const uint_fast8_t channel = midiCh(statusByte);
    static uint_fast8_t tmp[3];

    switch (cin)
    {
    case CIN::SYSCOMMON_2BYTES:
        break;
    case CIN::SYSCOMMON_3BYTES:
        break;
    case CIN::SYSEX_START_OR_CONTINUE:
        if (p.byte1 == 0xF0)
        {
            receivingSysEx = true;
            sysExIndex = 0;
            pushSysEx(p.byte2);
            pushSysEx(p.byte3);
        }
        else
        {
            pushSysEx(p.byte1);
            pushSysEx(p.byte2);
            pushSysEx(p.byte3);
        }
        break;
    case CIN::SYSEX_END_1BYTE:
        if (p.byte1 == 0xF7)
        {
            receivingSysEx = false;
            sysExCallback(sysExData.data(), sysExIndex);
        }
        else
        {
            SerialUSB.println("Invalid sysEx");
        }
        break;
    case CIN::SYSEX_END_2BYTES:
        if (p.byte2 == 0xF7)
        {
            pushSysEx(p.byte1);
            receivingSysEx = false;
            sysExCallback(sysExData.data(), sysExIndex);
        }
        else
        {
            SerialUSB.println("Invalid sysEx");
        }
        break;
    case CIN::SYSEX_END_3BYTES:
        if (p.byte3 == 0xF7)
        {
            pushSysEx(p.byte1);
            pushSysEx(p.byte2);
            receivingSysEx = false;
            sysExCallback(sysExData.data(), sysExIndex);
        }
        else
        {
            SerialUSB.println("Invalid sysEx");
        }
        break;
    case CIN::NOTE_OFF:
        noteOffCallback(p.byte2, p.byte3, channel);
        break;
    case CIN::NOTE_ON:
        if (p.byte3 == 0) // velocity 0 = noteOFF
        {
            noteOffCallback(p.byte2, p.byte3, channel);
            break;
        }
        noteOnCallback(p.byte2, p.byte3, channel);
        break;
    case CIN::POLYPHONIC_KEY_PRESSURE:
        break;
    case CIN::CONTROL_CHANGE:
        ccCallback(p.byte2, p.byte3, channel);
        break;
    case CIN::PROGRAM_CHANGE:
        break;
    case CIN::CHANNEL_PRESSURE:
        break;
    case CIN::PITCH_BEND:
        break;
    case CIN::SYS_REALTIME:
        break;
    default:
        break;
    }
}

void UsbMidiParser::noteOnCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch)
{
    digitalWrite(LED, LOW);
}

void UsbMidiParser::noteOffCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch)
{
    digitalWrite(LED, HIGH);
}

void UsbMidiParser::ccCallback(const uint_fast8_t controlNumber, const uint_fast8_t value, const uint_fast8_t ch)
{
    if (value > 0)
    {
        digitalWrite(LED, LOW);
    }
    else
    {
        digitalWrite(LED, HIGH);
    }
}

void UsbMidiParser::sysExCallback(const uint_fast8_t sysEx[], const uint32_t size)
{
    for (uint_fast8_t i = 0; i < size; ++i)
    {
        SerialUSB.print(sysEx[i]);
        SerialUSB.print(",");
    }
    Wire.beginTransmission(sysEx[0]);
    for (uint_fast8_t i = 1; i < size; ++i)
    {
        Wire.write(sysEx[i]);
    }
    Wire.endTransmission();
    SerialUSB.println("");
    delay(1);
}