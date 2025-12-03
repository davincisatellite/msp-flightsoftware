#ifndef DWIRE_TEST_EXAMPLES_H
#define DWIRE_TEST_EXAMPLES_H

#include <cstdint>

uint8_t try_simple(uint8_t i2c_address);
uint8_t try_with_begin(uint8_t i2c_address);
uint8_t try_without_beginTransmission(uint8_t i2c_address);
uint8_t try_with_endTransmission_begin(uint8_t i2c_address);
uint8_t try_with_endTransmission(uint8_t i2c_address);
uint8_t try_with_endTransmission_just_begin(uint8_t i2c_address);

uint8_t try_reset_simple(uint8_t i2c_address);
uint8_t try_reset_after_begin(uint8_t i2c_address);
uint8_t try_reset_after_begin_with_again_begin(uint8_t i2c_address);
uint8_t try_reset_after_beginTransmission(uint8_t i2c_address);
uint8_t try_reset_after_beginTransmission_again(uint8_t i2c_address);
uint8_t try_reset_after_beginTransmission_again_plus_end(uint8_t i2c_address);

uint8_t try_initializing_DWire(uint8_t i2c_address);
uint8_t try_initializing_DWire_End_start_again(uint8_t i2c_address);
uint8_t try_initializing_DWire_without_end(uint8_t i2c_address);

int mainnn();

#endif // DWIRE_TEST_EXAMPLES_H