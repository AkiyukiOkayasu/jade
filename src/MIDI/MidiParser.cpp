#include "MidiParser.hpp"

void UsbMidiParser::parse (midiEventPacket_t p)
{
    const uint_fast8_t cin = codeIndexNumber (p.header);
    const uint_fast8_t cn = cableNumber (p.header);
    const uint_fast8_t statusByte = p.byte1;
    const uint_fast8_t channel = midiCh (statusByte);

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
                pushSysEx (p.byte2);
                pushSysEx (p.byte3);
            }
            else
            {
                pushSysEx (p.byte1);
                pushSysEx (p.byte2);
                pushSysEx (p.byte3);
            }
            break;
        case CIN::SYSEX_END_1BYTE:
            if (p.byte1 == 0xF7)
            {
                receivingSysEx = false;
                if (onSysEx != nullptr)
                    onSysEx (sysExData.data(), sysExIndex);
            }
            else
            {
                SerialUSB.println ("Invalid sysEx");
            }
            break;
        case CIN::SYSEX_END_2BYTES:
            if (p.byte2 == 0xF7)
            {
                pushSysEx (p.byte1);
                receivingSysEx = false;
                if (onSysEx != nullptr)
                    onSysEx (sysExData.data(), sysExIndex);
            }
            break;
        case CIN::SYSEX_END_3BYTES:
            if (p.byte3 == 0xF7)
            {
                pushSysEx (p.byte1);
                pushSysEx (p.byte2);
                receivingSysEx = false;
                if (onSysEx != nullptr)
                    onSysEx (sysExData.data(), sysExIndex);
            }
            break;
        case CIN::NOTE_OFF:
            if (onNoteOff != nullptr)
                onNoteOff (p.byte2, p.byte3, channel);

            break;
        case CIN::NOTE_ON:
            if (p.byte3 == 0) // velocity 0 = noteOFF
            {
                if (onNoteOff != nullptr)
                    onNoteOff (p.byte2, p.byte3, channel);

                break;
            }

            if (onNoteOn != nullptr)
                onNoteOn (p.byte2, p.byte3, channel);

            break;
        case CIN::POLYPHONIC_KEY_PRESSURE:
            break;
        case CIN::CONTROL_CHANGE:
            if (onControlChange != nullptr)
                onControlChange (p.byte2, p.byte3, channel);
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

/*
void UsbMidiParser::noteOnCallback (const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch)
{
    digitalWrite (pin::LED, LOW);
}

void UsbMidiParser::noteOffCallback (const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch)
{
    digitalWrite (pin::LED, HIGH);
}

void UsbMidiParser::ccCallback (const uint_fast8_t controlNumber, const uint_fast8_t value, const uint_fast8_t ch)
{
    if (value > 0)
    {
        digitalWrite (pin::LED, LOW);
    }
    else
    {
        digitalWrite (pin::LED, HIGH);
    }
}

void UsbMidiParser::sysExCallback (const uint_fast8_t sysEx[], const uint32_t size)
{
    const uint8_t addr = margeBytes (sysEx[0], sysEx[1]);
    Wire.beginTransmission (addr);

    for (uint_fast8_t i = 2; i < size; i += 2)
    {
        Wire.write (margeBytes (sysEx[i], sysEx[i + 1]));
    }
    Wire.endTransmission();
}
*/