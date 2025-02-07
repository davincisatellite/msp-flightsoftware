#include "../src/EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy

bool test_getConfigParameterType() {
    if (EPS::getConfigParameterType(ConfigParameter::BOOT_RESUME_ENA) != Int8) {
        return false;
    }
    if (EPS::getConfigParameterType(ConfigParameter::BOARD_IDENTIFIER_KEY) != UInt8) {
        return false;
    }
    if(EPS::getConfigParameterType(ConfigParameter::HITHR_BMON_UNBAL_02) != Int16) {
        return false;
    }
    if(EPS::getConfigParameterType(ConfigParameter::VD6_CH_BF) != UInt16) {
        return false;
    }

    return true;
}

bool test_getAccessType() {
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR) != ReadWrite) {
        return false;
    }
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR) != ReadWrite) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::CONF_PARAM_CHANGED) != ReadOnly) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::STID) != ReadOnly) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::BID_USED) != ReadOnly) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR_USED) != ReadOnly) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::VD6_CH_BF) != ReadOnly) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::OBUS_LATCHOFF_DELAY_06) != ReadWrite) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::BP1_TEMP2_POSDIV) != ReadWrite) {
        return false;
    }
    if(EPS::getAccessType(ConfigParameter::OBUS_VD1_ALWAYS_ENA) != ReadWrite) {
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
    return false;

}

bool test_write_config_params(DWire &wire, uint8_t i2c_address) {
    
    //read param not allowed
    if (EPS::write_config_params(wire, i2c_address, ConfigParameter::TTC_PREVCMD_ELAPSED, CommandCode::GET_PARAM)) {
        return false;
    }
    return true;
}

bool test_no_operation(DWire &wire, uint8_t i2c_address) {
    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);
    if(reply.stid != 0x00 || reply.ivid != 0x06 || reply.rc != 0x03 || reply.stat != 0x80) {
        return false;
    }
    return true;
}

//WriteCommand and ReadCommand are tested indirectly through all other tests


// reset_watchdog(DWire &wire, uint8_t i2c_address);
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

    return 0;

}
