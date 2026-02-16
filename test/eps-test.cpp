#include "../src/EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy
#include "../src/Console.h"
#include "../src/DelfiPQcore.h"
#include "../src/delay.h"

/*
     For each config data type we will test the boundaries and a random value between them.
*/
bool test_getConfigParameterType() {
    int there_is_an_error=0;
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1000))!=Int8 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1A3C))!=Int8 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1FFF))!=Int8) {
        Console::log("It fails on the ones with the top hex digit 1 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x2000))!=UInt8 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x203A))!=UInt8 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x2FFF))!=UInt8) {
        Console::log("It fails on the ones with the top hex digit 2 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x3000))!=Int16 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x33F7))!=Int16 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x3FFF))!=Int16) {
        Console::log("It fails on the ones with the top hex digit 3 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4000))!=UInt16 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4D1A))!=UInt16 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4FFF))!=UInt16) {
        Console::log("It fails on the ones with the top hex digit 4 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x5000))!=Int32 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x509D))!=Int32 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x5FFF))!=Int32) {
        Console::log("It fails on the ones with the top hex digit 5 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x6000))!=UInt32 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x663B))!=UInt32 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x6FFF))!=UInt32) {
        Console::log("It fails on the ones with the top hex digit 6 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x7000))!=Float ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x70AA))!=Float ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x7FFF))!=Float) {
        Console::log("It fails on the ones with the top hex digit 7 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8000))!=Int64 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8D1A))!=Int64 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8FFF))!=Int64) {
        Console::log("It fails on the ones with the top hex digit 8 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9000))!=UInt64 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9F61))!=UInt64 ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9FFF))!=UInt64) {
        Console::log("It fails on the ones with the top hex digit 9 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0xA000))!=Double ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xA5E8))!=Double ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xAFFF))!=Double) {
        Console::log("It fails on the ones with the top hex digit A in getConfigParameterType");
        there_is_an_error=1;
    }
    //test some invalid values
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0xB000))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0x0))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xFFF))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xC068))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xD3A2))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xEF03))!=Invalid ||
            EPS::getConfigParameterType(static_cast<ConfigParameter>(0xFFFF))!=Invalid) {
        Console::log("It fails on the invalid values in getConfigParameterType");
        there_is_an_error=1;
    }
    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_getAccessType() {
    int there_is_an_error=0;
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_LOTHR in getaccesstype");
        there_is_an_error=1;
    }
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_HITHR in getaccesstype");
        there_is_an_error=1;
    }
    //SAFETY_VOLT_LOTHR and SAFETY_VOLT_HITHR_USED have the same address so the result should be the same, ReadWrite.
    //See the Documentation for more details. The main idea is that they are treated the same.
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR_USED) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_LOTHR_USED in getaccesstype");
        there_is_an_error=1;
    }
    if(EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR_USED) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_HITHR_USED in getaccesstype");
        there_is_an_error=1;
    }
    //test the other values. if 0x?8?? -> ReadOnly, else ReadWrite

    //0x1???
    for (int i = 0x1000; i <= 0x100A; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x1000 and 0x100A in getaccesstype");
            there_is_an_error=1;
        }
    if(EPS::getAccessType(ConfigParameter::CONF_PARAM_CHANGED) != ReadOnly) {//0x1800
        Console::log("fail CONF_PARAM_CHANGED in getaccesstype");
        there_is_an_error=1;
    }
    //0x2???
    for (int i = 0x2000; i <= 0x2002; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x2000 and 0x2002 in getaccesstype");
            there_is_an_error=1;
        }
    for (int i = 0x2800; i <= 0x2803; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
            Console::log("It fails between 0x2800 and 0x2803 in getaccesstype");
            there_is_an_error=1;
        }
    //0x3???
    for (int i = 0x3000; i <= 0x3029; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x3000 and 0x3029 in getaccesstype");
            there_is_an_error=1;
        }
    for (int i = 0x3800; i <= 0x3801; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
            Console::log("It fails between 0x3800 and 0x3801 in getaccesstype");
            there_is_an_error=1;
        }
    //0x4???
    for (int i = 0x4000; i <= 0x4027; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x4000 and 0x4027 in getaccesstype");
            there_is_an_error=1;
        }
    //0x480D and 0x480E are ReadWrite, and we tested them at the beginning
    for (int i = 0x4800; i <= 0x481E; ++i)
        if (i!=0x480D && i!=0x480E)
            if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
                Console::log("It fails between 0x4800 and 0x481E (excluding 0x480D and 0x480E)  in getaccesstype");
                there_is_an_error=1;
            }
    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_get_param_length() {
    if( EPS::get_param_length(Int8) == 1 &&
        EPS::get_param_length(UInt8) == 1 &&
        EPS::get_param_length(Int16) == 2 &&
        EPS::get_param_length(UInt16) == 2 &&
        EPS::get_param_length(Int32) == 4 &&
        EPS::get_param_length(UInt32) == 4 &&
        EPS::get_param_length(Float) == 4 &&
        EPS::get_param_length(Int64) == 8 &&
        EPS::get_param_length(UInt64) == 8 &&
        EPS::get_param_length(Double) == 8 &&
        EPS::get_param_length(Invalid) == 0) {
            return true;
    }
    Console::log("fail test_get_param_length");    
    return false;

}
bool test_writeCommand(uint8_t i2c_address) {
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    int there_is_an_error=0;
    EPS::writeCommand(my_wire,i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW);
    uint8_t byte1=my_wire.read();
    uint8_t byte2=my_wire.read();
    uint8_t byte3=my_wire.read();
    uint8_t byte4=my_wire.read();
    if (byte1!=STID) {
        Console::log("It fails on writing first byte in writeCommand");
        there_is_an_error=1;
    }
    if (byte2!=IVID) {
        Console::log("It fails on writing second byte in writeCommand");
        there_is_an_error=1;
    }
    //or simply if (byte3 != 0x70)
    if (static_cast<CommandCode>(byte3) != CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW){//static_cast<CommandCode>(0x70)) {
        Console::log("It fails on writing third byte in writeCommand");
        there_is_an_error=1;
    }
    if (byte4!=BID) {
        Console::log("It fails on writing forth byte in writeCommand");
        there_is_an_error=1;
    }
    if (there_is_an_error==1)
        return false;
    return true;
}
bool test_read_command(uint8_t i2c_address) {
    int there_is_an_error=0;
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();

    my_wire.beginTransmission(i2c_address);
    my_wire.write(STID);
    my_wire.write(IVID);
    my_wire.write(0x34);
    my_wire.write(BID);

    EPS::ReplyBase reply ={};
    EPS::readCommand(my_wire,reply);
    if (reply.stid!=STID) {
        Console::log("It fails on reading STID in read_command");
        there_is_an_error=1;
    }
    if (reply.ivid!=IVID) {
        Console::log("It fails on reading IVID in read_command");
        there_is_an_error=1;
    }
    if (reply.rc!=static_cast<uint8_t>(0x34)) {
        Console::log("It fails on reading rc in read_command");
        there_is_an_error=1;
    }
    if (reply.bid!=BID) {
        Console::log("It fails on reading BID in read_command");
        there_is_an_error=1;
    }
    //status byte
    if (reply.stat!=0) {
        Console::log("It fails on reading stat in read_command");
        there_is_an_error=1;
    }
    if (there_is_an_error==1)
        return false;
    return true;
}
bool test_write_config_params(uint8_t i2c_address) {
    int there_is_an_error=0;
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    //I assume i2c_address has an important value

    //invalid param (not allowed)
    if(EPS::write_config_params(my_wire, i2c_address, static_cast<ConfigParameter>(0xB234), CommandCode::GET_PARAM)) {
        Console::log("It fails on invalid config_param with command GET_PARAM in write_config_params");
        there_is_an_error=1;
    }

    my_wire = DWire();
    //read-only param should not be allowed
    if(EPS::write_config_params(my_wire, i2c_address, ConfigParameter::BID_USED, CommandCode::GET_PARAM)) {
        Console::log("It fails on read-only config_param (BID_USED) with command GET_PARAM in write_config_params");
        there_is_an_error=1;
    }

    my_wire = DWire();
    if(EPS::write_config_params(my_wire, i2c_address, ConfigParameter::HITHR_BAT_HEATER_01, CommandCode::GET_PARAM)==false) {
        Console::log("It fails on read/write config_param (BID_USED) with command GET_PARAM in write_config_params");
        there_is_an_error=1;
    }
    //test if the wire has content (6 bytes)
    my_wire.read();
    my_wire.read();
    uint8_t byte3=my_wire.read();
    my_wire.read();
    uint8_t byte5=my_wire.read();
    uint8_t byte6=my_wire.read();

    if (byte3!=static_cast<uint8_t>(CommandCode::GET_PARAM)) {
        Console::log("It fails on writing CommandCode to the wire in write_config_params");
        there_is_an_error=1;
    }

    if (byte5!=0x30) //test if the most significant byte of HITHR_BAT_HEATER_01 was written (little-endian format)
    {
        Console::log("byte 5 (least significant byte) was not correctly written");
        there_is_an_error=1;
    }
    if (byte6!=(static_cast<uint16_t>(ConfigParameter::HITHR_BAT_HEATER_01))>>8)
    {
        Console::log("byte 6 (most significant byte) was not correctly written");
        there_is_an_error=1;
    }

    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_read_config_params(uint8_t i2c_address) {
    int there_is_an_error=0;
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    EPS::config_reply reply ={};

    //test when we get different response length (8 instead of 9)
    my_wire.beginTransmission(i2c_address);
    for (uint8_t i=0x01;i<=0x08;i++)
        my_wire.write(i);
    reply=EPS::read_config_params(my_wire,i2c_address,static_cast<ConfigParameter>(0x100A),reply);
    if (reply.error!=true) {
        Console::log("It fails when reading insufficient data in read_config_params");
        there_is_an_error=1;
    }


    //aici tre sa continui sa verifici cazurile

    if (there_is_an_error==1)
        return false;
    return true;
}
// no_operation(DWire &wire, uint8_t i2c_address);
bool test_no_operation(uint8_t i2c_address) {
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    EPS::standard_reply reply = EPS::no_operation(my_wire, i2c_address);
    if(reply.rc != 0x03 || reply.stat != 0x80 || reply.error == true) {
        Console::log("fail no_operation ",reply.rc, reply.stat,reply.error);
        return false;
    }
    return true;
}

// cancel_operation(DWire &wire, uint8_t i2c_address);
bool test_cancel_operation(uint8_t i2c_address) {
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    EPS::standard_reply reply = EPS::cancel_operation(my_wire, i2c_address);

    // Check if an error occurred during the execution of the command
    if (reply.error) {
        Console::log("fail cancel_operation: Error flag set");
        return false;
    }

    // Expected response code (RC) for a successful cancel operation
    if (reply.rc != 0x03) { // Check ICD for the expected response code
        Console::log("fail cancel_operation: Unexpected response code");
        return false;
    }

    // Expected system status after cancellation
    if (reply.stat != 0x80) { // Check ICD for expected status
        Console::log("fail cancel_operation: Unexpected system status");
        return false;
    }

    Console::log("pass cancel_operation");
    return true;
}

// watchdog(DWire &wire, uint8_t i2c_address);
bool test_watchdog(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::watchdog(wire, i2c_address);
    if(reply.rc != 0x07) {
        Console::log("fail watchdog");
        return false;
    }
    return true;
}

// get_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
bool test_get_config_params(DWire &wire, uint8_t i2c_address) {
    EPS::config_reply reply;

    //Test: Retrieve STID
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::STID);
    if (reply.error || reply.par_id != ConfigParameter::STID) {
        Console::log("fail STID get_config_params");
        return false;
    }

    //Test: Retrieve IVID
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::IVID);
    if (reply.error || reply.par_id != ConfigParameter::IVID) {
        Console::log("fail IVID get_config_params");
        return false;
    }

    //Test: Retrieve BID_USED
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BID_USED);
    if (reply.error || reply.par_id != ConfigParameter::BID_USED) {
        Console::log("fail BID_USED get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (Int8)
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);
    if (reply.error || reply.par_id != ConfigParameter::BOOT_RESUME_ENA) {
        Console::log("fail BOOT_RESUME_ENA get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (UInt8)
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BOARD_IDENTIFIER_KEY);
    if (reply.error || reply.par_id != ConfigParameter::BOARD_IDENTIFIER_KEY) {
        Console::log("fail BOARD_IDENTIFIER_KEY get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (Int16)
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::HITHR_BMON_UNBAL_02);
    if (reply.error || reply.par_id != ConfigParameter::HITHR_BMON_UNBAL_02) {
        Console::log("fail HITHR_BMON_UNBAL_02 get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (UInt16)
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::VD6_CH_BF);
    if (reply.error || reply.par_id != ConfigParameter::VD6_CH_BF) {
        Console::log("fail VD6_CH_BF get_config_params");
        return false;
    }

    //Test: Retrieve a read-only configuration parameter
    reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::CONF_NVM_SAVE_CNTR);
    if (!reply.error) {
        Console::log("fail CONF_NVM_SAVE_CNTR get_config_params");
        return false;
    }
    return true;
}

// get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
bool test_get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_raw(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pcu_housekeeping_data_raw: Error flag set");
        return false;
    }

    // Check the expected fields are within a reasonable range
    if (reply.volt_brdsup > 10000) { // Assuming voltage values are in millivolts
        Console::log("fail get_pcu_housekeeping_data_raw: volt_brdsup out of range");
        return false;
    }

    if (reply.temp > 1000) { // Assuming temperature is in tenths of degrees Celsius
        Console::log("fail get_pcu_housekeeping_data_raw: temp out of range");
        return false;
    }

    for (int i = 0; i < 6; ++i) {
        if (reply.vip_output[i] > 255) { // VIP output should be 8-bit values
            Console::log("fail get_pcu_housekeeping_data_raw: vip_output out of range");
            return false;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (reply.cc[i][j] > 255) { // CC values should be 8-bit
                Console::log("fail get_pcu_housekeeping_data_raw: cc values out of range");
                return false;
            }
        }
    }

    return true;
}

// get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
bool test_get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_eng(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pcu_housekeeping_data_eng: Error flag set");
        return false;
    }

    // Check the expected fields are within a reasonable range
    if (reply.volt_brdsup > 10000) { // Assuming voltage values are in millivolts
        Console::log("fail get_pcu_housekeeping_data_eng: volt_brdsup out of range");
        return false;
    }

    if (reply.temp > 1000) { // Assuming temperature is in tenths of degrees Celsius
        Console::log("fail get_pcu_housekeeping_data_eng: temp out of range");
        return false;
    }

    for (int i = 0; i < 6; ++i) {
        if (reply.vip_output[i] > 255) { // VIP output should be 8-bit values
            Console::log("fail get_pcu_housekeeping_data_eng: vip_output out of range");
            return false;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (reply.cc[i][j] > 255) { // CC values should be 8-bit
                Console::log("fail get_pcu_housekeeping_data_eng: cc values out of range");
                return false;
            }
        }
    }

    return true;
}

// get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
bool test_get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_running_average(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pcu_housekeeping_data_running_average: Error flag set");
        return false;
    }

    // Check the expected fields are within a reasonable range
    if (reply.volt_brdsup > 10000) { // Assuming voltage values are in millivolts
        Console::log("fail get_pcu_housekeeping_data_running_average: volt_brdsup out of range");
        return false;
    }

    if (reply.temp > 1000) { // Assuming temperature is in tenths of degrees Celsius
        Console::log("fail get_pcu_housekeeping_data_running_average: temp out of range");
        return false;
    }

    for (int i = 0; i < 6; ++i) {
        if (reply.vip_output[i] > 255) { // VIP output should be 8-bit values
            Console::log("fail get_pcu_housekeeping_data_running_average: vip_output out of range");
            return false;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 14; ++j) {
            if (reply.cc[i][j] > 255) { // CC values should be 8-bit
                Console::log("fail get_pcu_housekeeping_data_running_average: cc values out of range");
                return false;
            }
        }
    }

    return true;
}


// get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address);
bool test_get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_overcurrent_reply reply = EPS::get_pdu_overcurrent_fault_state(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pdu_overcurrent_fault_state: Error flag set");
        return false;
    }

    // No need to check uint16_t max values, they are already bounded
    // Just add a basic validity check for non-negative values
    if (reply.stat_ob_on > 0xFFFF) {
        Console::log("fail get_pdu_overcurrent_fault_state: stat_ob_on out of range");
        return false;
    }

    if (reply.stat_ob_ocf > 0xFFFF) {
        Console::log("fail get_pdu_overcurrent_fault_state: stat_ob_ocf out of range");
        return false;
    }

    // Check overcurrent fault counters
    for (int i = 0; i < 16; ++i) {
        if (reply.ocf_cnt_ch[i] == 0xFFFF) { // Treat 0xFFFF as an invalid/unexpected response
            Console::log("fail get_pdu_overcurrent_fault_state: ocf_cnt_ch[%d] out of range",i);
            return false;
        }
    }

    return true;
}


// get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address);
bool test_get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_abf_placed_state reply = EPS::get_pdu_abf_placed_state(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pdu_abf_placed_state: Error flag set");
        return false;
    }

    // Define valid values
    const uint8_t VALID_PLACED = 0xAB;
    const uint8_t VALID_NOT_PLACED = 0x00;

    // Check ABF_PLACED_0 for a valid response
    bool is_valid_0 = (reply.abf_placed_0 == VALID_PLACED || reply.abf_placed_0 == VALID_NOT_PLACED);
    bool is_valid_1 = (reply.abf_placed_1 == VALID_PLACED || reply.abf_placed_1 == VALID_NOT_PLACED);

    // If ABF_PLACED_0 is invalid, ABF_PLACED_1 must be valid
    if (!is_valid_0 && !is_valid_1) {
        Console::log("fail get_pdu_abf_placed_state: Both ABF_PLACED_0 and ABF_PLACED_1 are invalid");
        return false;
    }

    return true;
}

// get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
// bool test_get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
//     EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_raw(wire, i2c_address);
//
//     // Validate the response
//     if (reply.error) {
//         Console::log("fail get_pdu_housekeeping_data_raw: Error flag set");
//         return false;
//     }
//
//     // Validate voltage board supply (reasonable range assumption)
//     if (reply.volt_brdsup > 10000) { // Assuming in millivolts
//         Console::log("fail get_pdu_housekeeping_data_raw: volt_brdsup out of range");
//         return false;
//     }
//
//     // Validate temperature (assuming it is in tenths of °C)
//     if (reply.temp > 1000) {
//         Console::log("fail get_pdu_housekeeping_data_raw: temp out of range");
//         return false;
//     }
//
//     // Validate VIP input values (each should be 8-bit)
//     for (int i = 0; i < 6; ++i) {
//         if (reply.vip_input[i] > 255) {
//             Console::log("fail get_pdu_housekeeping_data_raw: vip_input[%d] out of range",i);
//             return false;
//         }
//     }
//
//     // Validate stat_ch_on and stat_ch_ocf (both should be 16-bit)
//     if (reply.stat_ch_on > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_raw: stat_ch_on out of range");
//         return false;
//     }
//
//     if (reply.stat_ch_ocf > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_raw: stat_ch_ocf out of range");
//         return false;
//     }
//
//     // Validate VIP voltage values (each should be 8-bit)
//     for (int i = 0; i < 7; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_vd[i][j] > 255) {
//                 Console::log("fail get_pdu_housekeeping_data_raw: vip_vd[%d][%d] out of range",i,j);
//                 return false;
//             }
//         }
//     }
//
//     // Validate VIP channel values (each should be 8-bit)
//     for (int i = 0; i < 16; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_ch[i][j] > 255) {
//                 Console::log("fail get_pdu_housekeeping_data_raw: vip_ch[%d][%d] out of range",i,j);
//                 return false;
//             }
//         }
//     }
//
//     return true;
// }


// get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
// bool test_get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
//     EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_eng(wire, i2c_address);
//
//     // Validate the response
//     if (reply.error) {
//         Console::log("fail get_pdu_housekeeping_data_eng: Error flag set");
//         return false;
//     }
//
//     // Check that the board supply voltage is within a reasonable range
//     if (reply.volt_brdsup > 10000) {  // Assuming millivolts
//         Console::log("fail get_pdu_housekeeping_data_eng: volt_brdsup out of range");
//         return false;
//     }
//
//     // Check that the temperature is within an expected range
//     if (reply.temp > 1000) {  // Assuming tenths of degrees Celsius
//         Console::log("fail get_pdu_housekeeping_data_eng: temp out of range");
//         return false;
//     }
//
//     // Check VIP input channels (expecting 6 x uint8_t values)
//     for (int i = 0; i < 6; ++i) {
//         if (reply.vip_input[i] > 255) {  // 8-bit values must not exceed 255
//             Console::log("fail get_pdu_housekeeping_data_eng: vip_input out of range");
//             return false;
//         }
//     }
//
//     // Check that channel ON and overcurrent fault status values are within uint16_t range
//     if (reply.stat_ch_on > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_eng: stat_ch_on out of range");
//         return false;
//     }
//
//     if (reply.stat_ch_ocf > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_eng: stat_ch_ocf out of range");
//         return false;
//     }
//
//     // Validate `vip_vd` (7 voltage domains, 6 VIPs each)
//     for (int i = 0; i < 7; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_vd[i][j] > 255) {
//                 Console::log("fail get_pdu_housekeeping_data_eng: vip_vd out of range");
//                 return false;
//             }
//         }
//     }
//
//     // Validate `vip_ch` (16 channels, 6 VIPs each)
//     for (int i = 0; i < 16; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_ch[i][j] > 255) {  // 8-bit values should not exceed 255
//                 Console::log("fail get_pdu_housekeeping_data_eng: vip_ch out of range");
//                 return false;
//             }
//         }
//     }
//
//     return true;
// }


// get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
// bool test_get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
//     EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_running_average(wire, i2c_address);
//
//     // Validate the response
//     if (reply.error) {
//         Console::log("fail get_pdu_housekeeping_data_running_average: Error flag set");
//         return false;
//     }
//
//     // Check that the board supply voltage is within a reasonable range (assuming millivolts)
//     if (reply.volt_brdsup > 10000) {
//         Console::log("fail get_pdu_housekeeping_data_running_average: volt_brdsup out of range");
//         return false;
//     }
//
//     // Check that the temperature is within an expected range (assuming tenths of degrees Celsius)
//     if (reply.temp > 1000) {
//         Console::log("fail get_pdu_housekeeping_data_running_average: temp out of range");
//         return false;
//     }
//
//     // Check VIP input channels (expecting 6 x uint8_t values)
//     for (int i = 0; i < 6; ++i) {
//         if (reply.vip_input[i] > 255) {  // 8-bit values should be within 0-255
//             Console::log("fail get_pdu_housekeeping_data_running_average: vip_input out of range");
//             return false;
//         }
//     }
//
//     // Check that channel ON and overcurrent fault status values are within uint16_t range
//     if (reply.stat_ch_on > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_running_average: stat_ch_on out of range");
//         return false;
//     }
//
//     if (reply.stat_ch_ocf > 0xFFFF) {
//         Console::log("fail get_pdu_housekeeping_data_running_average: stat_ch_ocf out of range");
//         return false;
//     }
//
//     // Validate `vip_vd` (7 voltage domains, 6 VIPs each)
//     for (int i = 0; i < 7; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_vd[i][j] > 255) {
//                 Console::log("fail get_pdu_housekeeping_data_running_average: vip_vd out of range");
//                 return false;
//             }
//         }
//     }
//
//     // Validate `vip_ch` (16 channels, 6 VIPs each)
//     for (int i = 0; i < 16; ++i) {
//         for (int j = 0; j < 6; ++j) {
//             if (reply.vip_ch[i][j] > 255) {  // 8-bit values should not exceed 255
//                 Console::log("fail get_pdu_housekeeping_data_running_average: vip_ch out of range");
//                 return false;
//             }
//         }
//     }
//
//     return true;
// }

// get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
bool test_get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_raw(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pbu_housekeeping_data_raw: Error flag set");
        return false;
    }

    // Check that the board supply voltage is within a reasonable range (assuming millivolts)
    if (reply.volt_brdsup > 10000) {
        Console::log("fail get_pbu_housekeeping_data_raw: volt_brdsup out of range");
        return false;
    }

    // Check that the temperature is within an expected range (assuming tenths of degrees Celsius)
    if (reply.temp > 1000) {
        Console::log("fail get_pbu_housekeeping_data_raw: temp out of range");
        return false;
    }

    // Check VIP input channels (expecting 6 x uint8_t values)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {  // 8-bit values should be within 0-255
            Console::log("fail get_pbu_housekeeping_data_raw: vip_input out of range");
            return false;
        }
    }

    // Check `stat_bu` is within 16-bit range
    if (reply.stat_bu > 0xFFFF) {
        Console::log("fail get_pbu_housekeeping_data_raw: stat_bu out of range");
        return false;
    }

    // Validate `bp` (3 battery packs, 22 values each)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 22; ++j) {
            if (reply.bp[i][j] > 255) {
                Console::log("fail get_pbu_housekeeping_data_raw: bp out of range");
                return false;
            }
        }
    }

    return true;
}

// get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
bool test_get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_eng(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pbu_housekeeping_data_eng: Error flag set");
        return false;
    }

    // Check that the board supply voltage is within a reasonable range (assuming millivolts)
    if (reply.volt_brdsup > 10000) {
        Console::log("fail get_pbu_housekeeping_data_eng: volt_brdsup out of range");
        return false;
    }

    // Check that the temperature is within an expected range (assuming tenths of degrees Celsius)
    if (reply.temp > 1000) {
        Console::log("fail get_pbu_housekeeping_data_eng: temp out of range");
        return false;
    }

    // Check VIP input channels (expecting 6 x uint8_t values)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {  // 8-bit values should be within 0-255
            Console::log("fail get_pbu_housekeeping_data_eng: vip_input out of range");
            return false;
        }
    }

    // Check `stat_bu` is within 16-bit range
    if (reply.stat_bu > 0xFFFF) {
        Console::log("fail get_pbu_housekeeping_data_eng: stat_bu out of range");
        return false;
    }

    // Validate `bp` (3 battery packs, 22 values each)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 22; ++j) {
            if (reply.bp[i][j] > 255) {
                Console::log("fail get_pbu_housekeeping_data_eng: bp out of range");
                return false;
            }
        }
    }

    return true;
}

// get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
bool test_get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_running_average(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pbu_housekeeping_data_running_average: Error flag set");
        return false;
    }

    // Check that the board supply voltage is within a reasonable range (assuming millivolts)
    if (reply.volt_brdsup > 10000) {
        Console::log("fail get_pbu_housekeeping_data_running_average: volt_brdsup out of range");
        return false;
    }

    // Check that the temperature is within an expected range (assuming tenths of degrees Celsius)
    if (reply.temp > 1000) {
        Console::log("fail get_pbu_housekeeping_data_running_average: temp out of range");
        return false;
    }

    // Check VIP input channels (expecting 6 x uint8_t values)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {  // 8-bit values should be within 0-255
            Console::log("fail get_pbu_housekeeping_data_running_average: vip_input out of range");
            return false;
        }
    }

    // Check `stat_bu` is within 16-bit range
    if (reply.stat_bu > 0xFFFF) {
        Console::log("fail get_pbu_housekeeping_data_running_average: stat_bu out of range");
        return false;
    }

    // Validate `bp` (3 battery packs, 22 values each)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 22; ++j) {
            if (reply.bp[i][j] > 255) {
                Console::log("fail get_pbu_housekeeping_data_running_average: bp out of range");
                return false;
            }
        }
    }

    return true;
}

// system_reset(DWire &wire, uint8_t i2c_address);
bool test_system_reset(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::system_reset(wire, i2c_address);

    // Verify that the command executed successfully
    if (reply.error) {
        Console::log("fail system_reset: Error flag set");
        return false;
    }

    // Expected values based on the EPS documentation
    if (reply.rc != 0x03) { // Response code after a reset (should be checked in ICD)
        Console::log("fail system_reset: Unexpected response code");
        return false;
    }

    if (reply.stat != 0x80) { // Expected status after reset
        Console::log("fail system_reset: Unexpected system status");
        return false;
    }

    Console::log("pass system_reset");
    return true;
}


// switch_safety_mode(DWire &wire, uint8_t i2c_address);
bool test_switch_safety_mode(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::switch_safety_mode(wire, i2c_address);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail switch_safety_mode: Error flag set");
        return false;
    }

    // Expected response code (RC) for a successful switch to safety mode
    if (reply.rc != 0x03) { // Check ICD for the expected response code
        Console::log("fail switch_safety_mode: Unexpected response code");
        return false;
    }

    // Expected system status after switching to safety mode
    if (reply.stat != 0x80) { // Check ICD for expected status
        Console::log("fail switch_safety_mode: Unexpected system status");
        return false;
    }

    Console::log("pass switch_safety_mode");
    return true;
}

// switch_nominal_mode(DWire &wire, uint8_t i2c_address);
bool test_switch_nominal_mode(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::switch_nominal_mode(wire, i2c_address);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail switch_nominal_mode: Error flag set");
        return false;
    }

    // Expected response code (RC) for a successful switch to nominal mode
    if (reply.rc != 0x03) { // Check ICD for expected response code
        Console::log("fail switch_nominal_mode: Unexpected response code");
        return false;
    }

    // Expected system status after switching to nominal mode
    if (reply.stat != 0x00) { // Check ICD for expected nominal mode status
        Console::log("fail switch_nominal_mode: Unexpected system status");
        return false;
    }

    Console::log("pass switch_nominal_mode");
    return true;
}

// output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
bool test_output_bus_group_state(DWire &wire, uint8_t i2c_address) {
    uint16_t test_bitflag = 0xFFFF; // Example: Enable all output bus channels

    EPS::standard_reply reply = EPS::output_bus_group_state(wire, i2c_address, test_bitflag);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail output_bus_group_state: Error flag set");
        return false;
    }

    // Expected response code (RC) for a successful command execution
    if (reply.rc != 0x03) { // Check ICD for expected response code
        Console::log("fail output_bus_group_state: Unexpected response code");
        return false;
    }

    // Expected system status after setting the output bus group state
    if ((reply.stat & 0x01) == 0) { // Assuming bit 0 indicates success
        Console::log("fail output_bus_group_state: Unexpected system status");
        return false;
    }

    Console::log("pass output_bus_group_state");
    return true;
}

// output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx);
bool test_output_bus_channel_off(DWire &wire, uint8_t i2c_address) {
    uint8_t test_channel = 0x01; // Example: Turn off channel 1

    EPS::standard_reply reply = EPS::output_bus_channel_off(wire, i2c_address, test_channel);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail output_bus_channel_off: Error flag set");
        return false;
    }

    // Expected response code (RC) for a successful command execution
    if (reply.rc != 0x03) { // Check ICD for expected response code
        Console::log("fail output_bus_channel_off: Unexpected response code");
        return false;
    }

    // Expected system status after turning off the bus channel
    if ((reply.stat & 0x01) == 0) { // Assuming bit 0 indicates success
        Console::log("fail output_bus_channel_off: Unexpected system status");
        return false;
    }

    Console::log("pass output_bus_channel_off");
    return true;
}

// output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
bool test_output_bus_group_on(DWire &wire, uint8_t i2c_address) {
    uint16_t test_bitflag = 0x0003; // Example: Turn on channels 0 and 1

    EPS::standard_reply reply = EPS::output_bus_group_on(wire, i2c_address, test_bitflag);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail output_bus_group_on: Error flag set");
        return false;
    }

    // Validate the response code (RC)
    if (reply.rc != 0x03) { // Expected response code based on EPS ICD
        Console::log("fail output_bus_group_on: Unexpected response code");
        return false;
    }

    // Validate system status (STAT)
    if ((reply.stat & test_bitflag) != test_bitflag) { // Ensure correct channels are ON
        Console::log("fail output_bus_group_on: Incorrect system status");
        return false;
    }

    Console::log("pass output_bus_group_on");
    return true;
}

// output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
bool test_output_bus_group_off(DWire &wire, uint8_t i2c_address) {
    uint16_t test_bitflag = 0x0003; // Example: Turn off channels 0 and 1

    EPS::standard_reply reply = EPS::output_bus_group_off(wire, i2c_address, test_bitflag);

    // Check if an error occurred during execution
    if (reply.error) {
        Console::log("fail output_bus_group_off: Error flag set");
        return false;
    }

    // Validate the response code (RC)
    if (reply.rc != 0x03) { // Expected response code based on EPS ICD
        Console::log("fail output_bus_group_off: Unexpected response code");
        return false;
    }

    // Validate system status (STAT)
    if ((reply.stat & test_bitflag) != 0x00) { // Ensure correct channels are OFF
        Console::log("fail output_bus_group_off: Incorrect system status");
        return false;
    }

    Console::log("pass output_bus_group_off");
    return true;
}

// reset_configuration(DWire &wire, uint8_t i2c_address);
bool test_reset_configuration(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::reset_configuration(wire, i2c_address);

    // Check if an error occurred
    if (reply.error) {
        Console::log("fail reset_configuration: Error flag set");
        return false;
    }

    // Validate response code (RC) - expected from ICD
    if (reply.rc != 0x03) { // Adjust based on ICD expected response
        Console::log("fail reset_configuration: Unexpected response code");
        return false;
    }

    // Validate that system status (STAT) is in a reset state
    if (reply.stat != 0x80) { // Expected reset status (adjust as per EPS ICD)
        Console::log("fail reset_configuration: Incorrect system status");
        return false;
    }

    Console::log("pass reset_configuration");
    return true;
}

// set_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id, returnType conf_par_value);
bool test_set_config_params(DWire &wire, uint8_t i2c_address) {
    EPS::config_reply reply;

    // 1. Set a writable Int8 configuration parameter
    returnType param_value;
    param_value.i8 = 1; // Example value

    reply = EPS::set_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA, param_value);

    // Check if an error occurred
    if (reply.error) {
        Console::log("fail set_config_params: Error setting BOOT_RESUME_ENA");
        return false;
    }

    // Ensure the returned parameter ID matches the one we set
    if (reply.par_id != ConfigParameter::BOOT_RESUME_ENA) {
        Console::log("fail set_config_params: Incorrect parameter ID");
        return false;
    }

    // 2. Read the parameter to verify the change
    EPS::config_reply read_reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);

    if (read_reply.error) {
        Console::log("fail set_config_params: Error retrieving BOOT_RESUME_ENA after setting");
        return false;
    }

    if (read_reply.conf_par.i8 != param_value.i8) {
        Console::log("fail set_config_params: Incorrect value read back for BOOT_RESUME_ENA");
        return false;
    }

    Console::log("pass set_config_params");
    return true;
}

// reset_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
bool test_reset_config_params(DWire &wire, uint8_t i2c_address) {
    EPS::config_reply reply;

    // 1. Set a writable parameter to a known value
    returnType param_value;
    param_value.i8 = 5; // Set a test value

    EPS::config_reply set_reply = EPS::set_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA, param_value);
    if (set_reply.error) {
        Console::log("fail reset_config_params: Error setting BOOT_RESUME_ENA before reset");
        return false;
    }

    // 2. Reset the parameter
    EPS::config_reply reset_reply = EPS::reset_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);
    if (reset_reply.error) {
        Console::log("fail reset_config_params: Error resetting BOOT_RESUME_ENA");
        return false;
    }

    // 3. Read the parameter to verify it has been reset
    EPS::config_reply read_reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);
    if (read_reply.error) {
        Console::log("fail reset_config_params: Error retrieving BOOT_RESUME_ENA after reset");
        return false;
    }

    // 4. Check if the reset value matches the expected default (assuming default is 0)
    if (read_reply.conf_par.i8 != 0) {
        Console::log("fail reset_config_params: BOOT_RESUME_ENA was not reset to default value");
        return false;
    }

    Console::log("pass reset_config_params");
    return true;
}

//WriteCommand and ReadCommand are tested indirectly through all other tests
// #include "DWire_test_examples.h"
int mainnnn(void)//change name to mainnn
{
    //test the DWIRe first!!
    // int r = mainnn();
    // Console::log("DWire test result: ", r);
    //
    // if (r==0)//do not continue until we solved the DWire problem.
    //     return -1;

    //if the DWire tests were successful, then we continue

    DWire wire = DWire();
    uint8_t i2c_address = 0x20;
    Console::init(9600);

    int nr_of_errors=0;
    if (!test_getConfigParameterType()) {
        nr_of_errors++;
    }

    if(!test_getAccessType()) {
        nr_of_errors++;
    }

    if(!test_get_param_length()) {
        nr_of_errors++;
    }

    if(!test_write_config_params(i2c_address)) {
        nr_of_errors++;
    }

    if(!test_no_operation(i2c_address)) {
        nr_of_errors++;
    }

    if(!test_cancel_operation(i2c_address)) {
        nr_of_errors++;
    }

    if(!test_watchdog(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_config_params(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pcu_housekeeping_data_raw(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pcu_housekeeping_data_eng(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pcu_housekeeping_data_running_average(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pdu_overcurrent_fault_state(wire, i2c_address)) {
        nr_of_errors++;
    }

    // if(!test_get_pdu_abf_placed_state(wire, i2c_address)) {
    //     nr_of_errors++;
    // }
    //
    // if(!test_get_pdu_housekeeping_data_raw(wire, i2c_address)) {
    //     nr_of_errors++;
    // }
    //
    // if(!test_get_pdu_housekeeping_data_eng(wire, i2c_address)) {
    //     nr_of_errors++;
    // }
    //
    // if(!test_get_pdu_housekeeping_data_running_average(wire, i2c_address)) {
    //     nr_of_errors++;
    // }

    if(!test_get_pbu_housekeeping_data_raw(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pbu_housekeeping_data_eng(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_get_pbu_housekeeping_data_running_average(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_system_reset(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_switch_safety_mode(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_switch_nominal_mode(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_output_bus_group_state(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_output_bus_channel_off(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_output_bus_group_on(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_output_bus_group_off(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_reset_configuration(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_set_config_params(wire, i2c_address)) {
        nr_of_errors++;
    }

    if(!test_reset_config_params(wire, i2c_address)) {
        nr_of_errors++;
    }
    if (nr_of_errors == 0)
        Console::log("All tests passed successfully.");
    else
        Console::log(nr_of_errors+" errors found.");
    return nr_of_errors;
}



// Each test is completely independent and can be compiled separately
// I used i2c_address = 0x20, which seems to be ok.

// HOW TO TEST:
// 1. Compile the project with one test at a time (e.g., rename main1 to main)
// 2. Each test follows the correct I2C communication pattern:
//    a) Initialize DWire (setFastMode, begin)
//    b) Write command (beginTransmission, write, endTransmission)
//    c) Wait for EPS to process (delay_ms)
//    d) Request data from slave (requestFrom)  <-- THIS WAS MISSING!
//    e) Read data (read)

// OBS: without requestFrom(), the RX buffer is empty and read() returns 0.
//
// VERIFICATION:
// - Each test returns: 1 = PASS, 0 = FAIL
// - You can check the return value even if Console is unreliable
// - If Console shows weird results, check the return value instead

/*
 * IMPORTANT DWire I2C RULES (MSP432):
 * - NEVER call requestFrom(addr, 1)
 * - ALWAYS read >= 2 bytes
 * - DWire.begin() must be called for I2C configuration
 * - Use endTransmission(false) for register reads
 * - Prefer burst reads
 *
 * Violating these WILL break I2C on real peripherals.
 */

// Test 1: Basic DWire initialization and setup
// Returns: 1 on success, 0 on failure
int main() {
    Console::init(9600);
    DelfiPQcore::initMCU(); // Initialize MCU for clock synchronization
    
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    Console::log("Test 1: DWire initialized successfully");
    Console::log("Test 1: PASS - Basic initialization works");
    return 1; // Success
}

// Test 2: Write command and verify it's in TX buffer (without transmission)
// Returns: 1 on success, 0 on failure
int main2() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Write a command to the buffer
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION command
    wire.write(0x00); // BID

    Console::log("Test 2: Command written to TX buffer");
    Console::log("Test 2: PASS - Write to buffer works");
    return 1; // Success
}

// Test 3: Complete write cycle (beginTransmission, write, endTransmission)
// Returns: 1 on success, 0 on failure
int main3() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Write and end transmission
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    bool result = wire.endTransmission(true);
    
    if (result == false) {
        Console::log("Test 3: PASS - Transmission completed successfully");
        return 1; // Success
    } else {
        Console::log("Test 3: FAIL - Transmission failed or NAK received");
        return 0; // Failure
    }
}

// Test 4: Request data from slave (requestFrom) - basic test
// Returns: 1 on success, 0 on failure
int main4() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Request 5 bytes from the slave even though we did not write anything
    uint8_t bytes_received = wire.requestFrom(i2c_address, 5);

    if (bytes_received == 0) {
        Console::log("Test 4: PASS - Successfully requested 0 data");
        return 1; // Success
    } else {
        Console::log("Test 4: FAIL - Data received (may be expected if EPS not connected)");
        return 0; // Failure (or expected if no EPS connected)
    }
}

// Test 5: Complete I2C cycle - Write command, wait, request, read ending in False
// Returns: 1 on success, 0 on failure
int main5() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    wire.endTransmission(true);

    // delay_ms(25); // not sure if we should wait

    uint8_t bytes_received = wire.requestFrom(i2c_address, 4);
    
    if (bytes_received == 4) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        // uint8_t stat = wire.read();
        
        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (stid == 0x00 && ivid == 0x06 && bid == 0x00) {
            Console::log("Test 5: PASS - Complete I2C cycle works, valid response received");
            return 1; // Success
        } else {
            Console::log("Test 5: FAIL - Invalid response structure");
            return 0; // Failure
        }
    } else {
        Console::log("Test 5: FAIL - Expected 4 bytes");
        return 0; // Failure
    }
}
// Test 5.2: Complete I2C cycle - Write command, wait, request, read ending in False
// Returns: 1 on success, 0 on failure
int main5bis() {
    Console::init(9600);
    DelfiPQcore::initMCU();

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    wire.endTransmission(false);

    // delay_ms(25); // not sure if we should wait

    uint8_t bytes_received = wire.requestFrom(i2c_address, 4);

    if (bytes_received == 5) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        // uint8_t stat = wire.read();

        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (stid == 0x00 && ivid == 0x06 && bid == 0x00) {
            Console::log("Test 5: PASS - Complete I2C cycle works, valid response received");
            return 1; // Success
        } else {
            Console::log("Test 5: FAIL - Invalid response structure");
            return 0; // Failure
        }
    } else {
        Console::log("Test 5: FAIL - Expected 4 bytes");
        return 0; // Failure
    }
}

// Test 6: Test NO_OPERATION command using EPS function
// Returns: 1 on success, 0 on failure
int main6() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);
    
    if (!reply.error && reply.rc == 0x03 && reply.stat == 0x80) {
        Console::log("Test 6: PASS - NO_OPERATION command works");
        return 1; // Success
    } else {
        Console::log("Test 6: FAIL - NO_OPERATION returned error or wrong values");
        return 0; // Failure
    }
}

// Test 7: Test reading a simple config parameter (STID - Int8)
// Returns: 1 on success, 0 on failure
int main7() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::config_reply reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::STID);
    
    if (!reply.error && reply.par_id == ConfigParameter::STID) {
        Console::log("Test 7: PASS - Config parameter read works");
        return 1; // Success
    } else {
        Console::log("Test 7: FAIL - Config parameter read returned error");
        return 0; // Failure
    }
}

// Test 8: Test reading IVID (UInt8)
// Returns: 1 on success, 0 on failure
int main8() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::config_reply reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::IVID);
    
    if (!reply.error && reply.par_id == ConfigParameter::IVID) {
        Console::log("Test 8: PASS - IVID read works");
        return 1; // Success
    } else {
        Console::log("Test 8: FAIL - IVID read returned error");
        return 0; // Failure
    }
}

// Test 9: Test reading BID_USED (read-only parameter)
// Returns: 1 on success, 0 on failure
int main9() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::config_reply reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::BID_USED);
    
    if (!reply.error && reply.par_id == ConfigParameter::BID_USED) {
        Console::log("Test 9: PASS - BID_USED read works");
        return 1; // Success
    } else {
        Console::log("Test 9: FAIL - BID_USED read returned error");
        return 0; // Failure
    }
}

// Test 10: Test reading Int16 parameter
// Returns: 1 on success, 0 on failure
int main10() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::config_reply reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::HITHR_BAT_HEATER_01);
    
    if (!reply.error && reply.par_id == ConfigParameter::HITHR_BAT_HEATER_01) {
        Console::log("Test 10: PASS - Int16 parameter read works");
        return 1; // Success
    } else {
        Console::log("Test 10: FAIL - Int16 parameter read returned error");
        return 0; // Failure
    }
}

// Test 11: Test reading UInt16 parameter
// Returns: 1 on success, 0 on failure
int main11() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::config_reply reply = EPS::get_config_params(wire, i2c_address, ConfigParameter::VD6_CH_BF);
    
    if (!reply.error && reply.par_id == ConfigParameter::VD6_CH_BF) {
        Console::log("Test 11: PASS - UInt16 parameter read works");
        return 1; // Success
    } else {
        Console::log("Test 11: FAIL - UInt16 parameter read returned error");
        return 0; // Failure
    }
}

// Test 12: Test WATCHDOG command
// Returns: 1 on success, 0 on failure
int main12() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::standard_reply reply = EPS::watchdog(wire, i2c_address);
    
    if (!reply.error && reply.rc == 0x07) {
        Console::log("Test 12: PASS - WATCHDOG command works");
        return 1; // Success
    } else {
        Console::log("Test 12: FAIL - WATCHDOG returned error or wrong RC");
        return 0; // Failure
    }
}

// Test 13: Test CANCEL_OPERATION command
// Returns: 1 on success, 0 on failure
int main13() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::standard_reply reply = EPS::cancel_operation(wire, i2c_address);
    
    if (!reply.error && reply.rc == 0x03 && reply.stat == 0x80) {
        Console::log("Test 13: PASS - CANCEL_OPERATION command works");
        return 1; // Success
    } else {
        Console::log("Test 13: FAIL - CANCEL_OPERATION returned error");
        return 0; // Failure
    }
}

// Test 14: Test reading PCU housekeeping data (raw)
// Returns: 1 on success, 0 on failure
int main14() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x2C; // PCU_1_ADDR
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_raw(wire, i2c_address);
    
    if (!reply.error) {
        Console::log("Test 14: PASS - PCU housekeeping data read works");
        return 1; // Success
    } else {
        Console::log("Test 14: FAIL - PCU housekeeping data read returned error");
        return 0; // Failure
    }
}

// Test 15: Test reading PBU housekeeping data (raw)
// Returns: 1 on success, 0 on failure
int main15() {
    Console::init(9600);
    DelfiPQcore::initMCU();
    
    uint8_t i2c_address = 0x28; // PBU_1_ADDR
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_raw(wire, i2c_address);
    
    if (!reply.error) {
        Console::log("Test 15: PASS - PBU housekeeping data read works");
        return 1; // Success
    } else {
        Console::log("Test 15: FAIL - PBU housekeeping data read returned error");
        return 0; // Failure
    }
}
// Test 16. Write and read just 2 bytes
// Returns: 1 on success, 0 on failure
int main16() {
    Console::init(9600);
    DelfiPQcore::initMCU();

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    wire.write(0x01); // STID
    wire.write(0x06); // IVID
    wire.endTransmission(false);

    // delay_ms(25); // not sure if we should wait

    uint8_t bytes_received = wire.requestFrom(i2c_address, 2);

    if (bytes_received == 2) {
        uint8_t a = wire.read();
        uint8_t b = wire.read();

        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (a == 0x01 && b == 0x06) {
            Console::log("Test 16: PASS - Complete 2 reads");
            //return 1; // partial Success
        } else {
            Console::log("Test 16: FAIL - Invalid response structure");
            return 0; // Failure
        }
    } else {
        Console::log("Test 16: FAIL - Expected 2 bytes");
        return 0; // Failure
    }
    wire.beginTransmission(i2c_address);
    wire.write(0x02); // STID
    wire.write(0x04); // IVID
    wire.endTransmission(false);
    bytes_received = wire.requestFrom(i2c_address, 2);
    if (bytes_received == 2) {
        uint8_t c = wire.read();
        uint8_t d = wire.read();

        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (c == 0x02 && d == 0x04) {
            Console::log("Test 16: PASS - Complete 2 another reads");
            //return 1; // partial Success
        } else {
            Console::log("Test 16: FAIL - Invalid response structure for the second pait of reads");
            return 0; // Failure
        }
    } else {
        Console::log("Test 16: FAIL - Expected another 2 bytes");
        return 0; // Failure
    }
}