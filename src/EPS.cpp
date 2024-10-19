//
// Created by caspar on 6/9/23.
//
#include "EPS.h"


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

EPS::config_reply EPS::set_config_params(DWire &wire, uint8_t i2c_address, uint8_t par_id, uint8_t *par_val) {
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

    // Begin I2C transmission to the given I2C address
    wire.beginTransmission(i2c_address);

    // Command structure based on page 63
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x84); // Command code (Reset Configuration Parameter)
    wire.write(0x00); // BID

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
    uint8_t par_val_length = get_param_length(par_id);

    // Request 8 bytes + PAR_VAL length of data
    uint8_t response_length = 8 + par_val_length;
    uint8_t response = wire.requestForm(i2c_address, response_length);

    // If the response is the expected length, process the reply
    if (response == response_length) {
        reply.stid = wire.read();  // STID
        reply.ivid = wire.read();  // IVID
        reply.rc = wire.read();    // Response code
        reply.bid = wire.read();   // BID
        reply.stat = wire.read();  // Status byte

        // Reserved byte (skip)
        wire.read();

        // Param-ID (read in little-endian)
        reply.par_id = wire.read() + (wire.read() << 8);

        // Read the PAR_VAL
        for (uint8_t i = 0; i < par_val_length; ++i) {
            reply.par_val[i] = wire.read();
        }
        reply.par_val_length = par_val_length;

        // Set error code to 0 (success)
        reply.error = 0;
    } else {
        // Set error code to 1 (failure)
        reply.error = 1;
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

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x86); // Command code (Reset Configuration Parameter)
    wire.write(0x00); // BID

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
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();

        // Read and discard the reserved byte
        wire.read();

        // Read PAR_ID
        reply.par_id = wire.read() + (wire.read() << 8);

        // Read the PAR_VAL (up to 8 bytes)
        for (uint8_t i = 0; i < par_val_length; ++i) {
            reply.par_val[i] = wire.read();
        }
        reply.par_val_length = par_val_length;

        reply.error = 0;  // No error
    } else {
        reply.error = 1;  // Error
    }

    return reply;
}


EPS::standard_reply EPS::reset_watchdog(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;


    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x06);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::no_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x02);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::system_reset(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0xAA);
    wire.write(0x00);
    wire.write(0xA6);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::cancel_operation(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x04);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::watchdog(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x06);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
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

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x32);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pdu_overcurrent_reply EPS::get_pdu_overcurrent_fault_state(DWire &wire, uint8_t i2c_address) {
    pdu_overcurrent_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x42);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();

        // read (reserved) and discard
        wire.read();

        // reads in little endian order
        reply.stat_ob_on = wire.read() + (wire.read() << 1);
        reply.stat_ob_ocf = wire.read() + (wire.read() << 1);

        for (int i = 0; i < 16; ++i) {
            reply.ocf_cnt_ch[i] = wire.read() + (wire.read() << 1);
        }

        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    pbu_housekeeping_data_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x62);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();

        // read (reserved) and discard
        wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 1);
        reply.temp = wire.read() + (wire.read() << 1);

        for (int i = 0; i < 6; ++i) {
            reply.vip_input[i] = wire.read();
        }

        reply.stat_bu = wire.read() + (wire.read() << 1);

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

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x72);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();

        // read (reserved) and discard
        (void) wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 1);
        reply.temp = wire.read() + (wire.read() << 1);

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

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x70);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();

        // read (reserved) and discard
        wire.read();

        reply.volt_brdsup = wire.read() + (wire.read() << 1);
        reply.temp = wire.read() + (wire.read() << 1);

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


    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x30);
    wire.write(0x00);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x18);
    wire.write(0x16);
    wire.write(0x00);
    wire.write(ch_idx);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_state(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;


    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID - system type identifier
    wire.write(0x06); // IVID - interface version identifier
    wire.write(0x14); // CC - command code
    wire.write(0x00); // BID - board identifier

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
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
EPS::standard_reply EPS::output_bus_group_off(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;


    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x12);
    wire.write(0x00);

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
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::output_bus_group_on(DWire &wire, uint8_t i2c_address, uint16_t bitflag) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x06);
    wire.write(0x10);
    wire.write(0x00);

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
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::reset_configuration(DWire &wire, uint8_t i2c_address) {
    standard_reply reply;

    /* Write command to EPS */
    wire.beginTransmission(i2c_address);
    wire.write(0x00);
    wire.write(0x02);
    wire.write(0x90);
    wire.write(0x00);
    wire.write(0xA7);

    // delay
    delay_ms(25);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        reply.stid = wire.read();
        reply.ivid = wire.read();
        reply.rc = wire.read();
        reply.bid = wire.read();
        reply.stat = wire.read();
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}
