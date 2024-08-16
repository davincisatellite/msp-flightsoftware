//
// Created by caspar on 6/9/23.
//
// Defines all EPS commands

#ifndef EPS_CONVERSION_EPS_H
#define EPS_CONVERSION_EPS_H

// PDU i2c addresses
//TODO: store them better
#define PDU_1_ADDR 0x20
#define PDU_2_ADDR 0x21
#define PDU_3_ADDR 0x22
#define PDU_4_ADDR 0x23
#define PDU_5_ADDR 0x24
#define PDU_6_ADDR 0x25
#define PDU_7_ADDR 0x26
#define PDU_8_ADDR 0x27

//PBU addresses
#define PBU_1_ADDR 0x28
#define PBU_2_ADDR 0x29
#define PBU_3_ADDR 0x2A
#define PBU_4_ADDR 0x2B

//PCU addresses
#define PCU_1_ADDR 0x2C
#define PCU_2_ADDR 0x2D
#define PCU_3_ADDR 0x2E
#define PCU_4_ADDR 0x2F

#include "DWire.h"
#include "delay.h"

class EPS {
public:
    // standard reply
    struct standard_reply {
        uint8_t stid;
        uint8_t ivid;
        uint8_t rc;
        uint8_t bid;
        uint8_t stat;
        uint8_t error;
    };

    struct pdu_overcurrent_reply{
        uint16_t ocf_cnt_ch[16];
        uint16_t stat_ob_on;
        uint16_t stat_ob_ocf;

        uint8_t stid;
        uint8_t ivid;
        uint8_t rc;
        uint8_t bid;
        uint8_t stat;
        uint8_t error;
    };

    struct pcu_housekeeping_data_reply {
        uint8_t stid;
        uint8_t ivid;
        uint8_t rc;
        uint8_t bid;
        uint8_t stat;

        uint16_t volt_brdsup;
        uint16_t temp;
        uint8_t vip_output[6];
        uint8_t cc[4][14];      // Response CC_1 stored in cc[1]. Bytes are kept in the order that they are read

        uint8_t error;
    };

    struct pbu_housekeeping_data_reply {
        uint8_t stid;
        uint8_t ivid;
        uint8_t rc;
        uint8_t bid;
        uint8_t stat;

        uint16_t volt_brdsup;
        uint16_t temp;
        uint8_t vip_input[6];
        uint16_t stat_bu;
        uint8_t bp[3][22];      // Response BP_1 stored in bp[1]. Bytes are kept in the order that they are read

        uint8_t error;
    };

    static standard_reply reset_watchdog(DWire &wire, uint8_t i2c_address);
    static standard_reply no_operation(DWire &wire, uint8_t i2c_address);
    static standard_reply system_reset(DWire &wire, uint8_t i2c_address);
    static standard_reply cancel_operation(DWire &wire, uint8_t i2c_address);
    static standard_reply watchdog(DWire &wire, uint8_t i2c_address);
    // static pbu_housekeeping_data_reply watchdog(DWire &wire, uint8_t i2c_address); // removed because previous watchdog cannot be overloaded. merge error?
    static pcu_housekeeping_data_reply get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
    static pcu_housekeeping_data_reply get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
    static pdu_overcurrent_reply get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address);
    static pbu_housekeeping_data_reply get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
    static standard_reply switch_safety_mode(DWire &wire, uint8_t i2c_address);
    static standard_reply switch_nominal_mode(DWire &wire, uint8_t i2c_address);
    static standard_reply output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx);
    // static standard_reply output_bus_channel_on(DWire &wire, uint8_t i2c_address, uint8_t ch_idx); // redo with page 36
    static standard_reply output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply reset_configuration(DWire &wire, uint8_t i2c_address);
};

#endif //EPS_CONVERSION_EPS_H
