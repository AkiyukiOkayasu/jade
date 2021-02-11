/** 
    Constants about MIDI
    @file MIDIConstants.hpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu    
*/

#pragma once

#include <cstdint>

/** USB-MIDI CIN (Code Index Number)
    https://pcm1723.hateblo.jp/entry/20150109/1420819489
*/
namespace CIN
{
constexpr uint8_t SYSCOMMON_2BYTES = 0x02; // 2Byte
constexpr uint8_t SYSCOMMON_3BYTES = 0x03;
constexpr uint8_t SYSEX_START_OR_CONTINUE = 0x04;
constexpr uint8_t SYSEX_END_1BYTE = 0x05;  // 1Byte
constexpr uint8_t SYSEX_END_2BYTES = 0x06; // 2Byte
constexpr uint8_t SYSEX_END_3BYTES = 0x07;
constexpr uint8_t NOTE_OFF = 0x08;
constexpr uint8_t NOTE_ON = 0x09;
constexpr uint8_t POLYPHONIC_KEY_PRESSURE = 0x0A;
constexpr uint8_t CONTROL_CHANGE = 0x0B;
constexpr uint8_t PROGRAM_CHANGE = 0x0C;   // 2Byte
constexpr uint8_t CHANNEL_PRESSURE = 0x0D; // 2Byte
constexpr uint8_t PITCH_BEND = 0x0E;
constexpr uint8_t SYS_REALTIME = 0x0F; // 1Byte
} // namespace CIN