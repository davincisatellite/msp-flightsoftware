/*
 * receiver.h
 *
 *  Created on: Jun 23, 2023
 *      Author: bgrochowski
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#define RX_I2C_ADDRESS 0x60

#include "DWire.h"
#include "delay.h"

class Receiver {
protected:
    DWire &wire;
    uint8_t i2c_address;
public:
    Receiver(DWire &dwire, uint8_t i2c_address);

    struct frames_in_buffer {
        uint16_t number;
        bool error;
    };

    struct received_frame {
        uint16_t doppler_f;
        uint16_t rssi;
        uint16_t size;
        int frame;
        bool error;
    };

    struct rx_telemetry {
        double doppler_offset;        //Hz
        double signal_strength;       //dB
        double power_bus_voltage;     //V
        double total_current;         //mA
        double tx_current;            //mA
        double rx_current;            //mA
        double poweramp_current;      //mA
        double poweramp_temp;         //degC
        double oscillator_temp;       //degC
        bool error;

    };

    frames_in_buffer frames;
    received_frame frame;
    rx_telemetry telemetry;
    uint32_t uptime;
    unsigned char get_number_of_frames();
    unsigned char report_uptime();
    unsigned char measure_telemetry();
    unsigned char get_frame_from_buffer();
    unsigned char remove_frame_from_buffer();
    unsigned char reset_watchdog();
    unsigned char reset_software();
    unsigned char reset_hardware();
};



#endif /* RECEIVER_H_ */
