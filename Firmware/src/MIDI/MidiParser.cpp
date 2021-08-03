#include "MidiParser.hpp"

void UsbMidiParser::parse (midiEventPacket_t p)
{
    const uint8_t cin = codeIndexNumber (p.header);
    const uint8_t cn = cableNumber (p.header);
    const uint8_t statusByte = p.byte1;
    const uint8_t channel = midiCh (statusByte);

    switch (cin)
    {
        case CIN::SYSCOMMON_2BYTES:
            break;
        case CIN::SYSCOMMON_3BYTES:
            break;
        case CIN::SYSEX_START_OR_CONTINUE:
            if (p.byte1 == SysEx::BEGIN)
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
            if (p.byte1 == SysEx::END)
            {
                receivingSysEx = false;
                if (onSysEx)
                {
                    onSysEx (sysExData.data(), sysExIndex);
                }
            }
            break;
        case CIN::SYSEX_END_2BYTES:
            if (p.byte2 == SysEx::END)
            {
                pushSysEx (p.byte1);
                receivingSysEx = false;
                if (onSysEx)
                {
                    onSysEx (sysExData.data(), sysExIndex);
                }
            }
            break;
        case CIN::SYSEX_END_3BYTES:
            if (p.byte3 == SysEx::END)
            {
                pushSysEx (p.byte1);
                pushSysEx (p.byte2);
                receivingSysEx = false;
                if (onSysEx)
                {
                    onSysEx (sysExData.data(), sysExIndex);
                }
            }
            break;
        case CIN::NOTE_OFF:
            if (onNoteOff)
            {
                MIDI::Note note = { p.byte2, p.byte3, channel };
                onNoteOff (note);
            }

            break;
        case CIN::NOTE_ON:
        {
            MIDI::Note note = { p.byte2, p.byte3, channel };
            if (p.byte3 == 0) // velocity 0 = noteOFF
            {
                if (onNoteOff)
                {
                    onNoteOff (note);
                }

                break;
            }

            if (onNoteOn)
            {
                onNoteOn (note);
            }

            break;
        }
        case CIN::POLYPHONIC_KEY_PRESSURE:
            break;
        case CIN::CONTROL_CHANGE:
            if (onControlChange)
            {
                MIDI::ControlChange cc = { p.byte2, p.byte3, channel };
                onControlChange (cc);
            }
            break;
        case CIN::PROGRAM_CHANGE:
            break;
        case CIN::CHANNEL_PRESSURE:
            break;
        case CIN::PITCH_BEND:
            break;
        case CIN::SYS_REALTIME:
            if (onSystemRealtime)
            {
                onSystemRealtime (p.byte1);
            }
            break;
        default:
            break;
    }
}