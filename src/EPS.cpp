//
// Created by caspar on 6/9/23.
//
#include "EPS.h"

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
