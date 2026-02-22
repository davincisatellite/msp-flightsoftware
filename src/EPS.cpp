//
// Created by caspar on 6/9/23.
//
#include "EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy
#include "../src/Console.h"

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

/**
* Important Data taken from the Documentation:

* All bytes within a single write transfer belong to a single command.
* Similarly all bytes during a single read transfer belong to the same response

The system processes commands at fixed internal time intervals of 10 millisecond. The duration until a response is
available depends on when the command was provided with respect to this processing step, and could therefore differ
between 1 ms and 11 ms.

0xFF bytes:
1) The system will process command data at set intervals and generate a response. Reading from the system before a
response is ready will generate 0xFF bytes.
2) During master READ operations that pass beyond the
available response bytes a value of 0xFF (decimal: 255) will be generated


Scheme 1: fixed time wait. Read the response 20 ms after issuing the command.

CC: (a byte in a command) is always an even number.
RC: (is a response) is always an uneven number.
RC = CC OR 0x01 (always) This is how you know what command a response belongs to
*/
/**
* This method writes 4 bytes to the wire.
* The bytes are, in this order: STID, IVID, comandCode, BID
*/
void EPS::writeCommand(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    // 4 bytes commands
    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
    wire.endTransmission(true); //this is good, do not use false
}
void EPS::writeCommand5Bytes(DWire &wire, uint8_t i2c_address, CommandCode commandCode, uint8_t fifthByte) {
  /*
     Commands with 5 bytes, and what "fifthByte" means there:
     - System Reset           (RST_KEY=0xA6)
     - Output Bus Channel Off (CH_IDX)
     - Reset Configuration    (CONF_KEY=0xA7)
     - Load Configuration     (CONF_KEY=0xA7)
   */
    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
    wire.write(fifthByte);
    wire.endTransmission(true); //this is good, do not use false
}
void EPS::writeCommand6Bytes(DWire &wire, uint8_t i2c_address, CommandCode commandCode, uint8_t fifthByte, uint8_t sixthByte) {
  /*
     Commands with 6 bytes, and what "fifthByte" + "sixthByte" mean there:
     - Output Bus Group On    (CH_BF, example: (0x03 0x05)=0x0503)
     - Output Bus Group Off   (CH_BF, example: (0x03 0x05)=0x0503)
     - Output Bus Group State (CH_BF, example: (0x03 0x05)=0x0503)
     - Get Configuration Parameter   (PAR_ID, example: (0x00 0x20)=0x2000)
     - Reset Configuration Parameter (PAR_ID, example: (0x00 0x20)=0x2000)
   */
    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
    wire.write(fifthByte);
    wire.write(sixthByte);
    wire.endTransmission(true); //this is good, do not use false
}
void EPS::writeCommandSaveConfiguration(DWire &wire, uint8_t i2c_address, CommandCode commandCode, uint8_t CONF_KEY, uint16_t CHECKSUM) {
  /*
     Commands with 7 bytes
     Checksum f:  f(x) = x16 + x12 + x5 + 1. The seed is 0xFFFF.
     (To force save this value, set checksum to 0)
   */
    uint8_t CHECKSUM_byte_1 = (uint8_t)(CHECKSUM & 0x00FF);
    uint8_t CHECKSUM_byte_2 = (uint8_t)((CHECKSUM >> 8) & 0x00FF);

    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(commandCode));
    wire.write(BID);
    wire.write(CONF_KEY);
    wire.write(CHECKSUM_byte_1);
    wire.write(CHECKSUM_byte_2);
    wire.endTransmission(true); //this is good, do not use false
}
//Set Configuration Parameter has 6+ bytes.

void EPS::readCommand(DWire &wire, EPS::ReplyBase &reply) {
    /*
    It will respond with this format:
       {<STID> <IVID> <RC> <BID> <STAT>}
    where RC = CC Or 0x01
    STID:
       -0x00 (bypass)
       -0x11 (PDU)
       -0x12 (PBU)
       -0x13 (PCU)
       -0x1A (PIU)
    STAT:
       - 0x00 = accepted, but re read
       - 0x01 = rejected
       - 0x02 = rejected: invalid command code
       - 0x03 = rejected: parameter missing
       - 0x04 = rejected: parameter missing
       - 0x05 = rejected: unavailable in current mode/configuration
       - 0x06 = rejected: invalid system type, interface version or bid
       - 0x07 = internal error occurred during processing
       - 0x80 = accepted and first read
*/

    reply.stid = wire.read();   // STID
    reply.ivid = wire.read();   // IVID
    reply.rc = wire.read();     // Response code
    reply.bid = wire.read();    // BID
    reply.stat = wire.read();   // Status byte
//    Console::log("read: %d %d %d %d %d", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat);
}
bool read_n_bytes(DWire &wire, uint8_t *buf, uint8_t n) {
    //it is assumed you already called:  wire.requestFrom(i2c_address, n);
    //the first 5 bytes are STID, IVID, RC, BID, STAT
    if (n<0) return false;

    for (uint8_t i = 0; i < n; i++) {
        buf[i] = wire.read();
    }
    return true;
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

EPS::config_reply EPS::read_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, EPS::config_reply &reply) {

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
  /*
	Action: performs a no-operation. This is useful to check the availability of the system, without changing anything about
	the current configuration or operation. Response: {<STID> <IVID> 0x03 <BID> 0x80}

	Write length: 4 bytes.
	Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::NO_OPERATION);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error
    if (response == 5) {
        // wire->beginTransmission(i2c_address)
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }

    return reply;

}

EPS::standard_reply EPS::system_reset(DWire &wire, uint8_t i2c_address) {
  /*
    Action: perform a software induced reset of the MCU.
	Note: a reply to this command will not always be retrievable, given that the system will shut down the command interface
	while preparing for the hard reset. After reset the reply will not be available anymore, instead having no response available
	(i.e. returns 0xFF on READ)

	Write length: 5 bytes.
	Read length: 5 bytes.

   */
    standard_reply reply;

    /* Write command to EPS */
    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::SYSTEM_RESET, RESET_KEY_SYSTEM_RESET);

    // delay
    delay_ms(20);

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
  /*
	Action: Switches off any command-enable output bus channels that have been switched on after the system powered up.
	Only output bus channels that can be commanded off are affected.
	All force-enable channels will remain enabled.

	Write length: 4 bytes.
	Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::CANCEL_OPERATION);

    // delay
    delay_ms(20);

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
  /*
	Action: Resets the watchdog timer keeping the system from performing a reset.
	Note that any traffic with the system implicitly performs a watchdog reset, hence periodic interaction with the system
	through other commands removes the requirement on sending this particular command.

	Write length: 4 bytes.
	Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::WATCHDOG);

    // delay
    delay_ms(20);

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



EPS::standard_reply EPS::switch_safety_mode(DWire &wire, uint8_t i2c_address) {
  /*
    Action: move subsystem to safety mode. This switches off all non-auto-enable output bus channels. Auto-enable lines will
	remain powered. Any commands to enable non-auto-enable channels will be rejected. Automatic transfer to safety mode
	occurs when the PDU rail voltage level falls below the threshold set in the configuration parameter system.
	Note: only applicable to PDU and PIU boards

	Write length: 4 bytes.
	Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_TO_SAFETY_MODE);

    // delay
    delay_ms(20);

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

EPS::pdu_abf_placed_state EPS::get_pdu_abf_placed_state(DWire &wire, uint8_t i2c_address) {
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
  /*
    Action: move system to nominal mode. This provides full control of all output busses. The system automatically enters
	nominal mode after startup mode or when the PDU system is in safety mode or emergency low power mode and the PDU
	rail voltage exceeds their respective high threshold set in the configuration parameter system.
	Note: only applicable to PDU and PIU boards

	Write length: 4 bytes.
	Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_NOMINAL_MODE);

    // delay
    delay_ms(20);

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
EPS::standard_reply EPS::output_bus_channel_on(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {
  /*
    Action: Turn a single output bus channel on using the bus channel index. Index 0 represents channel 0 (CH0),
    index 1represents channel 1 (CH1), etc.
    Note: only applicable to PDU and PIU boards.

    Write length: 5 bytes.
    Read length: 5 bytes.
*/
    standard_reply reply;
    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_ON, ch_idx);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response if 5 bytes long populate reply struct else mark error.
    if (response == 5) {
        EPS::readCommand(wire, reply);
        reply.error = false;
    } else {
        reply.error = true;
    }
}
EPS::standard_reply EPS::output_bus_channel_off(DWire &wire, uint8_t i2c_address, uint8_t ch_idx) {
   /*
    Action: Turn a single output bus channel off using the bus channel index. Index 0 represents channel 0 (CH0),
    index 1represents channel 1 (CH1), etc.
    Note: only applicable to PDU and PIU boards.
    Note: channels configured as permanent output channel cannot be turned off and commanding this will return an error.

    Write length: 5 bytes.
    Read length: 5 bytes.
*/
    standard_reply reply;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_OFF, ch_idx);

    // delay
    delay_ms(20);

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
  /*
    Action: Turn-on bus channels that are marked with a 1-bit, turn-off bus channels that are not marked (i.e. 0-bit).
	The leastsignificant bit corresponds to bus channel 0 (CH00), the next bit corresponds to channel 1 (CH01), etc.
	For example, providing the flag field 0b00001010 (=0x0A, decimal 10) turns on bus channels 1 and 3, while turning off all
	the other channels.
	Note: only applicable to PDU and PIU boards.
    Note that permanent (always powered, non-switchable) channels are ignored and will remain powered.

    Write length: 6 bytes.
    Read length: 5 bytes.
   */
    standard_reply reply;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_STATE, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

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
  /*
    Action: Turn-off output bus channels that are marked with a 1-bit, leave bus channels that are not marked unaltered. The
	least-significant bit corresponds to bus channel 0 (CH0), the next bit corresponds to channel 1 (CH1), etc.
	For example, providing the flag field 0b00001010 (=0x0A, decimal 10) turns off bus channels 1 and 3, while leaving the
	other channels unaltered.
	Note: only applicable to PDU and PIU boards.

    Write length: 6 bytes.
    Read length: 5 bytes.
   */
    standard_reply reply;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_OFF, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

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
  /*
    Action: Turn-on output bus channels that are marked with a 1-bit, leave bus channels that are not marked unaltered. The
	least-significant bit corresponds to bus channel 0 (CH0), the next bit corresponds to channel 1 (CH1), etc.
	For example, providing the flag field 0b00001010 (=0x0A, decimal 10) turns on bus channels 1 and 3, while leaving the
	other channels unaltered.
	Note: only applicable to PDU and PIU boards.

    Write length: 6 bytes.
    Read length: 5 bytes.
   */
    standard_reply reply;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_ON, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

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
  /*
    Action: Reset all configuration parameters to hard-coded defaults, discarding any changes made, in volatile memory
    (only!). This is performed automatically at system startup before an attempt to load a configuration is performed. If no
    (valid) configuration is found that can be loaded, the system will use hard coded defaults.

    Write length: 5 bytes.
    Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::RESET_CONFIGURATION, CONF_KEY);
//    wire.write(CONF_KEY_RESET_CONFIGURATION);

    // delay
    delay_ms(20);

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

EPS::standard_reply EPS::load_configuration(DWire &wire, uint8_t i2c_address) {
  /*
    Action: Load all configuration parameters from non-volatile memory, discarding any changes made in volatile memory.
    This is performed automatically at system startup if a valid load configuration is encountered in non-volatile memory. If
    no (valid) configuration is found, the system will initialize using hard coded defaults.

    Write length: 5 bytes.
    Read length: 5 bytes.
   */
    standard_reply reply;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::LOAD_CONFIGURATION, CONF_KEY);
//    wire.write(CONF_KEY_RESET_CONFIGURATION);

    // delay
    delay_ms(20);

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