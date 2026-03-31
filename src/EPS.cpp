//
// Created by caspar on 6/9/23.
//
#include "EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcset
#include <cstdio>

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
//    Console::log("Command Raw bytes: %x, %x, %x, %x", STID, IVID, static_cast<uint8_t>(commandCode), BID);
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
//    Console::log("Command Raw bytes: %x, %x, %x, %x, %x", STID, IVID, static_cast<uint8_t>(commandCode), BID, fifthByte);
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
//    Console::log("Command Raw bytes: %x, %x, %x, %x, %x, %x", STID, IVID, static_cast<uint8_t>(commandCode), BID, fifthByte, sixthByte);
}
void writeCommandSetConfParam(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, ReturnType par_value) {
    uint8_t param_length = EPS::get_param_length(EPS::getConfigParameterType(par_id));
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&par_value);

    uint8_t bytes[2];
    uint16_t par_id_uint16 = static_cast<uint16_t>(par_id);
    bytes[0] = (uint8_t)(par_id_uint16 >> 8);   // most significant byte
    bytes[1] = (uint8_t)(par_id_uint16 & 0xFF); // least significant byte

    wire.beginTransmission(i2c_address);
    wire.write(STID);
    wire.write(IVID);
    wire.write(static_cast<uint8_t>(CommandCode::SET_CONF_PARAM));
    wire.write(BID);
    wire.write(bytes[1]);
    wire.write(bytes[0]);
    //write the value
    for(uint8_t i = 0; i < param_length; i++) {
        wire.write(bytePtr[i]);
    }
    wire.endTransmission(true); //this is good, do not use false
//    Console::log("Command Raw bytes: %x, %x, %x, %x, %x, %x", STID, IVID, static_cast<uint8_t>(CommandCode::SET_CONF_PARAM), BID, bytes[1], bytes[0]);
//    if (param_length==1)
//    Console::log("%x", bytePtr[0]);
//    if (param_length>1)
//    Console::log("%x, %x", bytePtr[0], bytePtr[1]);
}
void writeCommandSaveConfiguration(DWire &wire, uint8_t i2c_address, uint8_t CONF_KEY, uint16_t CHECKSUM) {
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
    wire.write(static_cast<uint8_t>(CommandCode::SAVE_CONFIGURATION));
    wire.write(BID);
    wire.write(CONF_KEY);
    wire.write(CHECKSUM_byte_1);
    wire.write(CHECKSUM_byte_2);
    wire.endTransmission(true); //this is good, do not use false
//    Console::log("Command Raw bytes: %x, %x, %x, %x, %x, %x, %x", STID, IVID, static_cast<uint8_t>(commandCode), BID, CONF_KEY, CHECKSUM_byte_1, CHECKSUM_byte_2);
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
    //Console::log("read: %d %d %d %d %d", reply.stid, reply.ivid, reply.rc, reply.bid, reply.stat);
}
bool EPS::read_n_bytes(DWire &wire, uint8_t *buf, uint8_t n) {
    //it is assumed you already called:  wire.requestFrom(i2c_address, n);
    //the first 5 bytes are STID, IVID, RC, BID, STAT

    for (uint8_t i = 0; i < n; i++) {
        buf[i] = wire.read();
    }
    return true;
}
void EPS::get_EPS_buffer(DWire &wire, uint8_t i2c_address, uint8_t *buffer, uint8_t buffer_length) {
    uint8_t response = wire.requestFrom(i2c_address, buffer_length);
    EPS::read_n_bytes(wire, buffer, buffer_length);
}
void fill_VIPD_variable(EPS::VIPD &vipd, uint8_t *buf) {
    //it is assumed that buf has at least 6 spaces (it should have exactly 6 spaces)
    vipd.volt  = buf[0]+(buf[1]<<8);
    vipd.curr  = buf[2]+(buf[3]<<8);
    vipd.power = buf[4]+(buf[5]<<8);
}
void fill_BPD_variable(EPS::BPD &bpd, uint8_t *buf) {
    //it is assumed that buf has at least 22 spaces (it should have exactly 22 spaces)
    fill_VIPD_variable(bpd.vip_bp_input, buf);
    bpd.stat_bp    = buf[6]+(buf[7]<<8);
    bpd.volt_cell1 = buf[8]+(buf[9]<<8);
    bpd.volt_cell2 = buf[10]+(buf[11]<<8);
    bpd.volt_cell3 = buf[12]+(buf[13]<<8);
    bpd.volt_cell4 = buf[14]+(buf[15]<<8);
    bpd.bat_temp1  = buf[16]+(buf[17]<<8);
    bpd.bat_temp2  = buf[18]+(buf[19]<<8);
    bpd.bat_temp3  = buf[20]+(buf[21]<<8);
}
void fill_CCD_variable(EPS::CCD &ccd, uint8_t *buf) {
    //it is assumed that buf has at least 14 spaces (it should have exactly 14 spaces)
    fill_VIPD_variable(ccd.vip_cc_output, buf);
    ccd.volt_in_mppt  = buf[6]+(buf[7]<<8);
    ccd.curr_in_mppt  = buf[8]+(buf[9]<<8);
    ccd.volt_out_mppt = buf[10]+(buf[11]<<8);
    ccd.curr_out_mppt = buf[12]+(buf[13]<<8);
}
void fill_CCSD_variable(EPS::CCSD &ccsd, uint8_t *buf) {
    //it is assumed that buf has at least 8 spaces (it should have exactly 8 spaces)
    ccsd.volt_in_mppt  = buf[0]+(buf[1]<<8);
    ccsd.curr_in_mppt  = buf[2]+(buf[3]<<8);
    ccsd.volt_out_mppt = buf[4]+(buf[5]<<8);
    ccsd.curr_out_mppt = buf[6]+(buf[7]<<8);
}
void fill_conf_variable(union ReturnType &par_value, uint8_t *buf, uint8_t n, ParameterType type) {
    //copy n bytes of the buffer into par_value.
    memset(&par_value, 0, sizeof(par_value));
    switch(type)
    {
        case ParameterType::Int8:
            par_value.i8 = (int8_t)buf[0];
            break;
        case ParameterType::UInt8:
            par_value.ui8 = buf[0];
            break;
        case ParameterType::Int16:
        {
            uint16_t tmp = (uint16_t)(buf[0] | (buf[1] << 8));
            par_value.i16 = (int16_t)tmp;
            break;
        }
        case ParameterType::UInt16:
        {
            par_value.ui16 = (uint16_t)(buf[0] | (buf[1] << 8));
            break;
        }
        case ParameterType::Invalid:
        default:
            break;
    }
}
void read_and_fill_conf_param(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, EPS::config_reply &reply) {
    ParameterType type = EPS::getConfigParameterType(par_id);
    uint8_t param_length = EPS::get_param_length(type);
    uint8_t read_length = 8+param_length;
    uint8_t response = wire.requestFrom(i2c_address, read_length);

    if (response == read_length) {
        uint8_t buffer[16];
        EPS::read_n_bytes(wire, buffer, read_length);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.par_id = static_cast<ConfigParameter>(buffer[6]+(buffer[7]<<8));
        //put param_length bytes in the variable.
        fill_conf_variable(reply.par_value, buffer+8, param_length, type);

        reply.error = false;
    } else
        reply.error = true;
}
void check_EPS_response(EPS::ReplyBase &reply, uint8_t expected_response_code) {
    /*
      This method checks the validity of a response from the EPS. It checks:
      -first 4 bits of the reply.error (0xX_) indicates the type of error.
      -the last 4 bits (0x_X) copy the error code from the stat byte, which is only relevant if the command was rejected.
      No error means reply.error == 0

      expected_response_code means response code
     */
    reply.error = 0;
    if (expected_response_code== 0xAB)//reset function, this would give us FF
    {
        if (reply.rc != static_cast<uint8_t>(expected_response_code) && reply.rc !=0xFF)
            reply.error = (reply.error | (0x20));
        if (reply.stat != 0x80 && reply.stat != 0x00 && reply.stat !=0xFF)
            reply.error = (reply.error | (0x40));
        return;
    }

    if (reply.rc != static_cast<uint8_t>(expected_response_code))
        reply.error = (reply.error | (0x20));// incorrect response code received, mark 6th bit

    if (reply.stat != 0x80 && reply.stat != 0x00) //we accept it if it is a "re read"
        reply.error = (reply.error | (0x40));// incorrect status byte received. See reply.stat. mark 7th bit

    reply.error = (reply.error | (reply.stat & 0x0F)); //copy the last 4 bits of stat, which contain the error code if the command was rejected.
    //if it is 0x00 at the end, then it is valid.
}
EPS::config_reply EPS::get_config_param(DWire &wire, uint8_t i2c_address, ConfigParameter par_id) {
    /*
      Action: get the value of a configuration parameter.
     */
    EPS::config_reply reply;

    uint8_t bytes[2];
    uint16_t par_id_uint16 = static_cast<uint16_t>(par_id);
    bytes[0] = (uint8_t)(par_id_uint16 >> 8);   // most significant byte
    bytes[1] = (uint8_t)(par_id_uint16 & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::GET_CONF_PARAM, bytes[1], bytes[0]);

    delay_ms(20);

    read_and_fill_conf_param(wire, i2c_address, par_id, reply);
    return reply;
}

EPS::config_reply EPS::set_config_param(DWire &wire, uint8_t i2c_address, ConfigParameter par_id, ReturnType par_value) {
    /*
      Action: change a configuration parameter. The change will take effect immediately and any function using the parameter
      will use the new value.
      Note: certain parameters are read-only and the command will return an error if the user tries to change such a parameter. Also
      certain parameters only allow specific settings or a reduced range. Updates outside of the allowed range will cause these
      values to be clamped to the minimum/maximum allowable.
     */

    EPS::config_reply reply;

    uint8_t bytes[2];
    uint16_t par_id_uint16 = static_cast<uint16_t>(par_id);
    bytes[0] = (uint8_t)(par_id_uint16 >> 8);   // most significant byte
    bytes[1] = (uint8_t)(par_id_uint16 & 0xFF); // least significant byte

    writeCommandSetConfParam(wire, i2c_address, par_id, par_value);

    delay_ms(20);

    read_and_fill_conf_param(wire, i2c_address, par_id, reply);
    return reply;
}

EPS::config_reply EPS::reset_config_param(DWire &wire, uint8_t i2c_address, ConfigParameter par_id) {
    /*
      Action: reset a parameter to its default hard-coded value. All parameters have this value at system power-up or after the
      software reset command.
     */
    EPS::config_reply reply;

    uint8_t bytes[2];
    uint16_t par_id_uint16 = static_cast<uint16_t>(par_id);
    bytes[0] = (uint8_t)(par_id_uint16 >> 8);   // most significant byte
    bytes[1] = (uint8_t)(par_id_uint16 & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::RESET_CONF_PARAM, bytes[1], bytes[0]);

    delay_ms(20);

    read_and_fill_conf_param(wire, i2c_address, par_id, reply);
    return reply;
}

EPS::standard_reply EPS::no_operation(DWire &wire, uint8_t i2c_address) {
    /*
      Action: performs a no-operation. This is useful to check the availability of the system, without changing anything about
      the current configuration or operation. Response: {<STID> <IVID> 0x03 <BID> 0x80}

      Write length: 4 bytes.
      Read length: 5 bytes.
     */
    standard_reply reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::NO_OPERATION);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::NO_OPERATION) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    /* Write command to EPS */
    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::SYSTEM_RESET, RESET_KEY_SYSTEM_RESET);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::SYSTEM_RESET) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::CANCEL_OPERATION);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::CANCEL_OPERATION) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::WATCHDOG);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::WATCHDOG) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_TO_SAFETY_MODE);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::SWITCH_TO_SAFETY_MODE) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::system_status_reply EPS::get_system_status(DWire &wire, uint8_t i2c_address){
    /*
      Action: return system status information.

        Write length: 4 bytes.
      Read length: 36 bytes.
     */
    system_status_reply reply;
    reply.error = 0;
    EPS::writeCommand(wire, i2c_address, CommandCode::GET_SYSTEM_STATUS);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 36);

    if (response == 36) {
        uint8_t buffer[36];
        EPS::read_n_bytes(wire, buffer, 36);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        reply.mode = buffer[5];
        reply.conf = buffer[6];
        reply.reset_cause = buffer[7];
        //8)AA 9)BB 10)CC 11)DD  -> 0xDDCCBBAA
        reply.uptime = buffer[8]+(buffer[9]<<8)+(buffer[10]<<16)+(buffer[11]<<24);
        reply.sys_error       = buffer[12]+(buffer[13]<<8);
        reply.rc_cnt_pwron    = buffer[14]+(buffer[15]<<8);
        reply.rc_cnt_wdg      = buffer[16]+(buffer[17]<<8);
        reply.rc_cnt_cmd      = buffer[18]+(buffer[19]<<8);
        reply.rc_cnt_mcu      = buffer[20]+(buffer[21]<<8);
        reply.rc_cnt_emlopo   = buffer[22]+(buffer[23]<<8);
        reply.prevcmd_elapsed = buffer[24]+(buffer[25]<<8);
        //the other 10 bytes are reserved.

        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::GET_SYSTEM_STATUS) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::overcurrent_reply EPS::get_overcurrent_fault_state(DWire &wire, uint8_t i2c_address) {
    /*
      Action: Prepare the response buffer with output bus over current events. Over current fault counters are incremented
      each time a bus is latched off due to an overcurrent event

      Note: only applicable to PDU/PIU boards. Specify it in STID

      Write length: 4 bytes.
      Read length: 42 bytes.
     */
    overcurrent_reply reply;
    reply.error = 0;
    EPS::writeCommand(wire, i2c_address, CommandCode::GET_OVERCURRENT_FAULT_STATE);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 42);

    if (response == 42) {
        uint8_t buffer[42];
        EPS::read_n_bytes(wire, buffer, 42);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.stat_ob_on  = buffer[6]+(buffer[7]<<8);
        reply.stat_ob_ocf = buffer[8]+(buffer[9]<<8);

        for (int i=0;i<16;i++)
            reply.ocf_cnt_ch[i] = buffer[10+2*i]+(buffer[11+2*i]<<8);

        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::GET_OVERCURRENT_FAULT_STATE) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::pbu_abf_placed_state EPS::get_pbu_abf_placed_state(DWire &wire, uint8_t i2c_address) {
    /*
      Action: Prepare the response buffer with ABF placed state information.
      Note: only applicable to PBU boards
      For the values of the abf_placed (one value, but redundant):
      - 0xAB = ABF is placed
      - 0x00 = ABF is not placed
      - other values is invalid

      Write length: 4 bytes.
      Read length: 8 bytes.
     */
    EPS::pbu_abf_placed_state reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::GET_PBU_ABF_PLACED_STATE);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 8);

    if (response == 8) {
        uint8_t buffer[42];
        EPS::read_n_bytes(wire, buffer, 42);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.abf_placed_0  = buffer[6];
        reply.abf_placed_1 = buffer[7];

        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::GET_PBU_ABF_PLACED_STATE) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::pdu_housekeeping_data_reply get_pdu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    /*
      Action: Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering form.
      Note: only applicable to PDU boards
      Note: in the raw form, the temp and volt vars are "uint16_t". In eng form, they are "int16_t". We used "int16_t".
      Consider this when you get the raw form (translate from int16 to uint16)

      Write length: 4 bytes.
      Read length: 158 bytes.
     */
    EPS::pdu_housekeeping_data_reply reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, commandCode);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 158);

    if (response == 158) {
        uint8_t buffer[158];
        EPS::read_n_bytes(wire, buffer, 158);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.volt_brdsup = buffer[6]+(buffer[7]<<8);
        reply.temp        = buffer[8]+(buffer[9]<<8);
        fill_VIPD_variable(reply.vip_input, buffer+10);
        reply.stat_ch_on  = buffer[16]+(buffer[17]<<8);
        reply.stat_ch_ocf = buffer[18]+(buffer[19]<<8);

        for(int i=0;i<7;i++)
            fill_VIPD_variable(reply.vip_vd[i], buffer+20+i*6);

        for(int i=0;i<16;i++)
            fill_VIPD_variable(reply.vip_ch[i], buffer+62+i*6);

        check_EPS_response(reply, static_cast<uint8_t>(commandCode) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}
EPS::pdu_housekeeping_data_reply EPS::get_pdu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_RAW);
}

EPS::pdu_housekeeping_data_reply EPS::get_pdu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_ENG);
}

EPS::pdu_housekeeping_data_reply EPS::get_pdu_housekeeping_data_avg(DWire &wire, uint8_t i2c_address) {
    return get_pdu_housekeeping_data(wire, i2c_address, CommandCode::GET_PDU_HOUSEKEEPING_DATA_AVG);
}

EPS::pbu_housekeeping_data_reply get_pbu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    /*
      Action: Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.
      Note: only applicable to PBU boards

      Write length: 4 bytes.
      Read length: 84 bytes.
     */
    EPS::pbu_housekeeping_data_reply reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, commandCode);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 84);

    if (response == 84) {
        uint8_t buffer[84];
        EPS::read_n_bytes(wire, buffer, 84);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.volt_brdsup = buffer[6]+(buffer[7]<<8);
        reply.temp        = buffer[8]+(buffer[9]<<8);
        fill_VIPD_variable(reply.vip_input, buffer+10);
        reply.stat_bu = buffer[16]+(buffer[17]<<8);

        for(int i=0;i<3;i++)
            fill_BPD_variable(reply.bp[i], buffer+18+i*22);

        check_EPS_response(reply, static_cast<uint8_t>(commandCode) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_RAW);
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_ENG);
}

EPS::pbu_housekeeping_data_reply EPS::get_pbu_housekeeping_data_avg(DWire &wire, uint8_t i2c_address) {
    return get_pbu_housekeeping_data(wire, i2c_address, CommandCode::GET_PBU_HOUSEKEEPING_DATA_AVG);
}

EPS::pcu_housekeeping_data_reply get_pcu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    /*
      Action: Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values.
      Note: only applicable to PCU boards

      Write length: 4 bytes.
      Read length: 72 bytes.
     */
    EPS::pcu_housekeeping_data_reply reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, commandCode);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 72);

    if (response == 72) {
        uint8_t buffer[72];
        EPS::read_n_bytes(wire, buffer, 72);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.volt_brdsup = buffer[6]+(buffer[7]<<8);
        reply.temp        = buffer[8]+(buffer[9]<<8);
        fill_VIPD_variable(reply.vip_output, buffer+10);

        for(int i=0;i<4;i++)
            fill_CCD_variable(reply.cc[i], buffer+16+i*14);

        check_EPS_response(reply, static_cast<uint8_t>(commandCode) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW);
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_ENG);
}

EPS::pcu_housekeeping_data_reply EPS::get_pcu_housekeeping_data_avg(DWire &wire, uint8_t i2c_address) {
    return get_pcu_housekeeping_data(wire, i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_AVG);
}


EPS::piu_housekeeping_data_reply get_piu_housekeeping_data(DWire &wire, uint8_t i2c_address, CommandCode commandCode) {
    /*
      Action: Prepare the response buffer with housekeeping data. The housekeeping data is returned in engineering values
      Note: only applicable to PIU boards
      Note: this method assumes you do not have a daughterboard.

      Write length: 4 bytes.
      Read length: 116 bytes w/o daughterboard (or 174 bytes with daughterboard).
     */
    EPS::piu_housekeeping_data_reply reply;
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, commandCode);

    delay_ms(20);

    uint8_t response = wire.requestFrom(i2c_address, 116);

    if (response == 116) {
        uint8_t buffer[116];
        EPS::read_n_bytes(wire, buffer, 116);

        reply.stid = buffer[0];
        reply.ivid = buffer[1];
        reply.rc   = buffer[2];
        reply.bid  = buffer[3];
        reply.stat = buffer[4];
        //buffer[5] is reserved
        reply.volt_brdsup = buffer[6]+(buffer[7]<<8);
        reply.temp        = buffer[8]+(buffer[9]<<8);
        fill_VIPD_variable(reply.vip_dist_input, buffer+10);
        fill_VIPD_variable(reply.vip_bat_input, buffer+16);
        reply.stat_ch_on  = buffer[22]+(buffer[23]<<8);
        reply.stat_ch_ocf = buffer[24]+(buffer[25]<<8);
        reply.bat_stat    = buffer[26]+(buffer[27]<<8);
        reply.bat_temp2   = buffer[28]+(buffer[29]<<8);
        reply.bat_temp3   = buffer[30]+(buffer[31]<<8);

        for(int16_t i=0;i<3;i++)
            reply.volt_vd[i] = buffer[32+i*2]+(buffer[33+i*2]<<8);

        for(int i=0;i<9;i++)
            fill_VIPD_variable(reply.vip_ch[i], buffer+38+i*6);

        for(int i=0;i<3;i++)
            fill_CCSD_variable(reply.cc[i], buffer+92+i*8);

        check_EPS_response(reply, static_cast<uint8_t>(commandCode) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}

EPS::piu_housekeeping_data_reply EPS::get_piu_housekeeping_data_raw(DWire &wire, uint8_t i2c_address) {
    return get_piu_housekeeping_data(wire, i2c_address, CommandCode::GET_PIU_HOUSEKEEPING_DATA_RAW);
}

EPS::piu_housekeeping_data_reply EPS::get_piu_housekeeping_data_eng(DWire &wire, uint8_t i2c_address) {
    return get_piu_housekeeping_data(wire, i2c_address, CommandCode::GET_PIU_HOUSEKEEPING_DATA_ENG);
}

EPS::piu_housekeeping_data_reply EPS::get_piu_housekeeping_data_avg(DWire &wire, uint8_t i2c_address) {
    return get_piu_housekeeping_data(wire, i2c_address, CommandCode::GET_PIU_HOUSEKEEPING_DATA_AVG);
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
    reply.error = 0;

    EPS::writeCommand(wire, i2c_address, CommandCode::SWITCH_NOMINAL_MODE);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::SWITCH_NOMINAL_MODE) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;
    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_ON, ch_idx);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error.
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::OUTPUT_BUS_CHANNEL_ON) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }
    return reply;
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
    reply.error = 0;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_CHANNEL_OFF, ch_idx);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::OUTPUT_BUS_CHANNEL_OFF) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_STATE, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::OUTPUT_BUS_GROUP_STATE) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_OFF, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::OUTPUT_BUS_GROUP_OFF) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    uint8_t bytes[2];
    bytes[0] = (uint8_t)(bitflag >> 8);   // most significant byte
    bytes[1] = (uint8_t)(bitflag & 0xFF); // least significant byte

    EPS::writeCommand6Bytes(wire, i2c_address, CommandCode::OUTPUT_BUS_GROUP_ON, bytes[1], bytes[0]);

    // delay
    delay_ms(20);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    // if response is 5 bytes long populate reply struct else mark error
    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::OUTPUT_BUS_GROUP_ON) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::RESET_CONFIGURATION, CONF_KEY);

    delay_ms(30);

    uint8_t response = wire.requestFrom(i2c_address, 5);

    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::RESET_CONFIGURATION) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
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
    reply.error = 0;

    EPS::writeCommand5Bytes(wire, i2c_address, CommandCode::LOAD_CONFIGURATION, CONF_KEY);

    delay_ms(30);

    // request 5 bytes of data (i.e) the length of the response
    uint8_t response = wire.requestFrom(i2c_address, 5);

    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::LOAD_CONFIGURATION) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}
EPS::standard_reply EPS::save_configuration(DWire &wire, uint8_t i2c_address) {
    /*
      Action: Commit all read/write configuration parameters kept in volatile memory to non-volatile memory.
      Note: a checksum taken over all read/write configuration parameters ensures no inadvertent changes have occurred between
      altering the volatile memory parameters and issuing the save command. Provide 0 as checksum to force-save without this
      protection.
      Note: it needs 30ms!!

      Write length: 7 bytes.
      Read length: 5 bytes.
     */
    standard_reply reply;
    reply.error = 0;

    writeCommandSaveConfiguration(wire, i2c_address, CONF_KEY, 0x0000);

    delay_ms(30);

    uint8_t response = wire.requestFrom(i2c_address, 5);

    if (response == 5) {
        EPS::readCommand(wire, reply);
        check_EPS_response(reply, static_cast<uint8_t>(CommandCode::SAVE_CONFIGURATION) | 0x01);
    } else {
        reply.error = (reply.error | (0x10)); // response too short, mark 5th bit
    }

    return reply;
}
