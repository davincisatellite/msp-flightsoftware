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

//TODO enforce read only behaviour too? Could be done with a seperate dictionary maybe. Or we could ensure that the second digit is not 8; 0x?8??
//Note that first number signifies type, and second number read or read/write (except for SAFETY_VOLT_LOTHR and SAFETY_VOLT_HITHR for some whatever reason :cry: )
//Note that for the values from 0x3000 to 0x300B, the ICD groups them in groups of 3 and just gives them one name. However, if I understand it correctly, they are in fact three different battery channels.
//Note that for the values from 0x4008 to 0x4027, the ICD groups them in groups of 16 and just gives them one name. However, if I understand it correctly, they are in fact sixteen enable channels.
//Note that int32, uint32, float, int64, uint64, and double are mentioned as possible data types in the ICD, but are not used as return type for any parameter value.
enum class ConfigParameter { //ICD table 3-25 page 78/87
    BOOT_RESUME_ENA         = 0x1000, //int8     //read/write
    OBUS_VD1_ALWAYS_ENA     = 0x1001, //int8     //read/write
    OBUS_VD2_ALWAYS_ENA     = 0x1002, //int8     //read/write
    OBUS_VD1_ALWAYS_DISA    = 0x1003, //int8     //read/write
    OBUS_VD2_ALWAYS_DISA    = 0x1004, //int8     //read/write
    AUTO_HEAT_ENA_BP1       = 0x1005, //int8     //read/write
    AUTO_HEAT_ENA_BP2       = 0x1006, //int8     //read/write
    AUTO_HEAT_ENA_BP3       = 0x1007, //int8     //read/write
    AUTO_BAL_ENA_BP1        = 0x1008, //int8     //read/write
    AUTO_BAL_ENA_BP2        = 0x1009, //int8     //read/write
    AUTO_BAL_ENA_BP3        = 0x100A, //int8     //read/write
    CONF_PARAM_CHANGED      = 0x1800, //int8     //read only
    BOARD_IDENTIFIER        = 0x2000, //uint8    //read/write
    BOARD_IDENTIFIER_KEY    = 0x2001, //uint8    //read/write
    RAVG_STRENGTH_P2        = 0x2002, //uint8    //read/write
    STID                    = 0x2800, //uint8    //read only
    IVID                    = 0x2801, //uint8    //read only
    BID_USED                = 0x2802, //uint8    //read only
    BOOT_RESUME_SHORT       = 0x2803, //uint8    //read only
    LOTHR_BAT_HEATER_01     = 0x3000, //int16    //read/write    //Note that in the ICD there are just three values (0x3000, 0x3001, 0x3002) that are all just called LOTHR_BAT_HEATER
    LOTHR_BAT_HEATER_02     = 0x3001, //int16    //read/write
    LOTHR_BAT_HEATER_03     = 0x3002, //int16    //read/write
    HITHR_BAT_HEATER_01     = 0x3003, //int16    //read/write    //Note that in the ICD there are just three values (0x3003, 0x3004, 0x3005) that are all just called HITHR_BAT_HEATER
    HITHR_BAT_HEATER_02     = 0x3004, //int16    //read/write
    HITHR_BAT_HEATER_03     = 0x3005, //int16    //read/write
    LOTHR_BMON_UNBAL_01     = 0x3006, //int16    //read/write    //Note that in the ICD there are just three values (0x3006, 0x3007, 0x3008) that are all just called LOTHR_BMON_UNBAL
    LOTHR_BMON_UNBAL_02     = 0x3007, //int16    //read/write
    LOTHR_BMON_UNBAL_03     = 0x3008, //int16    //read/write
    HITHR_BMON_UNBAL_01     = 0x3009, //int16    //read/write    //Note that in the ICD there are just three values (0x3009, 0x300A, 0x300B) that are all just called HITHR_BMON_UNBAL
    HITHR_BMON_UNBAL_02     = 0x300A, //int16    //read/write
    HITHR_BMON_UNBAL_03     = 0x300B, //int16    //read/write
    MCU_TEMP_BIAS           = 0x300C, //int16    //read/write
    MCU_TEMP_PREMUL         = 0x300D, //int16    //read/write
    MCU_TEMP_POSDIV         = 0x300E, //int16    //read/write
    BP1_TEMP1_BIAS          = 0x300F, //int16    //read/write
    BP1_TEMP2_BIAS          = 0x3010, //int16    //read/write
    BP1_TEMP3_BIAS          = 0x3011, //int16    //read/write
    BP2_TEMP1_BIAS          = 0x3012, //int16    //read/write
    BP2_TEMP2_BIAS          = 0x3013, //int16    //read/write
    BP2_TEMP3_BIAS          = 0x3014, //int16    //read/write
    BP3_TEMP1_BIAS          = 0x3015, //int16    //read/write
    BP3_TEMP2_BIAS          = 0x3016, //int16    //read/write
    BP3_TEMP3_BIAS          = 0x3017, //int16    //read/write
    BP1_TEMP1_PREMUL        = 0x3018, //int16    //read/write
    BP1_TEMP2_PREMUL        = 0x3019, //int16    //read/write
    BP1_TEMP3_PREMUL        = 0x301A, //int16    //read/write
    BP2_TEMP1_PREMUL        = 0x301B, //int16    //read/write
    BP2_TEMP2_PREMUL        = 0x301C, //int16    //read/write
    BP2_TEMP3_PREMUL        = 0x301D, //int16    //read/write
    BP3_TEMP1_PREMUL        = 0x301E, //int16    //read/write
    BP3_TEMP2_PREMUL        = 0x301F, //int16    //read/write
    BP3_TEMP3_PREMUL        = 0x3020, //int16    //read/write
    BP1_TEMP1_POSDIV        = 0x3021, //int16    //read/write
    BP1_TEMP2_POSDIV        = 0x3022, //int16    //read/write
    BP1_TEMP3_POSDIV        = 0x3023, //int16    //read/write
    BP2_TEMP1_POSDIV        = 0x3024, //int16    //read/write
    BP2_TEMP2_POSDIV        = 0x3025, //int16    //read/write
    BP2_TEMP3_POSDIV        = 0x3026, //int16    //read/write
    BP3_TEMP1_POSDIV        = 0x3027, //int16    //read/write
    BP3_TEMP2_POSDIV        = 0x3028, //int16    //read/write
    BP3_TEMP3_POSDIV        = 0x3029, //int16    //read/write
    ADC_MCU_TEMP_V25T30     = 0x3800, //int16    //read only
    ADC_MCU_TEMP_V25T85     = 0x3801, //int16    //read only
    TTC_WDG_TIMEOUT         = 0x4000, //uint16   //read/write
    TTC_WDG_TIMEOUT_KEY     = 0x4001, //uint16   //read/write
    OBUS_STARTUP_ENA_BF     = 0x4004, //uint16   //read/write
    OBUS_STARTUP_KEY        = 0x4005, //uint16   //read/write
    OBUS_LATCHOFF_ENA_BF    = 0x4006, //uint16   //read/write
    OBUS_LATCHOFF_KEY       = 0x4007, //uint16   //read/write
    OBUS_STARTUP_DELAY_01   = 0x4008, //uint16   //read/write    //Note that in the ICD there are just 16 values (0x4008-0x4017) that are all just called OBUS_STARTUP_DELAY
    OBUS_STARTUP_DELAY_02   = 0x4009, //uint16   //read/write
    OBUS_STARTUP_DELAY_03   = 0x400A, //uint16   //read/write
    OBUS_STARTUP_DELAY_04   = 0x400B, //uint16   //read/write
    OBUS_STARTUP_DELAY_05   = 0x400C, //uint16   //read/write
    OBUS_STARTUP_DELAY_06   = 0x400D, //uint16   //read/write
    OBUS_STARTUP_DELAY_07   = 0x400E, //uint16   //read/write
    OBUS_STARTUP_DELAY_08   = 0x400F, //uint16   //read/write
    OBUS_STARTUP_DELAY_09   = 0x4010, //uint16   //read/write
    OBUS_STARTUP_DELAY_10   = 0x4011, //uint16   //read/write
    OBUS_STARTUP_DELAY_11   = 0x4012, //uint16   //read/write
    OBUS_STARTUP_DELAY_12   = 0x4013, //uint16   //read/write
    OBUS_STARTUP_DELAY_13   = 0x4014, //uint16   //read/write
    OBUS_STARTUP_DELAY_14   = 0x4015, //uint16   //read/write
    OBUS_STARTUP_DELAY_15   = 0x4016, //uint16   //read/write
    OBUS_STARTUP_DELAY_16   = 0x4017, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_01  = 0x4018, //uint16   //read/write    //Note that in the ICD there are just 16 values (0x4018-0x4027) that are all just called OBUS_LATCHOFF_DELAY
    OBUS_LATCHOFF_DELAY_02  = 0x4019, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_03  = 0x401A, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_04  = 0x401B, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_05  = 0x401C, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_06  = 0x401D, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_07  = 0x401E, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_08  = 0x401F, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_09  = 0x4020, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_10  = 0x4021, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_11  = 0x4022, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_12  = 0x4023, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_13  = 0x4024, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_14  = 0x4025, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_15  = 0x4026, //uint16   //read/write
    OBUS_LATCHOFF_DELAY_16  = 0x4027, //uint16   //read/write
    SAFETY_VOLT_LOTHR       = 0x480D, //uint16   //read/write    //Note that its second digit is 8, but it is read/write
    SAFETY_VOLT_HITHR       = 0x480E, //uint16   //read/write    //Note that its second digit is 8, but it is read/write
    TTC_I2C_SLAVE_ADDR      = 0x4800, //uint16   //read only
    CONF_NVM_SAVE_CNTR      = 0x4801, //uint16   //read only
    CONF_NVM_SAVE_CHKS      = 0x4802, //uint16   //read only
    RST_CAUSE               = 0x4803, //uint16   //read only
    RST_CNTR_PWRON          = 0x4804, //uint16   //read only
    RST_CNTR_WDG            = 0x4805, //uint16   //read only
    RST_CNTR_CMD            = 0x4806, //uint16   //read only
    RST_CNTR_MCU            = 0x4807, //uint16   //read only
    RST_CNTR_EMLOPO         = 0x4808, //uint16   //read only
    RST_CODE_MCU_RAW        = 0x4809, //uint16   //read only
    EMLOPO_VOLT_LOTHR       = 0x480A, //uint16   //read only
    EMLOPO_VOLT_HITHR       = 0x480B, //uint16   //read only
    EMLOPO_PERIOD           = 0x480C, //uint16   //read only
    SAFETY_VOLT_LOTHR_USED  = 0x480D, //uint16   //read only
    SAFETY_VOLT_HITHR_USED  = 0x480E, //uint16   //read only
    SAFETY_LINGER           = 0x480F, //uint16   //read only
    TTC_WDG_TIMOUT_USED     = 0x4812, //uint16   //read only
    TTC_PREVCMD_ELAPSED     = 0x4813, //uint16   //read only
    OB_FORCE_ENA_USE_BF     = 0x4815, //uint16   //read only
    OB_STARTUP_ENA_USE_BF   = 0x4816, //uint16   //read only
    OB_LATCHOFF_ENA_USE_BF  = 0x4817, //uint16   //read only
    VD0_CH_BF               = 0x4818, //uint16   //read only
    VD1_CH_BF               = 0x4819, //uint16   //read only
    VD2_CH_BF               = 0x481A, //uint16   //read only
    VD3_CH_BF               = 0x481B, //uint16   //read only
    VD4_CH_BF               = 0x481C, //uint16   //read only
    VD5_CH_BF               = 0x481D, //uint16   //read only
    VD6_CH_BF               = 0x481E //uint16   //read only
};

enum ParameterType {
    // List of valid Param-IDs based on Table 3-24: Possible Parameter Data Types from page 77 ICD
    Int8 = 0x1000,
    UInt8 = 0x2000,
    Int16 = 0x3000,
    UInt16 = 0x4000,
    Int32 = 0x5000, //note, not used
    UInt32 = 0x6000, //note, not used
    Float = 0x7000, //note, not used
    Int64 = 0x8000, //note, not used
    UInt64 = 0x9000, //note, not used
    Double = 0xA000, //note, not used
    Invalid
    };

enum AccessType {
    ReadOnly,
    ReadWrite
};

union returnType {
    int8_t i8;
    uint8_t ui8;
    int16_t i16;
    uint16_t ui16;
    int32_t i32;
    uint32_t ui32;
    float f;
    int64_t i64;
    uint64_t ui64;
    double d;
};

enum class CommandCode {
    GET_PARAM = 0x82,                        // reference: page 62 of 87 (ICD)
    SET_PARAM = 0x84,                        // reference: page 64 of 87 (ICD)
    RESET_PARAM = 0x86,                      // reference: page 65 of 87 (ICD)
    WATCHDOG = 0x06,                         // reference: page 32 of 87 (ICD)
    NO_OPERATION = 0x02,                     // reference: page 30 of 87 (ICD)
    SYSTEM_RESET = 0xAA,                     // reference: page 29 of 87 (ICD)
    CANCEL_OPERATION = 0x04,                 // reference: page 31 of 87 (ICD)
    SWITCH_TO_SAFETY_MODE = 0x32,            // reference: page 39 of 87 (ICD)
    GET_PDU_OVERCURRENT_FAULT_STATE = 0x42,  // reference: page 43 of 87 (ICD)
    GET_PDU_ABF_PLACED_STATE = 0x44,         // reference: page 44 of 87 (ICD)
    GET_PDU_HOUSEKEEPING_DATA_RAW = 0x50,    // reference: page 46 of 87 (ICD)
    GET_PDU_HOUSEKEEPING_DATA_ENG = 0x52,    // reference: page 49 of 87 (ICD)
    GET_PDU_HOUSEKEEPING_DATA_AVG = 0x54,    // reference: page 51 of 87 (ICD)
    GET_PBU_HOUSEKEEPING_DATA_RAW = 0x60,    // reference: page 53 of 87 (ICD)
    GET_PBU_HOUSEKEEPING_DATA_ENG = 0x62,    // reference: page 54 of 87 (ICD)
    GET_PBU_HOUSEKEEPING_DATA_AVG = 0x64,    // reference: page 56 of 87 (ICD)
    GET_PCU_HOUSEKEEPING_DATA_ENG = 0x72,    // reference: page 60 of 87 (ICD)
    GET_PCU_HOUSEKEEPING_DATA_AVG = 0x74,    // reference: page 61 of 87 (ICD)
    GET_PCU_HOUSEKEEPING_DATA_RAW = 0x70,    // reference: page 58 of 87 (ICD)
    SWITCH_NOMINAL_MODE = 0x30,              // reference: page 38 of 87 (ICD)
    OUTPUT_BUS_CHANNEL_OFF = 0x18,           // reference: page 37 of 87 (ICD)
    OUTPUT_BUS_GROUP_STATE = 0x14,           // reference: page 35 of 87 (ICD)
    OUTPUT_BUS_GROUP_OFF = 0x12,             // reference: page 34 of 87 (ICD)
    OUTPUT_BUS_GROUP_ON = 0x10,              // reference: page 33 of 87 (ICD)
    RESET_CONFIGURATION = 0x90               // reference: page 66 of 87 (ICD)
};

// Constants for command codes and other identifiers
enum Identifiers {
    STID = 0x00,                                      // reference: page 17 of 87 (ICD)
    IVID = 0x06,                                      // reference: page 19 of 87 (ICD)
    BID = 0x00                                        // reference: page 20 of 87 (ICD)
};

// Enum for reset keys or confirmation keys
enum ResetKey {
    RESET_KEY_SYSTEM_RESET = 0xA6,                    // reference: page 29 of 87 (ICD)
    CONF_KEY_RESET_CONFIGURATION = 0xA7               // reference: page 66 of 87 (ICD)
};

class EPS {
public:

    // command codes all replies have
    class ReplyBase {
    public:
        uint8_t stid;
        uint8_t ivid;
        uint8_t rc; //TODO according to ICD, GetPBUHousekeepingData(ENG) returns the cc instead? I believe it is the only one
        uint8_t bid;
        uint8_t stat;
        uint8_t error;

        virtual ~ReplyBase() = default; // Virtual destructor for proper cleanup
    };

    // standard reply
    struct standard_reply : public ReplyBase {

    };

    struct pdu_overcurrent_reply : public ReplyBase {
        uint16_t ocf_cnt_ch[16];
        uint16_t stat_ob_on;
        uint16_t stat_ob_ocf;
    };

    struct pdu_abf_placed_state : public ReplyBase {
        uint8_t abf_placed_0;
        uint8_t abf_placed_1;
    };

    struct config_reply : public ReplyBase {
        ConfigParameter par_id;
        returnType conf_par; // maximum length of PAR_VAL
    };

    struct pcu_housekeeping_data_reply : public ReplyBase {
        uint16_t volt_brdsup;
        uint16_t temp;
        uint8_t vip_output[6];
        uint8_t cc[4][14];      // Response CC_1 stored in cc[1]. Bytes are kept in the order that they are read
    };

    struct pbu_housekeeping_data_reply : public ReplyBase {
        uint16_t volt_brdsup;
        uint16_t temp;
        uint8_t vip_input[6];
        uint16_t stat_bu;
        uint8_t bp[3][22];      // Response BP_1 stored in bp[1]. Bytes are kept in the order that they are read
    };

    struct pdu_housekeeping_data_reply : public ReplyBase {
        uint16_t volt_brdsup;
        uint16_t temp;
        uint8_t vip_input[6];
        uint16_t stat_ch_on;
        uint16_t stat_ch_ocf;
        uint8_t vip_vd[7][6];
        uint8_t vip_ch[16][6];
    };

    static standard_reply no_operation(DWire &wire, uint8_t i2c_address);
    static standard_reply system_reset(DWire &wire, uint8_t i2c_address);
    static standard_reply cancel_operation(DWire &wire, uint8_t i2c_address);
    static standard_reply watchdog(DWire &wire, uint8_t i2c_address);
    // static pbu_housekeeping_data_reply watchdog(DWire &wire, uint8_t i2c_address); // removed because previous watchdog cannot be overloaded. merge error?
    static pcu_housekeeping_data_reply get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
    static pcu_housekeeping_data_reply get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
    static pcu_housekeeping_data_reply get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
    static pdu_overcurrent_reply get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address);
    static pdu_abf_placed_state get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address);
    static pdu_housekeeping_data_reply get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
    static pdu_housekeeping_data_reply get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
    static pdu_housekeeping_data_reply get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
    static pbu_housekeeping_data_reply get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
    static pbu_housekeeping_data_reply get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
    static pbu_housekeeping_data_reply get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
    static standard_reply switch_safety_mode(DWire &wire, uint8_t i2c_address);
    static standard_reply switch_nominal_mode(DWire &wire, uint8_t i2c_address);
    static standard_reply output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx);
    // static standard_reply output_bus_channel_on(DWire &wire, uint8_t i2c_address, uint8_t ch_idx); // redo with page 36
    static standard_reply output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
    static standard_reply reset_configuration(DWire &wire, uint8_t i2c_address);
    static config_reply get_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
    static config_reply reset_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
    static config_reply set_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id, returnType conf_par_value);

    static ParameterType getConfigParameterType(ConfigParameter conf_par);
    static AccessType getAccessType(ConfigParameter conf_par);
    static uint8_t get_param_length(ParameterType par_type);
    static void writeCommand(DWire &wire, uint8_t i2c_address, CommandCode commandCode);
    static void readCommand(DWire &wire, EPS::ReplyBase &reply);
    static bool write_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, CommandCode commandCode);
};

#endif //EPS_CONVERSION_EPS_H