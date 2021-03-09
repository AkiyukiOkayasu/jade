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
} // namespace Command
} // namespace ii