#pragma once

#include "MIDIUSB.h"

/*
CIN (Code Index Number)
*/
namespace CIN
{
    constexpr uint8_t SYSCOMMON_2BYTES = 0x02; //2Byte
    constexpr uint8_t SYSCOMMON_3BYTES = 0x03;
    constexpr uint8_t SYSEX_START_OR_CONTINUE = 0x04;
    constexpr uint8_t SYSEX_END_1BYTE = 0x05;  //1Byte
    constexpr uint8_t SYSEX_END_2BYTES = 0x06; //2Byte
    constexpr uint8_t SYSEX_END_3BYTES = 0x07;
    constexpr uint8_t NOTE_OFF = 0x08;
    constexpr uint8_t NOTE_ON = 0x09;
    constexpr uint8_t POLYPHONIC_KEY_PRESSURE = 0x0A;
    constexpr uint8_t CONTROL_CHANGE = 0x0B;
    constexpr uint8_t PROGRAM_CHANGE = 0x0C;   //2Byte
    constexpr uint8_t CHANNEL_PRESSURE = 0x0D; //2Byte
    constexpr uint8_t PITCH_BEND = 0x0E;
    constexpr uint8_t SYS_REALTIME = 0x0F; //1Byte
} // namespace CIN

void parseUSBMIDIPacket(midiEventPacket_t p)
{
}

class USBMIDIParser
{
public:
    void parse(midiEventPacket_t p)
    {
        const uint8_t cin = codeIndexNumber(p);
        switch (cin)
        {
        case CIN::SYSCOMMON_2BYTES:
            break;
        case CIN::SYSCOMMON_3BYTES:
            break;
        case CIN::SYSEX_START_OR_CONTINUE:
            break;
        case CIN::SYSEX_END_1BYTE:
            break;
        case CIN::SYSEX_END_2BYTES:
            break;
        case CIN::SYSEX_END_3BYTES:
            break;
        case CIN::NOTE_OFF:
            break;
        case CIN::NOTE_ON:
            break;
        case CIN::POLYPHONIC_KEY_PRESSURE:
            break;
        case CIN::CONTROL_CHANGE:
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

private:
    uint8_t cableNumber(midiEventPacket_t p)
    {
        // Header上位4bit: CableNumber
        return (p.header >> 4) & 0x0F;
    }

    uint8_t codeIndexNumber(midiEventPacket_t p)
    {
        // Header下位4bit: CodeIndexNumber
        return p.header & 0x0F;
    }
};