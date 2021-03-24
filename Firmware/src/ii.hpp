/**
    monome ii constants.
    @file ii.hpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu
*/
#pragma once

#include <cstdint>

/** Constants for ii.
    ii format
    [address byte][command byte][data bytes(optional)]
*/
namespace ii
{
/// Address byte.
namespace Address
{
    constexpr uint8_t crow = 0x01;        ///< crow
    constexpr uint8_t JustFriends = 0x70; ///< Just Friends
    constexpr uint8_t Ansible = 0x20;     ///< Ansible
    constexpr uint8_t ER301_1 = 0x31;     ///< ER-301 1
    constexpr uint8_t ER301_2 = 0x32;     ///< ER-301 2
    constexpr uint8_t ER301_3 = 0x33;     ///< ER-301 3
} // namespace Address

/// Command byte.
namespace Command
{
    /** ii command for JustFriends.
        https://github.com/monome/crow/blob/main/lua/ii/jf.lua
    */
    namespace JustFriends
    {
        constexpr uint8_t TR = 1;
        constexpr uint8_t RMODE = 2;
        constexpr uint8_t RUN = 3;
        constexpr uint8_t SHIFT = 4;
        constexpr uint8_t VTR = 5;
        constexpr uint8_t MODE = 6;
        constexpr uint8_t TICK = 7;
        constexpr uint8_t VOX = 8;
        constexpr uint8_t NOTE = 9;
        constexpr uint8_t GOD = 10;
        constexpr uint8_t TUNE = 11;
        constexpr uint8_t QT = 12;
    } // namespace JustFriends

    /** ii command for ER-301.
        https://github.com/monome/crow/blob/main/lua/ii/er301.lua
    */
    namespace ER301
    {
        constexpr uint8_t TR = 0x00;       ///< Set TR port to state
        constexpr uint8_t TR_TOG = 0x01;   ///< Toggle TR
        constexpr uint8_t TR_PULSE = 0x05; ///< Pulse TR port using TO.TR.TIME/S/M as an interval
        constexpr uint8_t TR_TIME = 0x02;  ///< Time for TR.PULSE port in ms
        constexpr uint8_t TR_POL = 0x06;   ///< Polarity for TO.TR.PULSE port set to rising (0/1)
        constexpr uint8_t CV = 0x10;       ///< Set port CV to volts (bipolar), following SLEW time
        constexpr uint8_t CV_SLEW = 0x12;  ///< Set CV slew time in ms
        constexpr uint8_t CV_SET = 0x11;   ///< Set CV port to volts(bipolar), ignoring SLEW time
        constexpr uint8_t CV_OFF = 0x15;   ///< CV port offset, volts added at final stage
    }                                      // namespace ER301
} // namespace Command
} // namespace ii