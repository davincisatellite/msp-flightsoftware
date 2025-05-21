/*
 * antenna.h
 *
 *  Created on: May 04, 2023
 *      Author: bgrochowski
 */

#define ANTENNA_I2C_ADDRESS_MCA 0x31
#define ANTENNA_I2C_ADDRESS_MCB 0x32

#define ANTENNA_TEMPERATURE_THRESHOLD 0.0 // 0 deg C

#define SUCCESS 0
#define FAIL 1

#ifndef ANTENNA_H_
#define ANTENNA_H_

#include "DWire.h"
#include "delay.h"

class Antenna {
protected:
    DWire &wire;
    uint8_t i2c_address;
public:
    Antenna(DWire &dwire, uint8_t i2c_address);

    struct deployment_status {
        bool a1s;
        bool a1t;
        bool a1b;
        bool a2s;
        bool a2t;
        bool a2b;
        bool a3s;
        bool a3t;
        bool a3b;
        bool a4s;
        bool a4t;
        bool a4b;
        bool ig;
        bool arm;
        bool indb;
        bool error;
    };

    struct deployment_count {
        uint8_t count[4];
        bool error;
    };

    struct ant_temperature {
        // uint16_t Vout;
//        int16_t temp;
        float temp;
        bool error;
    };

    struct deployment_time {
        uint16_t time[4];
        bool error;
    };

    deployment_status status;
    ant_temperature temperature;
    deployment_count deployment_activation_count;
    deployment_time deployment_activation_time;
    unsigned char report_deployment_status();
    unsigned char report_temperature();
    unsigned char reset();
    unsigned char ping();
    unsigned char arm();
    unsigned char disarm();
    unsigned char deploy_sequential();
    unsigned char deploy(uint8_t antenna_no, bool override);
    unsigned char cancel_deploy();
    unsigned char report_deployment_activation_count();
    unsigned char report_deployment_activation_time();
    bool temp_above_threshold();
};



#endif /* ANTENNA_H_ */
