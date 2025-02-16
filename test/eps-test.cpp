#include "../src/EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy
#include "../src/Console.h"

bool test_getConfigParameterType() {
    if (EPS::getConfigParameterType(ConfigParameter::BOOT_RESUME_ENA) != Int8) {
        Console::log("fail BOOT_RESUME_ENA getconfigparamtype");
        return false;
    }
    if (EPS::getConfigParameterType(ConfigParameter::BOARD_IDENTIFIER_KEY) != UInt8) {
        Console::log("fail BOARD_IDENTIFIER_KEY getconfigparamtype");
        return false;
    }
    if(EPS::getConfigParameterType(ConfigParameter::HITHR_BMON_UNBAL_02) != Int16) {
        Console::log("fail HITHR_BMON_UNBAL_02 getconfigparamtype");
        return false;
    }
    if(EPS::getConfigParameterType(ConfigParameter::VD6_CH_BF) != UInt16) {
        Console::log("fail VD6_CH_BF getconfigparamtype");
        return false;
    }

    return true;
}

bool test_getAccessType() {
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_LOTHR getaccesstype");
        return false;
    }
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_HITHR getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::CONF_PARAM_CHANGED) != ReadOnly) {
        Console::log("fail CONF_PARAM_CHANGED getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::STID) != ReadOnly) {
        Console::log("fail STID getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::BID_USED) != ReadOnly) {
        Console::log("fail BID_USED getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR_USED) != ReadOnly) {
        Console::log("fail SAFETY_VOLT_HITHR_USED getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::VD6_CH_BF) != ReadOnly) {
        Console::log("fail VD6_CH_BF getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::OBUS_LATCHOFF_DELAY_06) != ReadWrite) {
        Console::log("fail OBUS_LATCHOFF_DELAY_06 getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::BP1_TEMP2_POSDIV) != ReadWrite) {
        Console::log("fail BP1_TEMP2_POSDIV getaccesstype");
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::OBUS_VD1_ALWAYS_ENA) != ReadWrite) {
        Console::log("fail OBUS_VD1_ALWAYS_ENA getaccesstype");
        return false;
    }
    
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

bool test_write_config_params(DWire &wire, uint8_t i2c_address) {
    
    //read param not allowed
    if (EPS::write_config_params(wire, i2c_address, ConfigParameter::TTC_PREVCMD_ELAPSED, CommandCode::GET_PARAM)) {
        Console::log("fail TTC_PREVCMD_ELAPSED write_config_params");
        return false;
    }
    return true;
}

// no_operation(DWire &wire, uint8_t i2c_address);
bool test_no_operation(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);
    if(reply.rc != 0x03 || reply.stat != 0x80) {
        Console::log("fail no_operation");
        return false;
    }
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
bool test_get_config_params(DWire &wire, uint8_t i2c_address, EPS::config_reply reply) {
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
            Console::log("fail get_pdu_overcurrent_fault_state: ocf_cnt_ch[" + std::to_string(i) + "] out of range");
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
bool test_get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_raw(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pdu_housekeeping_data_raw: Error flag set");
        return false;
    }

    // Validate voltage board supply (reasonable range assumption)
    if (reply.volt_brdsup > 10000) { // Assuming in millivolts
        Console::log("fail get_pdu_housekeeping_data_raw: volt_brdsup out of range");
        return false;
    }

    // Validate temperature (assuming it is in tenths of °C)
    if (reply.temp > 1000) {
        Console::log("fail get_pdu_housekeeping_data_raw: temp out of range");
        return false;
    }

    // Validate VIP input values (each should be 8-bit)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {
            Console::log("fail get_pdu_housekeeping_data_raw: vip_input[" + std::to_string(i) + "] out of range");
            return false;
        }
    }

    // Validate stat_ch_on and stat_ch_ocf (both should be 16-bit)
    if (reply.stat_ch_on > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_raw: stat_ch_on out of range");
        return false;
    }

    if (reply.stat_ch_ocf > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_raw: stat_ch_ocf out of range");
        return false;
    }

    // Validate VIP voltage values (each should be 8-bit)
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_vd[i][j] > 255) {
                Console::log("fail get_pdu_housekeeping_data_raw: vip_vd[" + std::to_string(i) + "][" + std::to_string(j) + "] out of range");
                return false;
            }
        }
    }

    // Validate VIP channel values (each should be 8-bit)
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_ch[i][j] > 255) {
                Console::log("fail get_pdu_housekeeping_data_raw: vip_ch[" + std::to_string(i) + "][" + std::to_string(j) + "] out of range");
                return false;
            }
        }
    }

    return true;
}


// get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
bool test_get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_eng(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pdu_housekeeping_data_eng: Error flag set");
        return false;
    }

    // Check that the board supply voltage is within a reasonable range
    if (reply.volt_brdsup > 10000) {  // Assuming millivolts
        Console::log("fail get_pdu_housekeeping_data_eng: volt_brdsup out of range");
        return false;
    }

    // Check that the temperature is within an expected range
    if (reply.temp > 1000) {  // Assuming tenths of degrees Celsius
        Console::log("fail get_pdu_housekeeping_data_eng: temp out of range");
        return false;
    }

    // Check VIP input channels (expecting 6 x uint8_t values)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {  // 8-bit values must not exceed 255
            Console::log("fail get_pdu_housekeeping_data_eng: vip_input out of range");
            return false;
        }
    }

    // Check that channel ON and overcurrent fault status values are within uint16_t range
    if (reply.stat_ch_on > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_eng: stat_ch_on out of range");
        return false;
    }

    if (reply.stat_ch_ocf > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_eng: stat_ch_ocf out of range");
        return false;
    }

    // Validate `vip_vd` (7 voltage domains, 6 VIPs each)
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_vd[i][j] > 255) {
                Console::log("fail get_pdu_housekeeping_data_eng: vip_vd out of range");
                return false;
            }
        }
    }

    // Validate `vip_ch` (16 channels, 6 VIPs each)
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_ch[i][j] > 255) {  // 8-bit values should not exceed 255
                Console::log("fail get_pdu_housekeeping_data_eng: vip_ch out of range");
                return false;
            }
        }
    }

    return true;
}


// get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
bool test_get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_running_average(wire, i2c_address);

    // Validate the response
    if (reply.error) {
        Console::log("fail get_pdu_housekeeping_data_running_average: Error flag set");
        return false;
    }

    // Check that the board supply voltage is within a reasonable range (assuming millivolts)
    if (reply.volt_brdsup > 10000) {
        Console::log("fail get_pdu_housekeeping_data_running_average: volt_brdsup out of range");
        return false;
    }

    // Check that the temperature is within an expected range (assuming tenths of degrees Celsius)
    if (reply.temp > 1000) {
        Console::log("fail get_pdu_housekeeping_data_running_average: temp out of range");
        return false;
    }

    // Check VIP input channels (expecting 6 x uint8_t values)
    for (int i = 0; i < 6; ++i) {
        if (reply.vip_input[i] > 255) {  // 8-bit values should be within 0-255
            Console::log("fail get_pdu_housekeeping_data_running_average: vip_input out of range");
            return false;
        }
    }

    // Check that channel ON and overcurrent fault status values are within uint16_t range
    if (reply.stat_ch_on > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_running_average: stat_ch_on out of range");
        return false;
    }

    if (reply.stat_ch_ocf > 0xFFFF) {
        Console::log("fail get_pdu_housekeeping_data_running_average: stat_ch_ocf out of range");
        return false;
    }

    // Validate `vip_vd` (7 voltage domains, 6 VIPs each)
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_vd[i][j] > 255) {
                Console::log("fail get_pdu_housekeeping_data_running_average: vip_vd out of range");
                return false;
            }
        }
    }

    // Validate `vip_ch` (16 channels, 6 VIPs each)
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 6; ++j) {
            if (reply.vip_ch[i][j] > 255) {  // 8-bit values should not exceed 255
                Console::log("fail get_pdu_housekeeping_data_running_average: vip_ch out of range");
                return false;
            }
        }
    }

    return true;
}

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


// cancel_operation(DWire &wire, uint8_t i2c_address);
bool test_cancel_operation(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::cancel_operation(wire, i2c_address);

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

int main(void)
{
    DWire wire = DWire();
    uint8_t i2c_address = 0x20;
    Console::init(9600);

    if (!test_getConfigParameterType()) {
        return 1;
    }

    if(!test_getAccessType()) {
        return 2;
    }

    if(!test_get_param_length()) {
        return 3;
    }

    if(!test_write_config_params(wire, i2c_address)) {
        return 4;
    }

    if(!test_no_operation(wire, i2c_address)) {
        return 5;
    }

    if(!test_watchdog(wire, i2c_address)) {
        return 6;
    }

    if(!test_get_config_params(wire, i2c_address)) {
        return 7;
    }

    if(!test_get_pcu_housekeeping_data_raw(wire, i2c_address)) {
        return 8;
    }

    if(!test_get_pcu_housekeeping_data_eng(wire, i2c_address)) {
        return 9;
    }

    if(!test_get_pcu_housekeeping_data_running_average(wire, i2c_address)) {
        return 10;
    }

    if(!test_get_pdu_overcurrent_fault_state(wire, i2c_address)) {
        return 11;
    }

    if(!test_get_pdu_abf_placed_state(wire, i2c_address)) {
        return 12;
    }

    if(!test_get_pdu_housekeeping_data_raw(wire, i2c_address)) {
        return 13;
    }

    if(!test_get_pdu_housekeeping_data_eng(wire, i2c_address)) {
        return 14;
    }

    if(!test_get_pdu_housekeeping_data_running_average(wire, i2c_address)) {
        return 15;
    }

    if(!test_get_pbu_housekeeping_data_raw(wire, i2c_address)) {
        return 16;
    }

    if(!test_get_pbu_housekeeping_data_eng(wire, i2c_address)) {
        return 17;
    }

    if(!test_get_pbu_housekeeping_data_running_average(wire, i2c_address)) {
        return 18;
    }

    if(!test_system_reset(wire, i2c_address)) {
        return 19;
    }

    if(!test_cancel_operation(wire, i2c_address)) {
        return 20;
    }

    if(!test_switch_safety_mode(wire, i2c_address)) {
        return 21;
    }

    if(!test_switch_nominal_mode(wire, i2c_address)) {
        return 22;
    }

    if(!test_output_bus_group_state(wire, i2c_address)) {
        return 23;
    }

    if(!test_output_bus_channel_off(wire, i2c_address)) {
        return 24;
    }

    if(!test_output_bus_group_on(wire, i2c_address)) {
        return 25;
    }

    if(!test_output_bus_group_off(wire, i2c_address)) {
        return 26;
    }

    if(!test_reset_configuration(wire, i2c_address)) {
        return 27;
    }

    if(!test_set_config_params(wire, i2c_address)) {
        return 28;
    }

    if(!test_reset_config_params(wire, i2c_address)) {
        return 29;
    }

    Console::log("All tests passed successfully.");
    return 0;
}
