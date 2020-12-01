#pragma once

#include <array>
#include "MIDIUSB.h"
#include "pin.hpp"

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

class UsbMidiHandler
{
public:
    virtual void noteOnCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch) = 0;
    virtual void noteOffCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch) = 0;
    virtual void ccCallback(const uint_fast8_t controlNumber, const uint_fast8_t value, const uint_fast8_t ch) = 0;
    virtual void sysExCallback(const uint_fast8_t sysEx[], const uint32_t size) = 0;
};

class UsbMidiParser : public UsbMidiHandler
{
public:
    void parse(midiEventPacket_t p);

    /*
     Callback functions
     */
    void noteOnCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch) override;
    void noteOffCallback(const uint_fast8_t note, const uint_fast8_t velocity, const uint_fast8_t ch) override;
    void ccCallback(const uint_fast8_t controlNumber, const uint_fast8_t value, const uint_fast8_t ch) override;
    void sysExCallback(const uint_fast8_t sysEx[], const uint32_t size) override;

private:
    constexpr uint_fast8_t midiCh(const uint_fast8_t statusByte)
    {
        // MIDI StatusByte下位4bit: MIDI channel
        return statusByte & 0x0F;
    }

    uint8_t cableNumber(const uint_fast8_t header) const
    {
        // Header上位4bit: CableNumber
        return (header >> 4) & 0x0F;
    }

    uint8_t codeIndexNumber(const uint_fast8_t header) const
    {
        // Header下位4bit: CodeIndexNumber
        return header & 0x0F;
    }

    void pushSysEx(uint_fast8_t d)
    {
        sysExData[sysExIndex] = d;
        sysExIndex++;
    }
    /*
    void clearSysExFlag()
    {
        receivingSysEx = false;
    }
    */

    uint8_t margeBytes(const uint8_t msb, const uint8_t lsb) const
    {
        return (msb << 4) | (lsb & 0x0F);
    }

    static constexpr uint8_t MAX_SYSEX_LENGTH = 32;
    bool receivingSysEx = false;
    uint_fast8_t sysExIndex = 0;
    std::array<uint_fast8_t, MAX_SYSEX_LENGTH> sysExData;
};