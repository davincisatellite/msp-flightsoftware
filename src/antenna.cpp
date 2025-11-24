/*
 * antenna.cpp
 *
 *  Created on: May 04, 2023
 *      Author: bgrochowski
 */

#include "antenna.h"
#include "Console.h"

Antenna::Antenna(DWire &dwire, uint8_t i2c_address_def) : wire(dwire) {
    i2c_address = i2c_address_def;
}

unsigned char Antenna::report_deployment_status() {

    /* Write command to Antenna. */
    wire.beginTransmission(i2c_address);
    wire.write(0b11000011);

    /* Read data from Antenna. */
    uint8_t res = wire.requestFrom(i2c_address, 2);

    /* Construct reply if data arrived. */
    if (res == 2)
    {
        uint8_t lsb = wire.read();
        uint8_t msb = wire.read();
        status.a1s = (msb & 0b10000000) == 0;
        status.a1t = (msb & 0b01000000) != 0;
        status.a1b = (msb & 0b00100000) != 0;
        status.a2s = (msb & 0b00001000) == 0;
        status.a2t = (msb & 0b00000100) != 0;
        status.a2b = (msb & 0b00000010) != 0;
        status.a3s = (lsb & 0b10000000) == 0;
        status.a3t = (lsb & 0b01000000) != 0;
        status.a3b = (lsb & 0b00100000) != 0;
        status.a4s = (lsb & 0b00001000) == 0;
        status.a4t = (lsb & 0b00000100) != 0;
        status.a4b = (lsb & 0b00000010) != 0;
        status.ig = (msb & 0b00000001) != 0;
        status.arm = (lsb & 0b00000001) != 0;
        status.indb = (msb & 0b00010000) != 0;
        status.error = false;
        return SUCCESS;
    }
    /* Signal error if data did not arrive. */
    else
    {
        status.error = true;
        return FAIL;
    }
}

unsigned char Antenna::report_temperature() {
    /* Write command to Antenna. */
    wire.beginTransmission(i2c_address);
    wire.write(0b11000000);

    /* Read data from Antenna. */
    uint8_t res = wire.requestFrom(i2c_address, 2);

    /* Construct reply if data arrived. */
    if (res == 2)
    {
        uint16_t lsb = wire.read();
        uint16_t msb = wire.read();
        uint32_t resp = (msb << 8) + lsb;

        uint16_t Vout = static_cast<uint16_t>((3300 * resp) / 1023);
        temperature.temp = -0.0907 * Vout + 190.15;
        temperature.error = false;
        return SUCCESS;
    }
    /* Signal error if data did not arrive. */
    else
    {
        temperature.error = true;
        return FAIL;
    }
}

unsigned char Antenna::reset() {
    wire.beginTransmission(i2c_address);
    wire.write(0b10101010);
    return wire.endTransmission();
}

// Pings the antenna. Returns 0 if the output of the temperature sensor makes sense, 1 otherwise.

unsigned char Antenna::ping() {
    if (report_temperature()) {
        return FAIL;
    }
    if (temperature.temp > -50.0 && temperature.temp < 150.0 && !temperature.error) {
        return SUCCESS;
    }
    else {
        return FAIL;
    }
}

// Arms the antenna. Returns 0 if antenna is armed successfully, 1 otherwise.

unsigned char Antenna::arm() {
    wire.beginTransmission(i2c_address);
    wire.write(0b10101101);
    wire.endTransmission();
    report_deployment_status();
    return static_cast<unsigned char>(!status.arm);
}

// Disarms the antenna. Returns 0 if antenna is disarmed successfully, 1 otherwise.

unsigned char Antenna::disarm () {
    wire.beginTransmission(i2c_address);
    wire.write(0b10101100);
    wire.endTransmission();
    report_deployment_status();
    return static_cast<unsigned char>(status.arm);
}

// Starts the automated sequential antenna deployment. Returns 0 if the deployment system is active on every antenna, 1 otherwise.

unsigned char Antenna::deploy_sequential() {
    wire.beginTransmission(i2c_address);
    wire.write(0b10100101);
    wire.write(0b00000000);
    wire.endTransmission();
    report_deployment_status();
    return static_cast<unsigned char>(!(status.a1b && status.a2b && status.a3b && status.a4b));
}

unsigned char Antenna::deploy(uint8_t antenna_no, bool override) {
    if (antenna_no < 1 || antenna_no > 4) {
        return FAIL;
    }
    uint8_t reg;
    if (override) {
        reg = 0b10111001 + antenna_no;
    }
    else {
        reg = 0b10100000 + antenna_no;
    }
    wire.beginTransmission(i2c_address);
    wire.write(reg);
    wire.write(0b00000000);
    wire.endTransmission();
    report_deployment_status();
    switch (antenna_no) {
        case 1:
            return static_cast<unsigned char>(!status.a1b);
        case 2:
            return static_cast<unsigned char>(!status.a2b);
        case 3:
            return static_cast<unsigned char>(!status.a3b);
        case 4:
            return static_cast<unsigned char>(!status.a4b);
    }
    return FAIL;
}

// Cancels any ongoing deployment. Returns 0 if there is no active ongoing deployment, 1 otherwise.

unsigned char Antenna::cancel_deploy() {
    wire.beginTransmission(i2c_address);
    wire.write(0b10101001);
    wire.endTransmission();
    report_deployment_status();
    return static_cast<unsigned char>(status.a1b && status.a2b && status.a3b && status.a4b);
}

// Returns true if the temperature has been reported correctly and is above the set threshold, false otherwise.
bool Antenna::temp_above_threshold() {
    if (report_temperature()) {
        return false;
    }
    else {
        return (temperature.temp > ANTENNA_TEMPERATURE_THRESHOLD && !temperature.error);
    }
}


unsigned char Antenna::report_deployment_activation_count() {
    deployment_activation_count.error = false;
    for (uint8_t antenna_no = 1; antenna_no <= 4; antenna_no++) {
        uint8_t reg = 0b10101111 + antenna_no;
        wire.beginTransmission(i2c_address);
        wire.write(reg);
        uint8_t res = wire.requestFrom(i2c_address, 1);
        if (res == 1)
        {
            deployment_activation_count.count[antenna_no-1] = wire.read();
        }
        else {
            deployment_activation_count.error = true;
            return FAIL;
        }
    }
    return SUCCESS;
}


unsigned char Antenna::report_deployment_activation_time() {
    deployment_activation_time.error = false;
    for (uint8_t antenna_no = 1; antenna_no <= 4; antenna_no++) {
        uint8_t reg = 0b10110011 + antenna_no;
        wire.beginTransmission(i2c_address);
        wire.write(reg);
        uint8_t res = wire.requestFrom(i2c_address, 2);
        if (res == 2)
        {
            uint8_t lsb = wire.read();
            uint8_t msb = wire.read();
            deployment_activation_time.time[antenna_no-1] = ((msb << 8) + lsb) / 20;
        }
        else {
            deployment_activation_time.error = true;
            return FAIL;
        }
    }
    return SUCCESS;
}
