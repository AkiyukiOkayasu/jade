/** 
    Constants about Seeeduino XAIO GPIO.
    @file pin.hpp
    @author Akiyuki Okayasu (akiyuki.okayasu@gmail.com)
    @copyright Copyright (c) 2021 - Akiyuki Okayasu    
*/

#pragma once

namespace pin
{
constexpr uint8_t LED = 13;
constexpr uint8_t I2C_SDA = 4;
constexpr uint8_t I2C_SCL = 5;
constexpr uint8_t CLOCK_OUT = 3; ///< D3
constexpr uint8_t GATE_OUT = 2;  ///< D2
constexpr uint8_t GATE_IN1 = 6;  ///< D6
constexpr uint8_t GATE_IN2 = 8;  ///< D8
constexpr uint8_t GATE_IN3 = 9;  ///< D9
constexpr uint8_t GATE_IN4 = 10; ///< D10
} // namespace pin