/*
 * DicePayload.cpp
 *
 *  Created on: Apr 26, 2023
 *      Author: Marco van Eerden
 */

#include <DicePayloadDriver.h>

// TODO: remove before flight
#include "Console.h"

/**
 * Constructor. Sets variables but does nothing else.
 */
// TODO: Do we want to continue after rebooting or do we want to start over?
DicePayloadDriver::DicePayloadDriver(DicePayloadCommandHandler &command_handler, uint32_t fsm_period_milliseconds) :
    state(state_t::INIT),
    command_handler(command_handler),
    fsm_period_milliseconds(fsm_period_milliseconds),
    clamp_unclamp_required_fsm_iterations(clamp_unclamp_required_time_milliseconds / fsm_period_milliseconds),
    payload_is_powered(true), /* Assume payload is powered, so we unpower it when initialising. */
    force_power_off(false),
    run_motors(false),
    sub_state(0u),
    power_off_when_stopped(true),
    fsm_iterations_since_init(0u),
    fsm_iterations_saved(0u)
{}

/**
 * Run the payload sequence.
 * Manual transition from the STOPPED to POWER_ON state.
 *
 * Returns DRIVER_BUSY if the driver is not in the STOPPED state.
 * Otherwise, returns the result of the state transition.
 */
error_code_t DicePayloadDriver::run_payload_sequence(bool run_motors)
{
    error_code_t error_code = error_code_t::NO_ERROR;

    if (this->state != state_t::STOPPED)
    {
        error_code = error_code_t::DRIVER_BUSY;
    }
    else if (this->force_power_off)
    {
        error_code = error_code_t::FORCED_POWER_OFF;
    }
    else
    {
        // TODO: remove before flight
        Console::log("Starting payload sequence. run_motors = %d", (uint8_t) run_motors);

        this->run_motors = run_motors;
        error_code = this->set_state(state_t::POWER_ON);
    }

    return error_code;
}

/**
 * Stop the payload sequence immediately by transitioning to the STOPPED state (manual transition).
 *
 * Returns NOT_YET_INITIALISED when in the INIT state.
 * Otherwise, returns the result of the state transition.
 *
 * This can leave the dice payload in a state that is not correct for starting the sequence, but this
 * is rectified in the CHECK_STATUS state.
 */
error_code_t DicePayloadDriver::terminate_payload_sequence(void)
{
    error_code_t error_code = error_code_t::NO_ERROR;

    if (this->state != state_t::INIT)
    {
        // TODO: remove before flight
        Console::log("Terminating payload sequence.");

        error_code = this->set_state(state_t::STOPPED);
    }
    else
    {
        error_code = error_code_t::NOT_YET_INITIALISED;
    }

    return error_code;
}

/**
 * Recover from an error by turning off force_power_off.
 */
void DicePayloadDriver::recover_from_error(void)
{
    this->force_power_off = false;
}

/**
 * Return the current payload driver state.
 */
DicePayloadDriver::state_t DicePayloadDriver::get_state(void)
{
    return this->state;
}

/**
 * Contains the state transition table.
 * If the desired state transition is valid, returns NO_ERROR.
 * Otherwise, returns INVALID_STATE_TRANSITION.
 *
 * If the desired state does not exist, returns IMPOSSIBLE_STATE.
 *
 * During operation, we should NEVER get the INVALID_STATE_TRANSITION or IMPOSSIBLE_STATE error!
 * If we do, the software needs to be updated because we cannot operate the payload.
 */
error_code_t DicePayloadDriver::set_state(state_t new_state)
{
    error_code_t error_code = error_code_t::NO_ERROR;

    // TODO: remove before flight
    Console::log("Setting state to %d", (uint8_t) new_state);

    /* Reset sub state to default value. */
    this->sub_state = 0;

    switch (this->state)
    {
    /* INIT -> STOPPED  (automatic) */
    case state_t::INIT:
        if (new_state == state_t::STOPPED)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* STOPPED -> STOPPED   (manual)
     *         -> POWER_ON  (manual)
     *         -> ERROR     (automatic)
     */
    case state_t::STOPPED:
        if (new_state == state_t::POWER_ON ||
            new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* POWER_ON -> CHECK_STATUS    (automatic)
     *          -> STOPPED         (manual)
     *          -> ERROR           (automatic)
     */
    case state_t::POWER_ON:
        if (new_state == state_t::CHECK_STATUS ||
            new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* CHECK_STATUS -> CLAMP        (automatic)
     *              -> STOPPED     (manual)
     *              -> ERROR       (automatic)
     */
    case state_t::CHECK_STATUS:
        if (new_state == state_t::CLAMP ||
            new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* CLAMP -> OPERATIONAL  (automatic)
     *       -> STOPPED      (manual)
     *       -> ERROR        (automatic)
     */
    case state_t::CLAMP:
        if (new_state == state_t::TAKE_PICTURE ||
            new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* TAKE_PICTURE -> STOPPED   (automatic)
     *              -> ERROR     (automatic)
     */
    case state_t::TAKE_PICTURE:
        if (new_state == state_t::UNCLAMP ||
            new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* UNCLAMP -> STOPPED   (automatic)
     *         -> ERROR     (automatic)
     */
    case state_t::UNCLAMP:
        if (new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* ERROR -> STOPPED (automatic)
     *       -> ERROR   (automatic)
     */
    case state_t::ERROR:
        if (new_state == state_t::STOPPED ||
            new_state == state_t::ERROR)
        {
            this->state = new_state;
        }
        else
        {
            error_code = error_code_t::INVALID_STATE_TRANSITION;
        }
        break;
    /* New state is not recognised. */
    default:
        error_code = error_code_t::IMPOSSIBLE_STATE;
        break;
    }

    return error_code;
}

/**
 * Check the dice payload status.
 * Return NO_ERROR if no error occurred.
 * Return STATUS_NOT_OK if status was not OK.
 * Return other error if communication failed (see DicePayloadCommandHandler::get_status).
 */
error_code_t DicePayloadDriver::check_payload_status(void)
{
    error_code_t error_code;
    status_t status;

    // TODO: remove before flight
    Console::log("Getting payload status...");

    /* Attempt to get the status of the payload. */
    error_code = this->command_handler.get_status(status);

    /* Return error code if communication failed. */
    if (error_code != error_code_t::NO_ERROR)
    {
        return error_code;
    }

    /* Return STATUS_NOT_OK error when status is invalid. */
    if (status != status_t::OK)
    {
        // TODO: we want to know what the actual status is, find a way to return it
        // TODO: remove before flight
        Console::log("Status not OK: %d", (uint8_t) status);
        return error_code_t::STATUS_NOT_OK;
    }

    return error_code;
}

/**
 * Start waiting for clamp or unclamp event.
 */
void DicePayloadDriver::clamp_unclamp_wait_start(void)
{
    this->fsm_iterations_saved = this->fsm_iterations_since_init;
}

/**
 * Check if the waiting time for clamping or unclamping has expired.
 */
bool DicePayloadDriver::clamp_unclamp_wait_expired(void)
{
    return this->fsm_iterations_since_init > this->fsm_iterations_saved + this->clamp_unclamp_required_fsm_iterations;
}

/**
 * Should be called on every iteration of the Dice Payload task.
 * Executes the appropriate action corresponding to the current state.
 *
 * Returns NO_ERROR if everything went well.
 * Returns IMPOSSIBLE_STATE if the payload is in an impossible state (should never happen).
 * Otherwise, returns a state-specific error.
 */
error_code_t DicePayloadDriver::state_step(void)
{
    error_code_t error_code = error_code_t::NO_ERROR;

    // TODO: remove before flight
    Console::log("Substate: %d", this->sub_state);

    switch (this->state)
    {
    case state_t::INIT:
        error_code = this->init_action();
        break;
    case state_t::STOPPED:
        error_code = this->stopped_action();
        break;
    case state_t::POWER_ON:
        error_code = this->power_on_action();
        break;
    case state_t::CHECK_STATUS:
        error_code = this->check_status_action();
        break;
    case state_t::CLAMP:
        error_code = this->clamp_action();
        break;
    case state_t::TAKE_PICTURE:
        error_code = this->take_picture_action();
        break;
    case state_t::UNCLAMP:
        error_code = this->unclamp_action();
        break;
    case state_t::ERROR:
        error_code = this->error_action();
        break;
    default:
        error_code = error_code_t::IMPOSSIBLE_STATE;
        break;
    }

    if (error_code != error_code_t::NO_ERROR)
    {
        // TODO: do we want to go to error state on EVERY error?
        this->set_state(state_t::ERROR);
    }

    /* Increase number of iterations. */
    fsm_iterations_since_init++;

    return error_code;
}

/**
 * Initialises the dice payload driver, called once per power cycle.
 *
 * TODO: what to initialise here?
 */
error_code_t DicePayloadDriver::init_action(void)
{
    // TODO: remove before flight
    Console::log("Init Action");

    // TODO: initialise required things (what? maybe camera driver?)

    return this->set_state(state_t::STOPPED);
}


/**
 * Turns off the dice payload if it is powered and we want it to turn off.
 * A scenario where we don't want to turn it off could be when there is still a pending command to be
 * processed (i.e. we need to take another picture).
 *
 * We can only exit this state when an external task tells us to do so using the run_payload_sequence function.
 */
error_code_t DicePayloadDriver::stopped_action(void)
{
    // TODO: remove before flight
    Console::log("Stopped Action");

    error_code_t error_code = error_code_t::NO_ERROR;

    if (this->power_off_when_stopped || this->force_power_off)
    {
        // TODO: power off dice payload (requires EPS driver)
        // TODO: check if payload is actually powered off (requires EPS driver)
        this->payload_is_powered = false;
    }

    return error_code;
}

/**
 * Can only be entered when commanded by an external task.
 * Turns on the dice payload and checks if it is actually on.
 * If it is not on, signals an error and returns.
 * If the payload is turned on successfully, go to the CHECK_STATUS state.
 */
// TODO: do we want to check if there is enough charge left to do the whole sequence?
error_code_t DicePayloadDriver::power_on_action(void)
{
    // TODO: remove before flight
    Console::log("Power On Action");

    error_code_t error_code = error_code_t::NO_ERROR;

    if (!this->payload_is_powered)
    {
        // TODO: power on payload and check result (requires EPS driver)
        // TODO: check if payload is powered, if not, error. (requires EPS driver)

        this->payload_is_powered = true;
    }
    else
    {
        /* Go to CHECK_STATUS if the payload is powered. */
        // TODO: how long does it take for the payload to boot up? Do we need to wait?
        error_code = this->set_state(state_t::CHECK_STATUS);
    }

    return error_code;
}

/**
 * Checks if the payload responds to commands and if its status is OK.
 * Also checks motor position and switches, and corrects these when necessary.
 * If everything checks out, go to the CLAMP state.
 */
error_code_t DicePayloadDriver::check_status_action(void) // TODO: rename this state
{
    // TODO: remove before flight
    Console::log("Check Status Action");

    error_code_t error_code;
    motor1_position_t motor1_position;

    /* Attempt to get the status of the payload. */
    error_code = this->check_payload_status();

    /* Return error code if communication failed. */
    if (error_code != error_code_t::NO_ERROR)
    {
        return error_code;
    }

    /*** CHECK MOTOR 1 POSITION ***/
    if (this->sub_state == 0u)
    {
        // TODO: remove before flight
        Console::log("Check motor 1 position");

        /* Attempt to get the position of motor 1. */
        error_code = this->command_handler.read_motor1_position(motor1_position);

        /* Return error code if communication failed. */
        if (error_code != error_code_t::NO_ERROR)
        {
            return error_code;
        }

        /* Unclamp the dice when the motor is not unclamped. */
        if (motor1_position != motor1_position_t::UNCLAMPED)
        {
            error_code = this->command_handler.unclamp();

            /* Return error code if communication failed. */
            if (error_code != error_code_t::NO_ERROR)
            {
                return error_code;
            }
            /* Go to substate 2 if communication was successful. */
            else
            {
                this->clamp_unclamp_wait_start();
                this->sub_state = 1u;
            }
        }
        /* Motor is already unclamped, skip state 2 and go to state 3. */
        else
        {
            this->sub_state = 2u;
        }
    }
    /*** CHECK IF MOTOR IS CLAMPED ***/
    else if (this->sub_state == 1u)
    {
        /* Wait a set number of FSM iterations before checking motor position again. */
        if (this->clamp_unclamp_wait_expired())
        {
            // TODO: remove before flight
            Console::log("Motor is clamped, check if unclamped.");

            /* Attempt to get the position of motor 1. */
            error_code = this->command_handler.read_motor1_position(motor1_position);

            /* Return error code if communication failed. */
            if (error_code != error_code_t::NO_ERROR)
            {
                return error_code;
            }

            /* Error when motor not unclamped. */
            if (motor1_position != motor1_position_t::UNCLAMPED)
            {
                return error_code_t::UNCLAMPING_TIMEOUT;
            }
            /* Motor is unclamped, go to state 3. */
            else
            {
                this->sub_state = 2u;
            }
        }
    }
    /*** CHECK STATE OF THE SWITCHES ***/
    else if (this->sub_state == 2u)
    {
        // TODO: how to do this?

        this->sub_state = 3u;
    }
    /*** WRITE SETTINGS TO PAYLOAD ***/
    else /* if (this->sub_state >= 3u) */
    {
        // TODO: do we want to do this every time we start up? It will take some time to transmit everything over I2C.

        /* Go to the CLAMP state if we want to run the motors. Else, go to the OPERATIONAL state. */
        if (this->run_motors)
        {
            this->set_state(state_t::CLAMP);
        }
        else
        {
            this->set_state(state_t::TAKE_PICTURE);
        }
    }

    return error_code_t::NO_ERROR;
}

/**
 * Checks payload status and clamps the dice. This is a separate state so we can skip it if we don't want to run the motors.
 * Go to the OPERATIONAL state when the command was received by the payload.
 */
error_code_t DicePayloadDriver::clamp_action(void)
{
    // TODO: remove before flight
    Console::log("Clamp Action");

    error_code_t error_code;
    motor1_position_t motor1_position;

    /* Attempt to get the status of the payload. */
    error_code = this->check_payload_status();

    /* Return error code if communication failed. */
    if (error_code != error_code_t::NO_ERROR)
    {
        return error_code;
    }

    if (this->sub_state == 0u)
    {
        /* Clamp the dice. */
        error_code = this->command_handler.clamp();

        /* Return error code if communication failed. */
        if (error_code != error_code_t::NO_ERROR)
        {
            return error_code;
        }

        /* Save the current number of iterations so we can use it in the next state. */
        this->clamp_unclamp_wait_start();
        this->sub_state = 1u;
    }
    /* Wait until clamping has happened or timed out. */
    else /* if (this->sub_state >= 1u) */
    {
        if (this->clamp_unclamp_wait_expired())
        {
            /* Attempt to get the position of motor 1. */
            error_code = this->command_handler.read_motor1_position(motor1_position);

            /* Return error code if communication failed. */
            if (error_code != error_code_t::NO_ERROR)
            {
                return error_code;
            }

            /* Error when motor not clamped. */
            if (motor1_position != motor1_position_t::CLAMPED)
            {
                return error_code_t::CLAMPING_TIMEOUT;
            }

            /* Go to the next state. */
            this->set_state(state_t::TAKE_PICTURE);
        }
    }

    return error_code_t::NO_ERROR;
}

/**
 * Takes a picture using the camera.
 * If run_motors is true, goes to UNCLAMP. Otherwise, goes to STOPPED.
 */
error_code_t DicePayloadDriver::take_picture_action(void)
{
    // TODO: remove before flight
    Console::log("Operational Action");

    error_code_t error_code;

    /* Attempt to get the status of the payload. */
    error_code = this->check_payload_status();

    /* Return error code if communication failed. */
    if (error_code != error_code_t::NO_ERROR)
    {
        return error_code;
    }

    // TDOO: remove before flight
    Console::log("Take camera picture");

    // TODO: take picture
    // TODO: do we need to turn on the LED? Also every time?
    // TODO: how long do we need to wait until it actually took a picture?

    /* Go to sub-state 2 to unclamp. */
    if (run_motors)
    {
        this->set_state(state_t::UNCLAMP);
    }
    /* Go straight to STOPPED when we did not run the motors. */
    else
    {
        // TODO: Can we just unpower it without the camera losing power?
        this->set_state(state_t::STOPPED);
    }

    return error_code_t::NO_ERROR;
}

/**
 * Unclamp the motors and wait for them to unclamp.
 * Go to the STOPPED state if unclamping was successful.
 */
error_code_t DicePayloadDriver::unclamp_action(void)
{
    motor1_position_t motor1_position;

    /* Attempt to get the status of the payload. */
    error_code_t error_code = this->check_payload_status();

    /* Return error code if communication failed. */
    if (error_code != error_code_t::NO_ERROR)
    {
        return error_code;
    }

    /* Unclamp motors. */
    if (this->sub_state == 0u)
    {
        // TODO: remove before flight
        Console::log("Unclamp motors");

        /* Unclamp the dice. We check if it actually unclamped in the next commanded sequence. */
        error_code = this->command_handler.unclamp();

        /* Return error code if communication failed. */
        if (error_code != error_code_t::NO_ERROR)
        {
            return error_code;
        }

        /* Wait for unclamping to finish. */
        this->clamp_unclamp_wait_start();
        this->sub_state = 1u;
    }
    /* Check if motor is unclamped. */
    else /* if (this->sub_state >= 1u) */
    {
        if (this->clamp_unclamp_wait_expired())
        {
            /* Attempt to get the position of motor 1. */
            error_code = this->command_handler.read_motor1_position(motor1_position);

            /* Return error code if communication failed. */
            if (error_code != error_code_t::NO_ERROR)
            {
                return error_code;
            }

            /* Error when motor not clamped. */
            if (motor1_position != motor1_position_t::UNCLAMPED)
            {
                return error_code_t::UNCLAMPING_TIMEOUT;
            }

            /* Go to the next state. */
            this->set_state(state_t::STOPPED);
        }
    }

    return error_code_t::NO_ERROR;
}

/**
 * This action executes when the driver encounters an error.
 * Go to the STOPPED state and power off the payload there.
 */
error_code_t DicePayloadDriver::error_action(void)
{
    // TDOO: remove before flight
    Console::log("Error Action");

    // TODO: not sure yet what else to do here

    /* Power of the Dice Payload until the higher-level task finds a way to solve the error. */
    this->force_power_off = true;

    return this->set_state(state_t::STOPPED);
}
