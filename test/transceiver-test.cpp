/*****************************************************************************
*
* Copyright (C) 2013 - 2017 Texas Instruments Incorporated - http://www.ti.com/
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the
*   distribution.
*
* * Neither the name of Texas Instruments Incorporated nor the names of
*   its contributors may be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*
* MSP432 empty main.c template
*
******************************************************************************/

#include <ti/devices/msp432p4xx/inc/msp.h>
#include "../src/DWire.h"
#include "../src/Console.h"
#include "../src/DelfiPQcore.h"
#include "../src/delay.h"
#include "../src/transmitter.h"
#include "../src/receiver.h"
#include "../src/I2CScanner.h"


#define TEST_TRANSMITTER true
#define TEST_RECEIVER true

DWire I2Cinternal(0);
Transmitter tx(I2Cinternal, TX_I2C_ADDRESS);
Receiver rx(I2Cinternal, RX_I2C_ADDRESS);


int main(void)
{
    // initialize the MCU:
    // - clock source
    // - clock tree
    DelfiPQcore::initMCU();

    delay_init();

    /* Init console with 9600 baud. */
    Console::init( 9600 );

    /* Delay. */
    delay_ms(1000);

    // Initialize I2C master
//    I2Cinternal.setFastMode();
    I2Cinternal.begin();

    uint32_t loop_iter = 0;

    Console::log("Transceiver Testing Program\n\n");


    while(1)
    {
       int devices = I2CScanner::scan(I2Cinternal);

       Console::log("%d devices found\n", devices);
       I2CScanner::scan(I2Cinternal, [](unsigned char n){ Console::log("%d \n", n);});

        if (TEST_TRANSMITTER)
        {
            char to_callsign[6] = {'D', 'V', 'S', 'G', 'N', 'D'};
            char from_callsign[6] = {'D', 'V', 'S', 'S', 'A', 'T'};
        
            Console::log("Transmitter test starting (Section 5.3)...\n");
        
            // Step: configure AX.25 callsigns before transmitting
            if (tx.set_default_to_callsign(to_callsign, 0)) {
                Console::log("SET TO CALLSIGN: FAIL\n");
            } else {
                Console::log("SET TO CALLSIGN: PASS\n");
            }
        
            if (tx.set_default_from_callsign(from_callsign, 0)) {
                Console::log("SET FROM CALLSIGN: FAIL\n");
            } else {
                Console::log("SET FROM CALLSIGN: PASS\n");
            }
        
            // Set transmitter bitrate, Stefano reccomended testing other bitrates as well
            if (tx.set_bitrate(9600)) {
                Console::log("TEST 1 (Set TX bitrate 9600 bps): FAIL\n");
            } else {
                Console::log("TEST 1 (Set TX bitrate 9600 bps): PASS\n");
            }
        
            // Send test frame "TEST" via I2C 
            uint8_t test_frame[4] = {'T', 'E', 'S', 'T'};
            if (tx.send_frame(test_frame, 4)) {
                Console::log("TEST 2 (Send TEST frame): FAIL\n");
            } else {
                Console::log("TEST 2 (Send TEST frame): PASS\n");
                Console::log("Remaining TX buffer size: %d, expected: 39\n", tx.buffer.free_slots);
            }
        
            delay_ms(2000);
        
            if (tx.report_last_telemetry()) {
                Console::log("TEST 3 (Telemetry test, last transmission): FAIL\n");
            } else {
                Console::log("TEST 3 (Telemetry test, last transmission): PASS\n");
                Console::log("RF forward power (mW): %f\n", tx.last_telemetry.rf_forward_power);
                Console::log("RF reflected power (mW): %f\n", tx.last_telemetry.rf_reflected_power);
                Console::log("Power bus voltage (V): %f\n", tx.last_telemetry.power_bus_voltage);
                Console::log("Total supply current (mA): %f\n", tx.last_telemetry.total_current);
                Console::log("PA temperature (C): %f\n", tx.last_telemetry.poweramp_temp);
                Console::log("LO temperature (C): %f\n", tx.last_telemetry.oscillator_temp);
            }
        
            if (tx.measure_telemetry()) {
                Console::log("TEST 4 (Instantaneous telemetry): FAIL\n");
            } else {
                Console::log("TEST 4 (Instantaneous telemetry): PASS\n");
                Console::log("RF forward power (mW): %f\n", tx.measured_telemetry.rf_forward_power);
                Console::log("RF reflected power (mW): %f\n", tx.measured_telemetry.rf_reflected_power);
                Console::log("Power bus voltage (V): %f\n", tx.measured_telemetry.power_bus_voltage);
                Console::log("Total supply current (mA): %f\n", tx.measured_telemetry.total_current);
                Console::log("PA temperature (C): %f\n", tx.measured_telemetry.poweramp_temp);
                Console::log("LO temperature (C): %f\n", tx.measured_telemetry.oscillator_temp);
            }
        
            Console::log("Transmitter test finished\n\n");
        }
        
        if (TEST_RECEIVER)
        {
            uint8_t hello_frame[5] = {'H', 'E', 'L', 'L', 'O'};
            bool match = true;
        
            Console::log("Receiver test starting...\n");
        
            if (rx.get_number_of_frames()) {
                Console::log("TEST 1 (Initial frame count): FAIL");
            } else {
                Console::log("TEST 1 (Initial frame count): PASS");
                Console::log("Number of frames in buffer: %d, expected: 0\n", rx.frames.number);
            }
        
            Console::log("Waiting 30 s for external HELLO frame...\n");
            delay_ms(30000);
        
            if (rx.get_number_of_frames()) {
                Console::log("TEST 2 (Frame count after HELLO): FAIL");
            } else {
                Console::log("TEST 2 (Frame count after HELLO): PASS");
                Console::log("Number of frames in buffer: %d, expected: > 0\n", rx.frames.number);
            }

            if (rx.get_frame_from_buffer()) {
                Console::log("TEST 3 (Read frame from buffer): FAIL\n");
            } else {
                Console::log("TEST 3 (Read frame from buffer): PASS\n");
                Console::log("Frame size: %d, expected: 5\n", rx.frame.size);
            
                bool match = (rx.frame.size == 5);
            
                for (int i = 0; i < 5 && match; i++) {
                    if (rx.frame.frame[i] != hello_frame[i]) {
                        match = false;
                    }
                }
            
                if (match) {
                    Console::log("TEST 4 (Payload == HELLO): PASS\n");
                } else {
                    Console::log("TEST 4 (Payload == HELLO): FAIL\n");
                    Console::log("Received bytes:\n");
                    for (int i = 0; i < rx.frame.size; i++) {
                        Console::log("Byte %d: %d, expected: %d\n", i, rx.frame.frame[i]);
                    }
                }
            }

                Console::log("Receiver test finished\n\n");
            }
       }


        delay_ms(5000);
        loop_iter++;
}
