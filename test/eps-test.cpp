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

bool test_no_operation(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);
    if(reply.rc != 0x03 || reply.stat != 0x80) {
        Console::log("fail no_operation");
        return false;
    }
    return true;
}

bool test_watchdog(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::watchdog(wire, i2c_address);
    if(reply.rc != 0x07) {
        Console::log("fail watchdog");
        return false;
    }
    return true;
}

//WriteCommand and ReadCommand are tested indirectly through all other tests


// no_operation(DWire &wire, uint8_t i2c_address);
// system_reset(DWire &wire, uint8_t i2c_address);
// cancel_operation(DWire &wire, uint8_t i2c_address);
// watchdog(DWire &wire, uint8_t i2c_address);
// get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
// get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
// get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
// get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address);
// get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address);
// get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
// get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
// get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
// get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address);
// get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address);
// get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address);
// switch_safety_mode(DWire &wire, uint8_t i2c_address);
// switch_nominal_mode(DWire &wire, uint8_t i2c_address);
// output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx);
// output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
// output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
// output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag);
// reset_configuration(DWire &wire, uint8_t i2c_address);
// get_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
// reset_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
// set_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id, returnType conf_par_value);


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

    return 0;

}
