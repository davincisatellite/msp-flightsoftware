//
// Created by caspar on 6/9/23.
//
#include "EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy

ParameterType EPS::getConfigParameterType(ConfigParameter conf_par) {
    uint16_t value = static_cast<uint16_t>(conf_par);
    uint16_t firstDigit = value >> 12;  // Shift right by 12 bits to isolate the top hex digit
    switch (firstDigit) {
        case 0x1:
            return Int8;
        case 0x2:
            return UInt8;
        case 0x3:
            return Int16;
        case 0x4:
            return UInt16;
        case 0x5:
            return Int32;
        case 0x6:
            return UInt32;
        case 0x7:
            return Float;
        case 0x8:
            return Int64;
        case 0x9:
            return UInt64;
        case 0xA:
            return Double;
        default:
            return Invalid;
    }
}

AccessType EPS::getAccessType(ConfigParameter conf_par) {
    //For some whatever reason these two do not follow the same pattern as the rest (0x?8??)
    if (conf_par == ConfigParameter::SAFETY_VOLT_LOTHR ||
        conf_par == ConfigParameter::SAFETY_VOLT_HITHR) {
        return ReadWrite;
    }

    // Cast to underlying integer type and shift to get the second hex digit
    uint16_t value = static_cast<uint16_t>(conf_par);
    uint16_t secondDigit = (value >> 8) & 0xF;  // Shift right by 8 bits and mask with 0xF

    if (secondDigit == 0x8) { return ReadOnly; }
    return ReadWrite;
}

// The data type determines how many bytes need to be supplied as the PAR_VAL! (page 77 ICD)
uint8_t EPS::get_param_length(ParameterType par_type) {
    switch (par_type) {
        case Int8:
        case UInt8:
            return 1;
        case Int16:
        case UInt16:
            return 2;
        case Int32:
        case UInt32:
        case Float:
            return 4;
        case Int64:
        case UInt64:
        case Double:
            return 8;
        default:
            return 0; // Invalid par_id
    }
}

void EPS::writeCommand(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
}

void EPS::readCommand(DWire &wire, EPS::ReplyBase &reply) {
    reply.stid = wire.read();   // STID
    reply.ivid = wire.read();   // IVID
    reply.rc = wire.read();     // Response code
    reply.bid = wire.read();    // BID
    reply.stat = wire.read();   // Status byte
}

bool EPS::write_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, CommandCode commandCode) {
    ParameterType param_type = EPS::getConfigParameterType(par_id);

    if (param_type == Invalid) { // Invalid config parameter
        return false;
    }

    if (EPS::getAccessType(par_id) == ReadOnly) { // Read-only parameter
        return false;
    }
    //the following if is redundant (we already test if param_type is invalid)
    // Get the expected length of PAR_VAL for the given PAR_ID
    //uint8_t par_val_length = EPS::get_param_length(param_type);
    //if (par_val_length == 0) { // Invalid param_id
    //    return false;
    //}

    EPS::writeCommand(wire, i2c_address, commandCode);

    // Write Param-ID in little-endian format
    uint16_t par_id_as_uint16 = static_cast<uint16_t>(par_id);
    wire.write(par_id_as_uint16 & 0xFF);  // least significant byte of par_id
    wire.write(par_id_as_uint16 >> 8);    // most significant byte of par_id

    return true;
}

EPS::config_reply read_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, EPS::config_reply &reply) {

    // Request 8 bytes + PAR_VAL length of data
    uint8_t par_val_length = EPS::get_param_length(EPS::getConfigParameterType(par_id));
    uint8_t response_length = 8 + par_val_length;
    uint8_t response = wire.requestFrom(i2c_address, response_length);

    // If the response is the expected length, process the reply
    if (response == response_length) {
        EPS::readCommand(wire, reply);

        // Reserved byte (skip)
        (void) wire.read();

        // Param-ID (read in little-endian)
        reply.par_id = static_cast<ConfigParameter>(wire.read() + (wire.read() << 8));

        if (reply.par_id != par_id) {
            // Return immediately if the Param-ID does not match the expected value
            return reply;
        }

        uint8_t buffer[8]; // Buffer to accumulate incoming bytes, max size is 8 (for double, int64, uint64)
        for (size_t i = 0; i < par_val_length; ++i) { // Read the bytes one by one
            buffer[i] = wire.read(); //TODO check whether this is in little endian order
        }

        // Copy the buffer into the reply. TODO check whether this actualy works
        std::memcpy(&reply.conf_par, buffer, par_val_length);

        // Set error code to 0 (success)
        reply.error = false;
    } else {
        // Set error code to 1 (failure)
        reply.error = true;
    }

    return reply;
}

EPS::config_reply EPS::set_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, returnType par_val) {
    // Initialise reply with default value to avoid uninitialised fields
    EPS::config_reply reply = {};
    reply.error = 1;  // Set default error code

    if(!EPS::write_config_params(wire, i2c_address, par_id, CommandCode::SET_PARAM)) {
        return reply;
    }

    // Reinterpret the union as a pointer to bytes
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&par_val); //TODO check whether this actually works

    // Send each byte
    uint8_t par_val_length = EPS::get_param_length(EPS::getConfigParameterType(par_id));
    for (size_t i = 0; i < par_val_length; ++i) { //TODO check whether this is in little endian order
        wire.write(bytePtr[i]);
    }

    // End transmission
    wire.endTransmission();

    // delay for the operation to complete
    delay_ms(25); //TODO investigate this delay. is it too much? Maybe poll instead (scheme 2 from ICD 3.3.4)

    return read_config_params(wire, i2c_address, par_id, reply);
}

EPS::config_reply EPS::reset_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id) {
    // Initialise reply with default value to avoid uninitialised fields
    EPS::config_reply reply = {};
    reply.error = 1;  // Set default error code

    if(!EPS::write_config_params(wire, i2c_address, par_id, CommandCode::RESET_PARAM)) {
        return reply;
    }

    // End transmission
    wire.endTransmission();

    // delay for the operation to complete
    delay_ms(25); //TODO investigate this delay. is it too much? Maybe poll instead (scheme 2 from ICD 3.3.4)

    return read_config_params(wire, i2c_address, par_id, reply);
}

EPS::config_reply EPS::get_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id) {
    // Initialise reply with default value to avoid uninitialised fields
    EPS::config_reply reply = {};
    reply.error = 1;  // Set default error code

    if(!EPS::write_config_params(wire, i2c_address, par_id, CommandCode::GET_PARAM)) {
        return reply;
    }

    // End transmission
    wire.endTransmission();

    // delay for the operation to complete
    delay_ms(25); //TODO investigate this delay. is it too much? Maybe poll instead (scheme 2 from ICD 3.3.4)

    return read_config_params(wire, i2c_address, par_id, reply);
}

EPS::standard_reply EPS::no_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::NO_OPERATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::system_reset(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    EPS::writeCommand(wire, i2c_address, CommandCode::SYSTEM_RESET);
    wire.write(RESET_KEY_SYSTEM_RESET);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::cancel_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::CANCEL_OPERATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::watchdog(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::WATCHDOG);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

// Removed this function decleration without body. Must be reimplemented with ICD page 36
//EPS::standard_reply EPS::output_bus_channel_on(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {

EPS::standard_reply EPS::switch_safety_mode(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_TO_SAFETY_MODE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_overcurrent_reply EPS::get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address) {
    pdu_overcurrent_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::GET_PDU_OVERCURRENT_FAULT_STATE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);

        // read (reserved) and discard
        (void) wire.read();

        // reads in little endian order
        reply.stat_ob_on = wire.read() + (wire.read() << 8);
        reply.stat_ob_ocf = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 16; ++i) {
            reply.ocf_cnt_ch[i] = wire.read() + (wire.read() << 8);
        }

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_abf_placed_state get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address) {
    EPS::pdu_abf_placed_state reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::GET_PDU_ABF_PLACED_STATE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);

        // read (reserved) and discard
        (void) wire.read();

        reply.abf_placed_0 = wire.read();
        reply.abf_placed_1 = wire.read();

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_housekeeping_data_reply get_pdu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    EPS::pdu_housekeeping_data_reply reply;

    EPS::writeCommand(wire, i2c_address, commandCode);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);

        // read (reserved) and discard
        (void) wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 8);
        reply.temp = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 6; ++i) {
            reply.vip_input[i] = wire.read();
        }

        reply.stat_ch_on = wire.read() + (wire.read() << 8);
        reply.stat_ch_ocf = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 7; ++i) {
            for (int j = 0; j < 6; ++j) {
                reply.vip_vd[i][j] = wire.read();
            }
        }

        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < 6; ++j) {
                reply.vip_ch[i][j] = wire.read();
            }
        }

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_housekeeping_data_reply get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_ENG);
}

EPS::pdu_housekeeping_data_reply get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_RAW);
}

EPS::pdu_housekeeping_data_reply get_pdu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_AVG);
}

EPS::pbu_housekeeping_data_reply get_pbu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    EPS::pbu_housekeeping_data_reply reply;

    EPS::writeCommand(wire, i2c_address, commandCode);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);

        // read (reserved) and discard
        (void) wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 8);
        reply.temp = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 6; ++i) {
            reply.vip_input[i] = wire.read();
        }

        reply.stat_bu = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 22; ++j) {
                reply.bp[i][j] = wire.read();
            }
        }

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_ENG);
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_RAW);
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_AVG);
}

EPS::pcu_housekeeping_data_reply get_pcu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    EPS::pcu_housekeeping_data_reply reply;

    EPS::writeCommand(wire, i2c_address, commandCode);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);

        // read (reserved) and discard
        (void) wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 8);
        reply.temp = wire.read() + (wire.read() << 8);

        for (int i = 0; i < 6; ++i) {
            reply.vip_output[i] = wire.read();
        }

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 14; ++j) {
                reply.cc[i][j] = wire.read();
            }
        }

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_ENG);
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW);
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_AVG);
}

EPS::standard_reply EPS::switch_nominal_mode(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_NOMINAL_MODE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_OFF);
    wire.write(ch_idx);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_STATE);

    uint8_t ch_bf[2]; // channel bitfield as two bytes
    ch_bf[0] = (uint8_t) (bitflag >> 8);  // most significant byte
    ch_bf[1] = (uint8_t) (bitflag & 0xFF); // least significant byte

    wire.write(ch_bf[1]);
    wire.write(ch_bf[0]);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_OFF);

    uint8_t ch_bf[2]; // channel bitfield as two bytes
    ch_bf[0] = (uint8_t) (bitflag >> 8);  // most significant byte
    ch_bf[1] = (uint8_t) (bitflag & 0xFF); // least significant byte

    wire.write(ch_bf[1]);
    wire.write(ch_bf[0]);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_ON);

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    wire.write(bytes[1]);
    wire.write(bytes[0]);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::reset_configuration(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::RESET_CONFIGURATION);
    wire.write(CONF_KEY_RESET_CONFIGURATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}