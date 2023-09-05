/*
 * DicePayload.h
 *
 *  Created on: Apr 28, 2023
 *      Author: Marco van Eerden
 *
 * Description:
 *      Contains common enums used by Dice Payload drivers.
 */

#ifndef DICEPAYLOADCOMMON_H_
#define DICEPAYLOADCOMMON_H_

namespace DicePayload
{
    /* Error codes used in the Dice Payload drivers. */
    enum class error_code_t
    {
        NO_ERROR = 0u,                      /* Everything is OK. */
        INVALID_COMMAND_FROM_PAYLOAD = 1u,  /* Payload sent a reply with a command that did not correspond to the command sent by the OBC. */
        I2C_READ_ERROR = 2u,                /* An error occurred during reading. */
        I2C_WRITE_ERROR = 3u,               /* An error occurred during writing. */
        IMPOSSIBLE_STATE = 4u,              /* The FSM is in an impossible state. */
        INVALID_STATE_TRANSITION = 5u,      /* Given state transition is not allowed. */
        DRIVER_BUSY = 6u,                   /* Driver is currently busy executing. */
        NOT_YET_INITIALISED = 7u,           /* Stop command was given but driver was not yet initialised. */
        FORCED_POWER_OFF = 8u,              /* Cannot start the payload sequence because the payload is forced to be powered off. */
        STATUS_NOT_OK = 9u,                 /* Payload status is not OK. */
        UNCLAMPING_TIMEOUT = 10u,           /* The driver waited for the payload to unclamp but it never did. */
        CLAMPING_TIMEOUT = 11u              /* The driver waited for the payload to clamp but it never did. */
    };

    /* Possible motor positions. See Dice Payload ICD document. */
    enum class motor1_position_t
    {
        UNKNOWN = 0x00,
        CLAMPED = 0x01,
        UNCLAMPED = 0x02
    };

    /* Possible motor directions. See Dice Payload ICD document. */
    enum class motor_direction_t
    {
        NONE = 0u,
        CLOCKWISE = 2u,
        COUNTER_CLOCKWISE = 3u
    };

//    TODO: ask payload to add more statuses to indicate what went wrong
    /* Possible payload statuses. See Dice Payload ICD document. */
    enum class status_t
    {
        INIT = 0x00,
        OK = 0x01,
        FAIL = 0xF1
    };
}

#endif /* DICEPAYLOADCOMMON_H_ */
