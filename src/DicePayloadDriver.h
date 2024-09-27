/*
 * DicePayload.h
 *
 *  Created on: Apr 26, 2023
 *      Author: Marco van Eerden
 */

#ifndef DICEPAYLOADDRIVER_H_
#define DICEPAYLOADDRIVER_H_

#include "DicePayloadCommon.h"
#include "DicePayloadCommandHandler.h"

using namespace DicePayload;

// TODO: add dice payload task
// TODO: processing picture should probably be somewhere else.
// TODO: elaborate on why enum classes: https://stackoverflow.com/questions/18335861/why-is-enum-class-preferred-over-plain-enum
// TODO: add driver for camera (change settings, take picture, process picture?).
// TODO: make a test plan!

class DicePayloadDriver
{
private:
    /* Current dice payload driver state.
     * This is NOT the state of the payload, it is the state of the payload DRIVER.
     */
    enum class state_t : uint8_t
    {
        INIT,
        STOPPED,
        POWER_ON,
        CHECK_STATUS,
        CLAMP,
        TAKE_PICTURE,
        UNCLAMP,
        ERROR
    } state;

    /* Milliseconds required for the payload to clamp or unclamp the dice. */
    // TODO: Is this the actual time?
    // TODO: scale this according to motor speed
    static constexpr uint32_t clamp_unclamp_required_time_milliseconds = 5000;

    /* Period of every FSM iteration in milliseconds. */
    const uint32_t fsm_period_milliseconds;

    /* FSM iterations required for the payload to clamp or unclamp the dice. */
    const uint32_t clamp_unclamp_required_fsm_iterations;

    /* Indicates if the payload is powered. */
    bool payload_is_powered;

    /* Force the payload to power off when in the STOPPED state. */
    bool force_power_off;

    /* Indicates if we want to run the motors or if we only want to take a picture. */
    bool run_motors;

    /* Sub state number used to activate different behaviour inside a state. */
    uint8_t sub_state;

    /* Used to send/receive to/from the payload. */
    DicePayloadCommandHandler &command_handler;

    /* FSM iterations since initialisation. */
    uint32_t fsm_iterations_since_init;

    /* Used to save the number of FSM iterations to compare later. Implements a delay. */
    uint32_t fsm_iterations_saved;

    error_code_t set_state(state_t new_state);
    error_code_t check_payload_status(void);
    void clamp_unclamp_wait_start(void);
    bool clamp_unclamp_wait_expired(void);

    error_code_t init_action(void);
    error_code_t stopped_action(void);
    error_code_t power_on_action(void);
    error_code_t check_status_action(void);
    error_code_t clamp_action(void);
    error_code_t take_picture_action(void);
    error_code_t unclamp_action(void);
    error_code_t error_action(void);
public:
    /* Set to false by the Dice Payload Task if there is a command waiting to be processed.
     * Used to determine if the payload needs to be powered off or not in the STOPPED state.
     * Defaults to true when the object is first created.
     */
    // TODO: should we do it like this? Maybe we don't know if there are any commands pending.
    // TODO: is this even necessary? Can we just stop and then start again?
    bool power_off_when_stopped;

    DicePayloadDriver(DicePayloadCommandHandler &command_handler, uint32_t fsm_period_milliseconds);

    error_code_t run_payload_sequence(bool run_motors);
    error_code_t terminate_payload_sequence(void);
    void recover_from_error(void);

    state_t get_state(void);
    error_code_t state_step(void);
};

#endif /* DICEPAYLOADDRIVER_H_ */
