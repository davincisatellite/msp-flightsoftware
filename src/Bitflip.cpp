#include "Bitflip.hpp"

Bitflip::Bitflip(DWire &i2c, int i2c_address) : wire(i2c) {
    this->i2c_address = i2c_address;
}

Bitflip::Error Bitflip::getStatus(Bitflip::Status &status) {
    wire.beginTransmission(i2c_address); // Begin transmission to address of object
    wire.write(Bitflip::COMMAND_STATUS); // Send status command
    uint8_t n = wire.requestFrom(i2c_address, 1); // Request one byte to read

    if (n == 1) {
        status = (Bitflip::Status) wire.read();
        wire.endTransmission();
        return Bitflip::ERROR_NO_ERROR;
    } else {
        status = Bitflip::STATUS_UNDEFINED;
        wire.endTransmission();
        return Bitflip::ERROR_I2C_READ_ERROR;
    }
}