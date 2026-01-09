/*
 * transmitter.cpp
 *
 *  Created on: May 06, 2023
 *      Author: bgrochowski, dptrias
 */

#include "transmitter.h"

Transmitter::Transmitter(DWire &dwire, uint8_t i2c_address_def) : wire(dwire) {
    i2c_address = i2c_address_def;
}

unsigned char Transmitter::report_state() {

    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x41);
    wire.endTransmission();

    /* Wait for transmitter to process command. */
    delay_ms(30);

    /* Read data from Transmitter. */
    uint8_t res = wire.requestFrom(i2c_address, 1);

    /* Construct reply if data arrived. */
    if (res == 1)
    {
        uint8_t resp = wire.read();
        state.on_idle = (resp & 0b00000001) != 0;
        state.beacon_active = (resp & 0b00000010) != 0;
        uint8_t tx_bitrate_resp = (resp & (0b00001100)) >> 2;
        switch(tx_bitrate_resp) {
            case 0:
                state.tx_bitrate = 1200;
                break;
            case 1:
                state.tx_bitrate = 2400;
                break;
            case 2:
                state.tx_bitrate = 4800;
                break;
            case 3:
                state.tx_bitrate = 9600;
                break;
            default:
                state.tx_bitrate = 0;
                state.error = true;
                return FAIL;
        }
        state.error = false;
        return SUCCESS;
    }
    /* Signal error if data did not arrive. */
    else
    {
        return FAIL;
    }
}

unsigned char Transmitter::report_uptime() {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x40);
    wire.endTransmission();

    delay_ms(30);

    /* Read data from Transmitter. */
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

unsigned char Transmitter::set_bitrate(uint16_t bitrate) {
    uint8_t param;
    switch(bitrate) {
        case 1200:
            param = 0b00000001;
            break;
        case 2400:
            param = 0b00000010;
            break;
        case 4800:
            param = 0b00000100;
            break;
        case 9600:
            param = 0b00001000;
            break;
        default:
            return 1;
    }
    wire.beginTransmission(i2c_address);
    wire.write(0x28);
    wire.write(param);
    return wire.endTransmission();
}

//Provides the telemetry channels that were sampled during the last frame transmission
unsigned char Transmitter::report_last_telemetry() {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x26);
    wire.endTransmission();

    delay_ms(30);

    /* Read data from Transmitter. */
    uint8_t res = wire.requestFrom(i2c_address, 18);
    if (res == 18)
    {
        uint16_t raw_rf_reflected_power = wire.read();
        raw_rf_reflected_power += (wire.read() << 8);
        uint16_t raw_rf_forward_power = wire.read();
        raw_rf_forward_power += (wire.read() << 8);
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
        last_telemetry.rf_reflected_power = raw_rf_reflected_power * raw_rf_reflected_power * 0.00005887;
        last_telemetry.rf_forward_power = raw_rf_forward_power * raw_rf_forward_power * 0.00005887;
        last_telemetry.power_bus_voltage = raw_power_bus_voltage * 0.00488;
        last_telemetry.total_current = raw_total_current * 0.16643964;
        last_telemetry.tx_current = raw_tx_current * 0.16643964;
        last_telemetry.rx_current = raw_rx_current * 0.16643964;
        last_telemetry.poweramp_current = raw_poweramp_current * 0.16643964;
        last_telemetry.poweramp_temp = (raw_poweramp_temp * -0.07669) + 195.6037;
        last_telemetry.oscillator_temp = (raw_oscillator_temp * -0.07669) + 195.6037;
        last_telemetry.error = false;
        return SUCCESS;
    }
    else
    {
        last_telemetry.error = true;
        return FAIL;
    }
}

unsigned char Transmitter::measure_telemetry() {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x25);
    wire.endTransmission();

    delay_ms(30);

    /* Read data from Transmitter. */
    uint8_t res = wire.requestFrom(i2c_address, 18);
    if (res == 18)
    {
        uint16_t raw_rf_reflected_power = wire.read();
        raw_rf_reflected_power += (wire.read() << 8);
        uint16_t raw_rf_forward_power = wire.read();
        raw_rf_forward_power += (wire.read() << 8);
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
        measured_telemetry.rf_reflected_power = raw_rf_reflected_power * raw_rf_reflected_power * 0.00005887;
        measured_telemetry.rf_forward_power = raw_rf_forward_power * raw_rf_forward_power * 0.00005887;
        measured_telemetry.power_bus_voltage = raw_power_bus_voltage * 0.00488;
        measured_telemetry.total_current = raw_total_current * 0.16643964;
        measured_telemetry.tx_current = raw_tx_current * 0.16643964;
        measured_telemetry.rx_current = raw_rx_current * 0.16643964;
        measured_telemetry.poweramp_current = raw_poweramp_current * 0.16643964;
        measured_telemetry.poweramp_temp = (raw_poweramp_temp * -0.07669) + 195.6037;
        measured_telemetry.oscillator_temp = (raw_oscillator_temp * -0.07669) + 195.6037;
        measured_telemetry.error = false;
        return SUCCESS;
    }
    else
    {
        measured_telemetry.error = true;
        return FAIL;
    }
}

unsigned char Transmitter::set_idle_mode(bool on_idle) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x24);
    wire.write(static_cast<uint8_t>(on_idle));
    return wire.endTransmission();
}

//Sets the default AX.25 TO callsign in the transmitter. This callsign is included in AX.25 frames and AX.25 beacon transmissions that are transmitted using the default callsigns
unsigned char Transmitter::set_default_to_callsign(char to_callsign[6], uint8_t num) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x22);
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(to_callsign[i]));
    }
    wire.write(num);
    return wire.endTransmission();
}

//Sets the default AX.25 FROM callsign in the transmitter. This callsign is included in AX.25 frames and AX.25 beacon transmissions that are transmitted using the default callsigns.
unsigned char Transmitter::set_default_from_callsign(char from_callsign[6], uint8_t num) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x23);
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(from_callsign[i]));
    }
    wire.write(num);
    return wire.endTransmission();
}

//Clears any beacon that is currently set in the transceiver. If a beacon transmission is currently in progress, this transmission will be completed.
unsigned char Transmitter::clear_beacon() {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x1F);
    return wire.endTransmission();
}

//Performs a reset of the I2C watchdog without performing any other operation.
unsigned char Transmitter::reset_watchdog() {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0xCC);
    return wire.endTransmission();
}

//Performs a software reset of the transmitter processor.
unsigned char Transmitter::reset_software() {
    /* Write command to Transmitter. */
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
unsigned char Transmitter::reset_hardware() {
    /* Write command to Transmitter. */
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

//Adds a frame to the frame buffer of the transmitter. The frame will contain the default callsigns as they are set in the controller at the time this command is received.
//This command will disable any beacon that is currently being transmitted by the transceiver.
unsigned char Transmitter::send_frame(uint8_t* frame, uint8_t size) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x10);
    for (int i=0; i<size; i++) {
        wire.write(static_cast<uint8_t>(frame[i]));
    }
    wire.endTransmission();
    delay_ms(30);

    /* Read data from Transmitter. */
    uint8_t res = wire.requestFrom(i2c_address, 1);
    if (res == 1)
    {
        uint8_t resp = wire.read();
        buffer.free_slots = resp;
        if (resp == 0xFF) {
            //Frame has not been added.
            buffer.error = true;
            return FAIL;
        }
        else {
            buffer.error = false;
            return SUCCESS;
        }
    }
    /* Signal error if data did not arrive. */
    else
    {
        buffer.error = true;
        return FAIL;
    }
}

unsigned char Transmitter::send_frame_override_cs(uint8_t* frame, uint8_t size, char to_callsign[6], uint8_t to_num, char from_callsign[6], uint8_t from_num) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x11);

    //Override TO callsign
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(to_callsign[i]));
    }
    wire.write(to_num);

    //Override FROM callsign
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(from_callsign[i]));
    }
    wire.write(from_num);

    //Send frame
    for (int i=0; i<size; i++) {
        wire.write(static_cast<uint8_t>(frame[i]));
    }

    wire.endTransmission();

    delay_ms(30);

    /* Read data from Transmitter. */
    uint8_t res = wire.requestFrom(i2c_address, 1);
    if (res == 1)
    {
        uint8_t resp = wire.read();
        buffer.free_slots = resp;
        if (resp == 0xFF) {
            //Buffer is completely full, frame has not been added.
            buffer.error = true;
            return FAIL;
        }
        else {
            buffer.error = false;
            return SUCCESS;
        }
    }
    /* Signal error if data did not arrive. */
    else
    {
        buffer.error = true;
        return FAIL;
    }
}

unsigned char Transmitter::set_beacon(uint8_t* frame, uint8_t size, uint16_t time_interval) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x14);
    // Send the time interval, little endian.
    wire.write(static_cast<uint8_t>(time_interval & 0xFF));
    wire.write(static_cast<uint8_t>((time_interval >> 8) & 0xFF));
    // Send the beacon frame
    for (int i=0; i<size; i++) {
        wire.write(static_cast<uint8_t>(frame[i]));
    }
    return wire.endTransmission();
}

unsigned char Transmitter::set_beacon_override_cs(uint8_t* frame, uint8_t size, uint16_t time_interval, char to_callsign[6], uint8_t to_num, char from_callsign[6], uint8_t from_num) {
    wire.beginTransmission(i2c_address);
    wire.write(0x15);

    // Send the time interval, little endian.
    wire.write(static_cast<uint8_t>(time_interval & 0xFF));
    wire.write(static_cast<uint8_t>((time_interval >> 8) & 0xFF));

    //Override TO callsign
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(to_callsign[i]));
    }
    wire.write(to_num);

    //Override FROM callsign
    for (int i=0; i<6; i++) {
        wire.write(static_cast<uint8_t>(from_callsign[i]));
    }
    wire.write(from_num);

    // Send the beacon frame
    for (int i=0; i<size; i++) {
        wire.write(static_cast<uint8_t>(frame[i]));
    }
    return wire.endTransmission();
}

//Sets the transmitter operating mode.
//Mode 1: Nominal mode. Mode 2: Transponder mode. Any other value will have no effect.
unsigned char Transmitter::set_tx_mode(uint8_t mode) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x38);

    /* Write mode byte */
    wire.write(mode);
    return wire.endTransmission();
}

//Sets the RSSI (Received Signal Strength Indicator) threshold used in the transponder mode to decide whether to transmit a received frame or not.
unsigned char Transmitter::set_rssi_transponder_threshold(uint16_t threshold) {
    /* Write command to Transmitter. */
    wire.beginTransmission(i2c_address);
    wire.write(0x52);

    /* Write threshold bytes, big endian */
    wire.write(static_cast<uint8_t>((threshold >> 8) & 0xFF));
    wire.write(static_cast<uint8_t>(threshold & 0xFF));
    return wire.endTransmission();
}

unsigned char Transmitter::ping() {
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