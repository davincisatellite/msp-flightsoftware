//
// Created by caspar on 6/9/23.
//
#include "EPS.h"
#include <cstddef> //for size_t

// Constants for command codes and other identifiers
enum Identifiers {
    STID = 0x00,                                      // reference: page 17 of 87 (ICD)
    IVID = 0x06,                                      // reference: page 19 of 87 (ICD)
    BID = 0x00                                        // reference: page 20 of 87 (ICD)
};

enum class CommandCode {
    SET_PARAM = 0x84,                        // reference: page 64 of 87 (ICD)
    RESET_PARAM = 0x86,                      // reference: page 65 of 87 (ICD)
    WATCHDOG = 0x06,                         // reference: page 32 of 87 (ICD)
    NO_OPERATION = 0x02,                     // reference: page 30 of 87 (ICD)
    SYSTEM_RESET = 0xAA,                     // reference: page 29 of 87 (ICD)
    CANCEL_OPERATION = 0x04,                 // reference: page 31 of 87 (ICD)
    SWITCH_TO_SAFETY_MODE = 0x32,            // reference: page 39 of 87 (ICD)
    GET_PDU_OVERCURRENT_FAULT_STATE = 0x42,  // reference: page 43 of 87 (ICD)
    GET_PBU_HOUSEKEEPING_DATA_RAW = 0x60,    // reference: page 53 of 87 (ICD)
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

// Enum for reset keys or confirmation keys
enum ResetKey {
    RESET_KEY_SYSTEM_RESET = 0xA6,                    // reference: page 29 of 87 (ICD)
    CONF_KEY_RESET_CONFIGURATION = 0xA7               // reference: page 66 of 87 (ICD)
};

bool is_valid_param_id(uint16_t par_id) {
    // List of valid Param-IDs based on Table 3-24: Possible Parameter Data Types from page 77 ICD
    const uint16_t valid_param_ids[] = {
        0x1000,  // int8
        0x2000,  // uint8
        0x3000,  // int16
        0x4000,  // uint16
        0x5000,  // int32
        0x6000,  // uint32
        0x7000,  // float
        0x8000,  // int64
        0x9000,  // uint64
        0xA000   // double
    };

    // Get the number of valid Param-IDs
    const size_t num_valid_params = 10;

    // Check if the given par_id is in the list of valid Param-IDs
    for (size_t i = 0; i < num_valid_params; ++i) {
        if (par_id == valid_param_ids[i]) {
            return true; // Return true if par_id is valid
        }
    }

    return false; // Return false if par_id is not in the list of valid param IDs
}

// The data type determines how many bytes need to be supplied as the PAR_VAL! (page 77 ICD)
uint8_t get_param_length(uint16_t par_id) {
    switch (par_id) {
        case 0x1000: // int8
            return 1;
        case 0x2000: // int8
            return 1;
        case 0x3000: // int16
            return 2;
        case 0x4000: // uint16
            return 2;
        case 0x5000: // int32
            return 4;
        case 0x6000: // uint32
            return 4;
        case 0x7000: // float
            return 4;
        case 0x8000: // int64
            return 8;
        case 0x9000: // uint64
            return 8;
        case 0xA000: // double
            return 8;
        default:
            return 0; // Invalid par_id
    }
}

void writeCommand(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
}

void readCommand(DWire &wire, EPS::ReplyBase &reply) {
    reply.stid = wire.read();   // STID
    reply.ivid = wire.read();   // IVID
    reply.rc = wire.read();     // Response code
    reply.bid = wire.read();    // BID
    reply.stat = wire.read();   // Status byte
}

EPS::config_reply EPS::set_config_params(DWire &wire, uint8_t i2c_address, uint16_t par_id, uint8_t *par_val) {
    // Initialise reply structure
    config_reply reply = {};
    reply.error = 1;  // Set default error code

    // Validate the PAR_ID based on the configuration list
    if (!is_valid_param_id(par_id)) {
        // Invalid PAR_ID; return the reply with default values and error set to 1
        return reply;
    }

    // Get the expected length of PAR_VAL for the given PAR_ID
    uint8_t par_val_length = get_param_length(par_id);
    if (par_val_length == 0) { // Invalid param_id
        return reply;
    }

    // Boundary check to ensure par_val_length does not exceed the size of reply.par_val array
    if (par_val_length > sizeof(reply.par_val)) {
        return reply;  // Return immediately to avoid buffer overflow
    }

    writeCommand(wire, i2c_address, CommandCode::SET_PARAM);

    wire.write(par_id & 0xFF);  // least significant byte of PAR_ID
    wire.write(par_id >> 8);    // most significant byte of PAR_ID

    // Write the PAR_VAL data
    for (uint8_t i = 0; i < par_val_length; ++i) {
        wire.write(par_val[i]);
    }

    // End transmission
    wire.endTransmission();

    // delay for the operation to complete
    delay_ms(25);

    // Get the expected length of PAR_VAL for the given PAR_ID
    //uint16_t par_val_length = get_param_length(par_id);

    // Request 8 bytes + PAR_VAL length of data
    uint8_t response_length = 8 + par_val_length;
    uint8_t response = wire.requestFrom(i2c_address, response_length);

    // If the response is the expected length, process the reply
    if (response == response_length) {
        readCommand(wire, reply);

        // Reserved byte (skip)
        (void) wire.read();

        // Param-ID (read in little-endian)
        reply.par_id = wire.read() + (wire.read() << 8);

        // Read the PAR_VAL
        for (uint8_t i = 0; i < par_val_length; ++i) {
            reply.par_val[i] = wire.read();
        }
        reply.par_val_length = par_val_length;

        // Set error code to 0 (success)
        reply.error = false;
    } else {
        // Set error code to 1 (failure)
        reply.error = true;
    }

    return reply;
}

EPS::config_reply EPS::reset_config_params(DWire &wire, uint8_t i2c_address, uint16_t par_id) {
    // Initialise reply with default value to avoid uninitialised fields
    config_reply reply = {};
    reply.error = 1;


    // Validate the Param-ID based on the configuration list
    if (!is_valid_param_id(par_id)) {
        // Invalid par_id; return the reply with default values and error set to 1
        return reply;
    }

    // Get the expected length of PAR_VAL for the given param ID
    uint8_t par_val_length = get_param_length(par_id);
    if (par_val_length == 0) { // Invalid param_id
        // Return reply with error still set to 1 (already initialised)
        return reply;
    }

    // Boundary check to ensure par_val_length does not exceed the size of reply.par_val array
    if (par_val_length > sizeof(reply.par_val)) {
        // Return immediately to avoid buffer overflow, error is already set to 1
        return reply;
    }

    writeCommand(wire, i2c_address, CommandCode::RESET_PARAM);

    // Write Param-ID in little-endian format
    wire.write(par_id & 0xFF);  // least significant byte of par_id
    wire.write(par_id >> 8);    // most significant byte of par_id

    // End transmission
    wire.endTransmission();

    // delay for the operation to complete
    delay_ms(25);

    // Request 8 bytes + n bytes of PAR_VAL data, assuming max length of PAR_VAL (8 bytes)
    uint8_t response_length = 8 + par_val_length; // 8 bytes fixed part + 2 bytes PAR_ID (reserved byte already included)
    uint8_t response = wire.requestFrom(i2c_address, response_length);

    // If the response is the expected length, parse the data
    if (response == response_length) {
        readCommand(wire, reply);

        // Read and discard the reserved byte
        (void) wire.read();

        // Read PAR_ID
        reply.par_id = wire.read() + (wire.read() << 8);

        // Read the PAR_VAL (up to 8 bytes)
        for (uint8_t i = 0; i < par_val_length; ++i) {
            reply.par_val[i] = wire.read();
        }
        reply.par_val_length = par_val_length;

        reply.error = false;  // No error
    } else {
        reply.error = true;  // Error
    }

    return reply;
}


EPS::standard_reply EPS::reset_watchdog(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::WATCHDOG);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::no_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::NO_OPERATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::system_reset(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    writeCommand(wire, i2c_address, CommandCode::SYSTEM_RESET);
    wire.write(RESET_KEY_SYSTEM_RESET);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::cancel_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::CANCEL_OPERATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::watchdog(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::WATCHDOG);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
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

    writeCommand(wire, i2c_address, CommandCode::SWITCH_TO_SAFETY_MODE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_overcurrent_reply EPS::get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address) {
    pdu_overcurrent_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PDU_OVERCURRENT_FAULT_STATE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    pbu_housekeeping_data_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_RAW);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    pbu_housekeeping_data_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_AVG);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    pcu_housekeeping_data_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_ENG);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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
EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    pcu_housekeeping_data_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_running_average(DWire &wire, uint8_t i2c_address) {
    pcu_housekeeping_data_reply reply;

    writeCommand(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_AVG);

     // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);

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

EPS::standard_reply EPS::switch_nominal_mode(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::SWITCH_NOMINAL_MODE);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_OFF);
    wire.write(ch_idx);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_STATE);

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
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_OFF);

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
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_ON);

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
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::reset_configuration(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    writeCommand(wire, i2c_address, CommandCode::RESET_CONFIGURATION);
    wire.write(CONF_KEY_RESET_CONFIGURATION);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
