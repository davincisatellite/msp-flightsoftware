/*
 * DicePayloadCommandHandler.h
 *
 *  Created on: Apr 28, 2023
 *      Author: Marco van Eerden
 *
 * Description:
 *      Contains definitions for the Dice Payload command handler.
 */

#ifndef DICEPAYLOADCOMMANDHANDLER_H_
#define DICEPAYLOADCOMMANDHANDLER_H_

#include <DicePayloadCommon.h>
#include "DWire.h"

using namespace DicePayload;

class DicePayloadCommandHandler
{
private:
    //    TODO: add commands to read sensors when implemented in payload software
    /* List of commands. See Dice Payload ICD document. */
    enum class command_t : uint8_t
    {
        STATUS = 0x00,
        R_M1_SPEED = 0x01,
        R_M2_SPEED = 0x02,
        R_M1_LENGTH = 0x03,
        R_M2_LENGTH = 0x04,
        R_M1_POSITION = 0x05,
        R_LED_BRIGHTNESS = 0x06,
        R_LED_STATUS = 0x07,
        W_M1_SPEED = 0x08,
        W_M2_SPEED = 0x09,
        W_M1_LENGTH = 0x0A,
        W_M2_LENGTH = 0x0B,
        W_LED_BRIGHTNESS = 0x0C,
        SET_MOTORS = 0x10,
        LED_ON = 0x20,
        LED_OFF = 0x21,
        CLAMP = 0x22,
        UNCLAMP = 0x23,
        SET_SWITCHES = 0xE0,
        RESET = 0xF2
    };

    union set_switches_command_t
    {
        struct
        {
            uint8_t clamped1_on : 1;
            uint8_t clamped2_on : 1;
            uint8_t unclamped1_on : 1;
            uint8_t unclamped2_on : 1;
            uint8_t prefix : 4;
        };
        uint8_t command;
    };

    union set_motors_command_t
    {
        struct
        {
            uint8_t motor1_direction : 2;
            uint8_t motor2_direction : 2;
            uint8_t prefix : 4;
        };
        uint8_t command;
    };

    //    TODO: check actual address
    static constexpr uint8_t I2C_ADDRESS = 0x08u;
    static constexpr uint8_t MESSAGE_START = '$';
    static constexpr uint8_t MESSAGE_STOP = '\n';
    static constexpr uint8_t MESSAGE_SIZE_BYTES = 4u;
    static constexpr uint8_t SWITCHES_COMMAND_PREFIX = 0b1110;
    static constexpr uint8_t MOTORS_COMMAND_PREFIX = 0b0001;

    DWire &i2c_bus;

    error_code_t write_command(const command_t command);
    error_code_t write_command(const uint8_t command);
    error_code_t write_data(const command_t command, const uint8_t data);
    error_code_t read_data(const command_t command, uint8_t &data);
    void send_command(const uint8_t command);
    void send_command_with_data(const uint8_t command, const uint8_t data);
public:
    DicePayloadCommandHandler(DWire &i2c_bus);

    error_code_t get_status(status_t &data_buffer);
    error_code_t read_motor1_speed(uint8_t &data_buffer);
    error_code_t read_motor2_speed(uint8_t &data_buffer);
    error_code_t read_motor1_length(uint8_t &data_buffer);
    error_code_t read_motor2_length(uint8_t &data_buffer);
    error_code_t read_motor1_position(motor1_position_t &data_buffer);
    error_code_t read_led_brightness(uint8_t &data_buffer);
    error_code_t read_led_status(uint8_t &data_buffer);

    error_code_t write_motor1_speed(const uint8_t data);
    error_code_t write_motor2_speed(const uint8_t data);
    error_code_t write_motor1_length(const uint8_t data);
    error_code_t write_motor2_length(const uint8_t data);
    error_code_t write_led_brightness(const uint8_t data);

    error_code_t stop_motors();
    error_code_t run_motors(const motor_direction_t motor1_direction, const motor_direction_t motor2_direction);
    error_code_t set_led_state(const bool on);

    error_code_t clamp();
    error_code_t unclamp();
    error_code_t set_switches(const bool clamped1_on, const bool clamped2_on, const bool unclamped1_on, const bool unclamped2_on);
    error_code_t reset();
};


#endif /* DICEPAYLOADCOMMANDHANDLER_H_ */
