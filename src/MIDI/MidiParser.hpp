/**
    USB-MIDI event parser
    @file MIDIParser.hpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu
*/
#pragma once

#include "MIDIConstants.hpp"
#include "MIDIUSB.h"

#include <array>
#include <functional>

/// USB-MIDI event parser
class UsbMidiParser
{
public:
    void parse (midiEventPacket_t p);

    /** Note on callback.
        You can assign a lambda to this callback object to have it called when the MIDI note on is comed.
        @param noteNumber [0, 127]
        @param velocity [0, 127]
        @param midiChannel [0, 15]
    */
    std::function<void (const uint_fast8_t, const uint_fast8_t, const uint_fast8_t)> onNoteOn;

    /** Note off callback.
        You can assign a lambda to this callback object to have it called when the MIDI note off is comed.
        @param noteNumber [0, 127]
        @param velocity [0, 127]
        @param midiChannel [0, 15]
    */
    std::function<void (const uint_fast8_t, const uint_fast8_t, const uint_fast8_t)> onNoteOff;

    /** Control change callback.
        You can assign a lambda to this callback object to have it called when the MIDI control change is comed.
        @param controlNumber [0, 127]
        @param value [0, 127]
        @param midiChannel [0, 15]
    */
    std::function<void (const uint_fast8_t, const uint_fast8_t, const uint_fast8_t)> onControlChange;

    /** SysEx callback.
        You can assign a lambda to this callback object to have it called when the MIDI SysEx is comed.
        @param SysExByteArray
        @param SysExByteArraySize
    */
    std::function<void (const uint8_t[], const uint8_t)> onSysEx;

private:
    constexpr uint8_t midiCh (const uint8_t statusByte) const
    {
        // MIDI StatusByte下位4bit: MIDI channel
        return statusByte & 0x0F;
    }

    /** 
        USB-MIDIイベントパケットのbyte0（midiEventPacket_tのheader)からCable Numberを取得する
        https://pcm1723.hateblo.jp/entry/20150109/1420819489
        @param header midiEventPacket_t の header
        @return uint8_t USB-MIDI Cable Number
        @note USB-MIDIデバイスが複数使用されているときのインデックスとして利用する
    */
    constexpr uint8_t cableNumber (const uint8_t header) const
    {
        // Header上位4bit: CableNumber
        return (header >> 4) & 0x0F;
    }

    /** 
        USB-MIDIイベントパケットのbyte0（midiEventPacket_tのheader)からCode Index Numberを取得する
        https://pcm1723.hateblo.jp/entry/20150109/1420819489
        @param header  midiEventPacket_t の header
        @return uint8_t USB-MIDI 
        @note USB-MIDIメッセージの種別を表す
    */
    constexpr uint8_t codeIndexNumber (const uint8_t header) const
    {
        // Header下位4bit: CodeIndexNumber
        return header & 0x0F;
    }

    void pushSysEx (uint8_t d)
    {
        sysExData[sysExIndex] = d;
        sysExIndex++;
    }

    static constexpr uint8_t MAX_SYSEX_LENGTH = 32;
    bool receivingSysEx = false;
    uint8_t sysExIndex = 0;
    std::array<uint_fast8_t, MAX_SYSEX_LENGTH> sysExData;
};