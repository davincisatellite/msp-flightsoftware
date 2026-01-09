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

           Console::log("Transmitter test starting...\n");
           if (tx.reset_software()) {
               Console::log("TEST 1 (Soft Reset): FAIL");
           }
           else {
               Console::log("TEST 1 (Soft Reset): PASS");
           }

           delay_ms(2000);
           if (tx.reset_watchdog()) {
               Console::log("TEST 2 (Reset Watchdog): FAIL");
           }
           else {
               Console::log("TEST 2 (Reset Watchdog): PASS");
           }

           if (tx.reset_hardware()) {
               Console::log("TEST 3 (Hard Reset): FAIL");
           }
           else {
               Console::log("TEST 3 (Hard Reset): PASS");
           }

           delay_ms(2000);

           if (tx.set_default_to_callsign(to_callsign, 0)) {
               Console::log("TEST 4 (Set default TO Callsign): FAIL");
           }
           else {
               Console::log("TEST 4 (Set default TO Callsign): PASS");
           }

           if (tx.set_default_from_callsign(from_callsign, 0)) {
               Console::log("TEST 5 (Set default FROM Callsign): FAIL");
           }
           else {
               Console::log("TEST 5 (Set default FROM Callsign): PASS");
           }
           uint8_t frame[1] = {0b10101010};
           if (tx.send_frame(frame, 1)) {
               Console::log("TEST 6 (Send Frame): FAIL");
           }
           else {
               Console::log("TEST 6 (Send Frame): PASS");
               Console::log("Remaining TX buffer size: %d, expected: 39", tx.buffer.free_slots);
           }

           if (tx.send_frame_override_cs(frame, 1, to_callsign, 1, from_callsign, 1)) {
               Console::log("TEST 7 (Send Frame with override Callsign): FAIL");
           }
           else {
               Console::log("TEST 7 (Send Frame with override Callsign): PASS");
               Console::log("Remaining TX buffer size: %d, expected: 38", tx.buffer.free_slots);
           }

           uint8_t beacon[1] = {0b01010101};
           if (tx.set_beacon(beacon, 1, 60)) {
               Console::log("TEST 8 (Set Beacon): FAIL");
           }
           else {
               Console::log("TEST 8 (Set Beacon): PASS");
           }

           if (tx.set_beacon_override_cs(beacon, 1, 60, to_callsign, 1, from_callsign, 1)) {
               Console::log("TEST 9 (Set Beacon with override callsign): FAIL");
           }
           else {
               Console::log("TEST 9 (Set Beacon with override callsign): PASS");
           }


           if (tx.set_idle_mode(true)) {
               Console::log("TEST 10 (Set Idle Mode): FAIL");
           }
           else {
               Console::log("TEST 10 (Set Idle Mode): PASS");
           }

           if (tx.measure_telemetry()) {
               Console::log("TEST 11 (Measure Telemetry): FAIL");
           }
           else {
               Console::log("TEST 11 (Measure Telemetry): PASS");
               Console::log("Measured instantaneous RF reflected power (mW): %f", tx.measured_telemetry.rf_reflected_power);
               Console::log("Measured instantaneous RF forward power (mW): %f", tx.measured_telemetry.rf_forward_power);
               Console::log("Measured power bus voltage (V): %f", tx.measured_telemetry.power_bus_voltage);
               Console::log("Measured total supply current (mA): %f", tx.measured_telemetry.total_current);
               Console::log("Measured transmitter current (mA): %f", tx.measured_telemetry.tx_current);
               Console::log("Measured receiver current: (mA): %f", tx.measured_telemetry.rx_current);
               Console::log("Measured power amplifier current (mA): %f", tx.measured_telemetry.poweramp_current);
               Console::log("Measured power amplifier temperature (C): %f", tx.measured_telemetry.poweramp_temp);
               Console::log("Measured local oscillator temperature (C): %f", tx.measured_telemetry.oscillator_temp);
           }

           if (tx.report_last_telemetry()) {
               Console::log("TEST 12 (Report Telemetry during last transmission): FAIL");
           }
           else {
               Console::log("TEST 12 (Report Telemetry during last transmission): PASS");
               Console::log("Last instantaneous RF reflected power (mW): %f", tx.last_telemetry.rf_reflected_power);
               Console::log("Last instantaneous RF forward power (mW): %f", tx.last_telemetry.rf_forward_power);
               Console::log("Last power bus voltage (V): %f", tx.last_telemetry.power_bus_voltage);
               Console::log("Last total supply current (mA): %f", tx.last_telemetry.total_current);
               Console::log("Last transmitter current (mA): %f", tx.last_telemetry.tx_current);
               Console::log("Last receiver current (mA): %f", tx.last_telemetry.rx_current);
               Console::log("Last power amplifier current (mA): %f", tx.last_telemetry.poweramp_current);
               Console::log("Last power amplifier temperature (C): %f", tx.last_telemetry.poweramp_temp);
               Console::log("Last local oscillator temperature (C): %f", tx.last_telemetry.oscillator_temp);

           }

           if (tx.set_bitrate(9600)) {
               Console::log("TEST 13 (Set TX bitrate): FAIL");
           }
           else {
               Console::log("TEST 13 (Set TX bitrate): PASS");
           }


           if (tx.report_uptime()) {
               Console::log("TEST 14 (Report uptime): FAIL");
           }
           else {
               Console::log("TEST 14 (Report uptime): PASS");
               Console::log("TX uptime: %d s, expected: [2, 8] s", tx.uptime);
           }

           if (tx.report_state()) {
               Console::log("TEST 15 (Report state): FAIL");
           }
           else {
               Console::log("TEST 15 (Report state): PASS");
               Console::log("TX Idle state: %d, expected: 1", static_cast<int>(tx.state.on_idle));
               Console::log("Beacon active: %d, expected: 1", static_cast<int>(tx.state.beacon_active));
               Console::log("TX bitrate: %d, expected: 9600", tx.state.tx_bitrate);
           }

           if (tx.clear_beacon()) {
               Console::log("TEST 16 (Clear Beacon): FAIL");
           }
           else {
               Console::log("TEST 16 (Clear Beacon): PASS");
           }

           if (tx.set_tx_mode(2)) {
               Console::log("TEST 17 (Set TX Mode to Transponder): FAIL");
           }
           else {
               Console::log("TEST 17 (Set TX Mode to Transponder): PASS");
           }

           if (tx.set_rssi_transponder_threshold(100)) {
           Console::log("TEST 18 (Set RSSI Transponder Threshold): FAIL");
           }
           else {
           Console::log("TEST 18 (Set RSSI Transponder Threshold): PASS");
           }

           if (tx.set_tx_mode(1)) {
               Console::log("TEST 19 (Retrun TX Mode to Nominal): FAIL");
           }
           else {
               Console::log("TEST 19 (Retrun TX Mode to Nominal): PASS");
           }
           Console::log("Transmitter test finished\n\n");


       }

       if (TEST_RECEIVER)
       {
          Console::log("Receiver test starting...\n");
          if (rx.reset_software()) {
              Console::log("TEST 1 (Soft Reset): FAIL");
          }
          else {
              Console::log("TEST 1 (Soft Reset): PASS");
          }

          delay_ms(1000);
          if (rx.reset_watchdog()) {
              Console::log("TEST 2 (Reset Watchdog): FAIL");
          }
          else {
              Console::log("TEST 2 (Reset Watchdog): PASS");
          }

          if (rx.reset_hardware()) {
              Console::log("TEST 3 (Hard Reset): FAIL");
          }
          else {
              Console::log("TEST 3 (Hard Reset): PASS");
          }

          delay_ms(1000);

          if (rx.get_number_of_frames()) {
              Console::log("TEST 4 (Get number of frames in buffer): FAIL");
          }
          else {
              Console::log("TEST 4 (Get number of frames in buffer): PASS");
              Console::log("Number of frames in buffer: %d, expected: 0", rx.frames.number);
          }

          if (rx.remove_frame_from_buffer()) {
              Console::log("TEST 5 (Remove frame from buffer): FAIL");
          }
          else {
              Console::log("TEST 5 (Remove frame from buffer): PASS");
              Console::log("There is probably no frame in buffer; this command has no effect");
          }

          if (rx.measure_telemetry()) {
              Console::log("TEST 6 (Measure Telemetry): FAIL");
          }
          else {
              Console::log("TEST 6 (Measure Telemetry): PASS");
              Console::log("Measured instantaneous Doppler effect offset (Hz): %f", rx.telemetry.doppler_offset);
              Console::log("Measured instantaneous signal strength (dBm): -%f", -rx.telemetry.signal_strength);
              Console::log("Measured power bus voltage (V): %f", rx.telemetry.power_bus_voltage);
              Console::log("Measured total supply current (mA): %f", rx.telemetry.total_current);
              Console::log("Measured transmitter current (mA): %f", rx.telemetry.tx_current);
              Console::log("Measured receiver current (mA): %f", rx.telemetry.rx_current);
              Console::log("Measured power amplifier current (mA): %f", rx.telemetry.poweramp_current);
              Console::log("Measured power amplifier temperature (C): %f", rx.telemetry.poweramp_temp);
              Console::log("Measured local oscillator temperature (C): %f", rx.telemetry.oscillator_temp);
          }
          if (rx.report_uptime()) {
              Console::log("TEST 7 (Report uptime): FAIL");
          }
          else {
              Console::log("TEST 7 (Report uptime): PASS");
              Console::log("RX uptime: %d s, expected: [1, 4] s", rx.uptime);
          }

          Console::log("Receiver test finished\n\n");
       }


        delay_ms(5000);
        loop_iter++;
    }
}
