/*
 * DicePayloadCommandHandler.cpp
 *
 *  Created on: Apr 28, 2023
 *      Author: Marco van Eerden
 *
 * Description:
 *      Contains the Dice Payload command handler. This class can be used to send commands
 *      to the dice payload and receive data from it. It directly interfaces with and abstracts from
 *      the I2C bus.
 */

#include "DicePayloadCommandHandler.h"

// TODO: remove before flight
#include "Console.h"

/**
 * Constructor.
 */
DicePayloadCommandHandler::DicePayloadCommandHandler(DWire &i2c_bus) :
    i2c_bus(i2c_bus)
{}

/**
 * Write a single command without data to the payload via the I2C bus.
 * This function is here so we can convert command_t to uint8_t under the hood.
 *
 * @param command Command to send.
 *
 * @return I2C_WRITE_ERROR if write was unsuccessful. NO_ERROR otherwise.
 */
error_code_t DicePayloadCommandHandler::write_command(const command_t command)
{
    return this->write_command(static_cast<uint8_t>(command));
}

/**
 * Write a single command without data to the payload via the I2C bus.
 *
 * @param command Command to send.
 *
 * @return I2C_WRITE_ERROR if write was unsuccessful. NO_ERROR otherwise.
 */
error_code_t DicePayloadCommandHandler::write_command(const uint8_t command)
{
    this->send_command(command);

    return this->i2c_bus.endTransmission() ? error_code_t::I2C_WRITE_ERROR : error_code_t::NO_ERROR;
}

/**
 * Write a single command with data to the payload the via I2C bus.
 *
 * @param command   Command to send.
 * @param data      Data byte to send.
 *
 * @return I2C_WRITE_ERROR if write was unsuccessful. NO_ERROR otherwise.
 */
error_code_t DicePayloadCommandHandler::write_data(const command_t command, const uint8_t data)
{
    this->send_command_with_data(static_cast<uint8_t>(command), data);

    return this->i2c_bus.endTransmission() ? error_code_t::I2C_WRITE_ERROR : error_code_t::NO_ERROR;
}

// TODO: ask payload to remove $ and \n --> they can do it
// TODO: do we need a CRC? --> Yes it will be implemented
/**
 * Write a single command to the payload, then read data coming from the payload.
 * The data byte that is read is stored in the 'data' parameter.
 *
 * @param command   Command to send.
 * @param data      Data is written into here.
 *
 * @return INVALID_COMMAND_FROM_PAYLOAD if the command sent by the payload is not equal to the command sent by us.
 *         I2C_READ_ERROR if an error occurred during reading.
 *         NO_ERROR otherwise.
 */
error_code_t DicePayloadCommandHandler::read_data(const command_t command, uint8_t &data)
{
    error_code_t error_code = error_code_t::NO_ERROR;


    /* Send command to receive data. */
    this->send_command(static_cast<uint8_t>(command));

    /* Get number of bytes received from I2C bus. */
    uint8_t number_of_bytes_to_read = this->i2c_bus.requestFrom(I2C_ADDRESS, MESSAGE_SIZE_BYTES);

    if (number_of_bytes_to_read == MESSAGE_SIZE_BYTES)
    {
        /* Read and discard MESSAGE_START symbol. */
        //TODO: remove this when payload software updated
        this->i2c_bus.read();

        /* Read command and compare to command sent. */
        if (static_cast<command_t>(this->i2c_bus.read()) != command)
        {
            error_code = error_code_t::INVALID_COMMAND_FROM_PAYLOAD;
        }

        /* Read and store data. We still store it in case of an invalid command,
         * because it may show something about why that occurred.
         * We also need to read the message to get it out of the buffer.
         */
        data = this->i2c_bus.read();

        // TODO: check CRC when implemented in payload software

        /* Read and discard MESSAGE_STOP symbol. */
        //TODO: remove this when payload software updated
        this->i2c_bus.read();
    }
    /* In the else case, number_of_bytes_to_read is always 0. This only happens when an error occurred or nothing was received. */
    else
    {
        /* Clear data so it is clear we got an error. */
        data = 0;

        error_code = error_code_t::I2C_READ_ERROR;
    }

    return error_code;
}

/**
 * Send a command over the I2C bus.
 *
 * @param command Command to send.
 */
void DicePayloadCommandHandler::send_command(const uint8_t command)
{
    this->i2c_bus.beginTransmission(I2C_ADDRESS);

    // TODO: remove before flight
    Console::log("COMMAND SENT: %d | ", command);

    this->i2c_bus.write(static_cast<uint_fast8_t>(command));
}

/**
 * Send a command over the I2C bus, with a data byte attached.
 *
 * @param command   Command to send.
 * @param data      Data to send.
 */
void DicePayloadCommandHandler::send_command_with_data(const uint8_t command, const uint8_t data)
{
    this->i2c_bus.beginTransmission(I2C_ADDRESS);

    // TODO: remove before flight
    Console::log("COMMAND SENT: %d - WITH DATA: %d |", command, data);

    this->i2c_bus.write(static_cast<uint_fast8_t>(command));
    this->i2c_bus.write(data);
}

/**
 * Get dice payload status. Possible statuses are indicated in the status_t enum.
 *
 * @param data_buffer Function writes status to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::get_status(status_t &data_buffer)
{
    uint8_t data_buffer_raw;
    error_code_t error_code = this->read_data(command_t::STATUS, data_buffer_raw);

    data_buffer = static_cast<status_t>(data_buffer_raw);

    return error_code;
}

/**
 * Get the speed of motor 1. The speed is given in PWM duty cycle.
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_motor1_speed(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_M1_SPEED, data_buffer);
}

/**
 * Get the speed of motor 2. The speed is given in PWM duty cycle.
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_motor2_speed(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_M2_SPEED, data_buffer);
}

/**
 * Get the length of motor 1. The length is given in TODO:.
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_motor1_length(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_M1_LENGTH, data_buffer);
}

/**
 * Get the length of motor 2. The length is given in TODO:.
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_motor2_length(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_M2_LENGTH, data_buffer);
}

/**
 * Get the position of motor 1. Possible positions are indicated in the motor1_position_t enum.
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_motor1_position(motor1_position_t &data_buffer)
{
    uint8_t data_buffer_raw;
    error_code_t error_code = this->read_data(command_t::R_M1_POSITION, data_buffer_raw);

    data_buffer = static_cast<motor1_position_t>(data_buffer_raw);

    return error_code;
}

/**
 * Get the brightness of the LED given in TODO
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_led_brightness(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_LED_BRIGHTNESS, data_buffer);
}

/**
 * Get the status of the LED (on = 1, off = 0).
 *
 * @param data_buffer Function writes value to this buffer.
 *
 * @return Error code (see read_data).
 */
error_code_t DicePayloadCommandHandler::read_led_status(uint8_t &data_buffer)
{
    return this->read_data(command_t::R_LED_STATUS, data_buffer);
}

/**
 * Set the motor 1 speed in TODO
 *
 * @param data Motor speed.
 *
 * @return Error code (see write_data).
 */
error_code_t DicePayloadCommandHandler::write_motor1_speed(const uint8_t data)
{
    return this->write_data(command_t::W_M1_SPEED, data);
}

/**
 * Set the motor 2 speed in TODO
 *
 * @param data Motor speed.
 *
 * @return Error code (see write_data).
 */
error_code_t DicePayloadCommandHandler::write_motor2_speed(const uint8_t data)
{
    return this->write_data(command_t::W_M2_SPEED, data);
}

/**
 * Set the motor 1 length in TODO
 *
 * @param data Motor length.
 *
 * @return Error code (see write_data).
 */
error_code_t DicePayloadCommandHandler::write_motor1_length(const uint8_t data)
{
    return this->write_data(command_t::W_M1_LENGTH, data);
}

/**
 * Set the motor 2 length in TODO
 *
 * @param data Motor length.
 *
 * @return Error code (see write_data).
 */
error_code_t DicePayloadCommandHandler::write_motor2_length(const uint8_t data)
{
    return this->write_data(command_t::W_M2_LENGTH, data);
}

/**
 * Set the LED brightness in TODO
 *
 * @param data LED brightness.
 *
 * @return Error code (see write_data).
 */
error_code_t DicePayloadCommandHandler::write_led_brightness(const uint8_t data)
{
    return this->write_data(command_t::W_LED_BRIGHTNESS, data);
}

/**
 * Stop all motors by setting direction to NONE.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::stop_motors()
{
    return this->run_motors(motor_direction_t::NONE, motor_direction_t::NONE);
}

/**
 * Run motors in given direction. A direction of NONE stops the motor.
 *
 * @param motor1_direction Motor 1 direction.
 * @param motor2_direction Motor 2 direction.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::run_motors(const motor_direction_t motor1_direction, const motor_direction_t motor2_direction)
{
    /* The command depends on the motor directions. See Dice Payload ICD document for more information. */
    set_motors_command_t command_container;
    command_container.prefix = MOTORS_COMMAND_PREFIX;
    command_container.motor1_direction = static_cast<uint8_t>(motor1_direction);
    command_container.motor2_direction = static_cast<uint8_t>(motor2_direction);

    return this->write_command(command_container.command);
}

/**
 * Set LED to ON (true) or OFF (false).
 *
 * @param on LED state.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::set_led_state(const bool on)
{
    return this->write_command(on ? command_t::LED_ON : command_t::LED_OFF);
}

/**
 * Command the payload to run the CLAMP routine.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::clamp()
{
    return this->write_command(command_t::CLAMP);
}

/**
 * Command the payload to run the UNCLAMP routine.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::unclamp()
{
    return this->write_command(command_t::UNCLAMP);
}

/**
 * Set the state of the switches. State can be ON (true) or OFF (false) for each switch.
 *
 * @param clamped1_on   State of the clamped1 switch.
 * @param clamped2_on   State of the clamped2 switch.
 * @param unclamped1_on State of the unlcamped1 switch.
 * @param unclamped2_on State of the unclamped2 switch.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::set_switches(const bool clamped1_on, const bool clamped2_on, const bool unclamped1_on, const bool unclamped2_on)
{
    /* The command depends on the states of the switches. See Dice Payload ICD document for more information. */
    set_switches_command_t command_container;
    command_container.prefix = SWITCHES_COMMAND_PREFIX;
    command_container.clamped1_on = clamped1_on;
    command_container.clamped2_on = clamped2_on;
    command_container.unclamped1_on = unclamped1_on;
    command_container.unclamped2_on = unclamped2_on;

    return this->write_command(command_container.command);
}

/**
 * Reset the microcontroller on the payload.
 *
 * @return Error code (see write_command).
 */
error_code_t DicePayloadCommandHandler::reset()
{
    return this->write_command(command_t::RESET);
}
