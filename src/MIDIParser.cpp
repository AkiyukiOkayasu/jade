#include "MIDIParser.hpp"

void UsbMidiParser::parse(midiEventPacket_t p)
{
    const uint_fast8_t cin = codeIndexNumber(p.header);
    const uint_fast8_t cn = cableNumber(p.header);
    const uint_fast8_t statusByte = p.byte1;
    const uint_fast8_t channel = midiCh(statusByte);

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

void UsbMidiParser::ccCallback(const uint_fast8_t cc, const uint_fast8_t value, const uint_fast8_t ch)
{
}

void UsbMidiParser::sysExCallback(const uint_fast8_t sysEx[], const uint32_t size)
{
}