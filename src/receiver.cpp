/*
 * receiver.cpp
 *
 *  Created on: Jun 23, 2023
 *      Author: bgrochowski
 */

#include "receiver.h"
#include "Console.h"

Receiver::Receiver(DWire &dwire, uint8_t i2c_address_def) : wire(dwire) {
    i2c_address = i2c_address_def;
}

//Performs a reset of the I2C watchdog without performing any other operation.
unsigned char Receiver::reset_watchdog() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0xCC);
    return wire.endTransmission();
}

//Performs a software reset of the transmitter processor.
unsigned char Receiver::reset_software() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0xAA);
    wire.endTransmission();
    report_uptime();
    if (uptime < 3) {
        return SUCCESS;
    }
    else {
        return FAIL;
    }
}

//Power cycles the full board (transmitter and receiver will be both reset).
unsigned char Receiver::reset_hardware() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0xAB);
    wire.endTransmission();
    report_uptime();
    if (uptime < 3) {
        return SUCCESS;
    }
    else {
        return FAIL;
    }
}

unsigned char Receiver::get_number_of_frames() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0x21);
    wire.endTransmission();
    delay_ms(30);
    uint8_t res = wire.requestFrom(i2c_address, 2);
    if (res == 2)
    {
        uint8_t lsb = wire.read();
        uint8_t msb = wire.read();
        frames.number = (msb << 8) + lsb;
        frames.error = false;
        return SUCCESS;
    }
    else
    {
        frames.error = true;
        return FAIL;
    }
}

unsigned char Receiver::get_frame_from_buffer() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0x22);
    wire.endTransmission();
    delay_ms(30);
    uint8_t res = wire.requestFrom(i2c_address, 2);
    if (res == 2)
    {
        uint8_t lsb = wire.read();
        uint8_t msb = wire.read();
        uint16_t frame_size = (msb << 8) + lsb;
        frame.size = frame_size;
        res = wire.requestFrom(i2c_address, 4 + frame_size);
        if (res == 4 + frame_size)
        {
            lsb = wire.read();
            msb = wire.read();
            frame.doppler_f = (msb << 8) + lsb;
            lsb = wire.read();
            msb = wire.read();
            frame.rssi = (msb << 8) + lsb;
            for (int i = 0; i<frame_size; i++) {
                frame.frame[i] = wire.read();
            }
            for (int i = frame_size; i<200; i++) {
                frame.frame[i] = 0;
            }
            frame.error = false;
            return SUCCESS;
        }
        else
        {
            frame.error = true;
            return FAIL;
        }
    }
    else
    {
        return FAIL;
    }
}

unsigned char Receiver::remove_frame_from_buffer() {
    /* Write command to Receiver. */
    get_number_of_frames();
    uint16_t old_frames_num = frames.number;
    if (old_frames_num == 0)
        return FAIL;
    wire.beginTransmission(i2c_address);
    wire.write(0x24);
    wire.endTransmission();
    get_number_of_frames();
    if (old_frames_num - frames.number == 1)
        return SUCCESS;
    else
        return FAIL;
}

unsigned char Receiver::measure_telemetry() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0x1A);
    wire.endTransmission();
    delay_ms(30);

    /* Read data from Receiver. */
    uint8_t res = wire.requestFrom(i2c_address, 18);
    if (res == 18)
    {
        uint16_t raw_doppler_offset = wire.read();
        raw_doppler_offset += (wire.read() << 8);
        uint16_t raw_signal_strength = wire.read();
        raw_signal_strength += (wire.read() << 8);
        uint16_t raw_power_bus_voltage = wire.read();
        raw_power_bus_voltage += (wire.read() << 8);
        uint16_t raw_total_current = wire.read();
        raw_total_current += (wire.read() << 8);
        uint16_t raw_tx_current = wire.read();
        raw_tx_current += (wire.read() << 8);
        uint16_t raw_rx_current = wire.read();
        raw_rx_current += (wire.read() << 8);
        uint16_t raw_poweramp_current = wire.read();
        raw_poweramp_current += (wire.read() << 8);
        uint16_t raw_poweramp_temp = wire.read();
        raw_poweramp_temp += (wire.read() << 8);
        uint16_t raw_oscillator_temp = wire.read();
        raw_oscillator_temp += (wire.read() << 8);
        telemetry.doppler_offset = (raw_doppler_offset * 13.352) - 22300;
        telemetry.signal_strength = (raw_signal_strength * 0.03) - 152;
        telemetry.power_bus_voltage = raw_power_bus_voltage * 0.00488;
        telemetry.total_current = raw_total_current * 0.16643964;
        telemetry.tx_current = raw_tx_current * 0.16643964;
        telemetry.rx_current = raw_rx_current * 0.16643964;
        telemetry.poweramp_current = raw_poweramp_current * 0.16643964;
        telemetry.poweramp_temp = (raw_poweramp_temp * -0.07669) + 195.6037;
        telemetry.oscillator_temp = (raw_oscillator_temp * -0.07669) + 195.6037;
        telemetry.error = false;
        return SUCCESS;
    }
    else
    {
        telemetry.error = true;
        return FAIL;
    }
}

unsigned char Receiver::report_uptime() {
    /* Write command to Receiver. */
    wire.beginTransmission(i2c_address);
    wire.write(0x40);
    wire.endTransmission();
    delay_ms(30);

    /* Read data from Receiver. */
    uint8_t res = wire.requestFrom(i2c_address, 4);
    if (res == 4)
    {
        uint32_t resp = 0;
        for (int i = 0; i < 4; i++) {
            resp = (resp >> 8); // Shift existing bits to the right
            resp = resp + (wire.read() << 24); // Add the new byte shifted to the leftmost position
        }
        uptime = resp;
        return SUCCESS;
    }
    else {
        return FAIL;
    }
}

unsigned char Receiver::ping() {
    if (report_uptime() != SUCCESS) {
        return FAIL;
    }
    // Check if the two most-significant bits are zero
    if ((uptime & 0xC0000000) == 0) {
        return SUCCESS;
    } else {
        return FAIL;
    }
}