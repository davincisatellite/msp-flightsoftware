/*
 * transmitter.h
 *
 *  Created on: May 06, 2023
 *      Author: bgrochowski
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#define TX_I2C_ADDRESS 0x61

#include "DWire.h"
#include "delay.h"

class Transmitter {
protected:
    DWire &wire;
    uint8_t i2c_address;
public:
    Transmitter(DWire &dwire, uint8_t i2c_address_def);

    struct transmitter_state {
        bool on_idle; //true = tx remains on while idle
        bool beacon_active;
        uint16_t tx_bitrate;
        bool error;
    };

    struct tx_telemetry {
        double rf_reflected_power;    //mW
        double rf_forward_power;      //mW
        double power_bus_voltage;     //V
        double total_current;         //mA
        double tx_current;            //mA
        double rx_current;            //mA
        double poweramp_current;      //mA
        double poweramp_temp;         //degC
        double oscillator_temp;       //degC
        bool error;
    };

    struct tx_buffer {
        uint8_t free_slots;
        bool error;
    };

    transmitter_state state;
    tx_telemetry last_telemetry;
    tx_telemetry measured_telemetry;
    uint32_t uptime;
    tx_buffer buffer;
    unsigned char report_state();
    unsigned char report_last_telemetry();
    unsigned char measure_telemetry();
    unsigned char report_uptime();
    unsigned char set_bitrate(uint16_t bitrate);
    unsigned char set_idle_mode(bool on_idle);
    unsigned char set_default_to_callsign(char to_callsign[6], uint8_t num);
    unsigned char set_default_from_callsign(char from_callsign[6], uint8_t from);
    unsigned char reset_watchdog();
    unsigned char clear_beacon();
    unsigned char reset_software();
    unsigned char reset_hardware();
    unsigned char send_frame(uint8_t* frame, uint8_t size);
    unsigned char send_frame_override_cs(uint8_t* frame, uint8_t size, char to_callsign[6], uint8_t to_num, char from_callsign[6], uint8_t from_num);
    unsigned char set_beacon(uint8_t* frame, uint8_t size, uint16_t time_interval);
    unsigned char set_beacon_override_cs(uint8_t* frame, uint8_t size, uint16_t time_interval, char to_callsign[6], uint8_t to_num, char from_callsign[6], uint8_t from_num);


};



#endif /* TRANSMITTER_H_ */
