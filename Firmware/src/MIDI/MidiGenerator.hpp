/** 
    USB-MIDI event generator.
    @file MidiGenerator.hpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu    
*/

#pragma once

#include "MIDIConstants.hpp"
#include "MIDIUSB.h"

/** 
    Make USB-MIDI note on packet.
    @param noteNumber MIDI note number [0, 127]
    @param velocity MIDI note velocity [0, 127]
    @param channel MIDI channel [0, 15]
    @return midiEventPacket_t 
*/
midiEventPacket_t makeNoteOn (uint8_t noteNumber, uint8_t velocity, uint8_t channel)
{
    midiEventPacket_t p;
    p.header = CIN::NOTE_ON;
    p.byte1 = ChannelMessage::NOTE_ON | channel;
    p.byte2 = noteNumber;
    p.byte3 = velocity;
    return p;
}

/** 
    Make USB-MIDI note off packet.
    @param noteNumber MIDI note number [0, 127]
    @param channel MIDI channel [0, 15]
    @return midiEventPacket_t 
*/
midiEventPacket_t makeNoteOff (uint8_t noteNumber, uint8_t channel)
{
    midiEventPacket_t p;
    p.header = CIN::NOTE_OFF;
    p.byte1 = ChannelMessage::NOTE_OFF | channel;
    p.byte2 = noteNumber;
    p.byte3 = 0;
    return p;
}