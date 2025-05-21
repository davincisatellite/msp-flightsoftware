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
#include "../src/antenna.h"
#include "../src/I2CScanner.h"


#define TEST_ANTENNA true

DWire I2Cinternal(0);
Antenna antennaA(I2Cinternal, ANTENNA_I2C_ADDRESS_MCA);
Antenna antennaB(I2Cinternal, ANTENNA_I2C_ADDRESS_MCB);



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

    Console::log("Antenna Testing Program\n\n");


    while(1)
    {
        int devices = I2CScanner::scan(I2Cinternal);
        Console::log("%d devices found", devices);
        I2CScanner::scan(I2Cinternal, [](unsigned char n){ Console::log("%d", n);});
        Console::log("\n");
        if (TEST_ANTENNA)
        {
            Console::log("Antenna A test starting...\n");
            Console::log("Antenna address 0x31");

            if (antennaB.reset()) {
                Console::log("TEST 1 (Reset): FAIL");
            }
            else {
                Console::log("TEST 1 (Reset): PASS");
            }

            antennaA.report_deployment_status();
            Console::log("Is the antenna system armed: %d", static_cast<int>(antennaA.status.arm));
            Console::log("Is there a status error: %d", static_cast<int>(antennaA.status.error));
            if (antennaA.arm()) {
                Console::log("TEST 2 (Arm): FAIL");
            }
            else {
                Console::log("TEST 2 (Arm): PASS");
            }
            antennaA.report_deployment_status();
            Console::log("Antenna 1 deployed: %d", static_cast<int>(antennaA.status.a1s));
            Console::log("Antenna 2 deployed: %d", static_cast<int>(antennaA.status.a2s));
            Console::log("Antenna 3 deployed: %d", static_cast<int>(antennaA.status.a3s));
            Console::log("Antenna 4 deployed: %d", static_cast<int>(antennaA.status.a4s));
            Console::log("Antenna 1 deployment system ready: %d", static_cast<int>(antennaA.status.a1b));
            Console::log("The antenna system independent burn status: %d", static_cast<int>(antennaA.status.indb));
            Console::log("Is the antenna system currently ignoring the antenna deployment switches: %d", static_cast<int>(antennaA.status.ig));
            Console::log("Is the antenna system armed: %d", static_cast<int>(antennaA.status.arm));
            delay_ms(100);
            if (antennaA.disarm()) {
                Console::log("TEST 3 (Disarm): FAIL");
            }
            else {
                Console::log("TEST 3 (Disarm): PASS");
            }
            if (antennaA.report_temperature()) {
                Console::log("TEST 4 (Temp): FAIL");
            }
            else {
                Console::log("Test 4 (Temp): PASS       temp = %d (millidegree C)",  static_cast<int>(antennaA.temperature.temp*1000));
            }
            if (antennaA.temp_above_threshold()) {
                Console::log("Test 5 (Temp_Threshold): PASS     Temp_Thresohold = %d (0 deg C)", static_cast<int>(ANTENNA_TEMPERATURE_THRESHOLD*1000));
            }
            else {
                Console::log("Test 5 (Temp_Threshold): FAIL");
            }
            if (antennaA.report_deployment_activation_count()) {
                Console::log("Test 6 (Deploy_Count): FAIL");
            }
            else {
                Console::log("Test 6 (Deploy_Count): PASS");
                for (int i=0; i<4; i++) {
                    Console::log("Antenna %d deploy activation count: %d", i+1, antennaA.deployment_activation_count.count[i]);
                }
            }
            delay_ms(100);
            antennaA.report_deployment_status();
            Console::log("Secondary deployment status check (delayed)");
            Console::log("Antenna 1 deployed: %d", static_cast<int>(antennaA.status.a1s));
            Console::log("Antenna 2 deployed: %d", static_cast<int>(antennaA.status.a2s));
            Console::log("Antenna 3 deployed: %d", static_cast<int>(antennaA.status.a3s));
            Console::log("Antenna 4 deployed: %d", static_cast<int>(antennaA.status.a4s));
            Console::log("Antenna 1 deployment system ready: %d", static_cast<int>(antennaA.status.a1b));
            Console::log("The antenna system independent burn status: %d", static_cast<int>(antennaA.status.indb));
            Console::log("Is the antenna system currently ignoring the antenna deployment switches: %d", static_cast<int>(antennaA.status.ig));
            Console::log("Is the antenna system armed: %d", static_cast<int>(antennaA.status.arm));
            Console::log("Is there a status error: %d", static_cast<int>(antennaA.status.error));
            if (antennaA.report_deployment_activation_time()) {
                Console::log("Test 7 (Deploy_Time): FAIL");
            }
            else {
                Console::log("Test 7 (Deploy_Time): PASS");
                for (int i=0; i<4; i++) {
                    Console::log("Antenna %d deploy activation time: %d", i+1, antennaA.deployment_activation_time.time[i]);
                }
            }
            Console::log("\nAntenna A test finished\n");

            delay_ms(3000);

            Console::log("Antenna B test starting...\n");
            if (antennaB.reset()) {
                Console::log("TEST 1 (Reset): FAIL");
            }
            else {
                Console::log("TEST 1 (Reset): PASS");
            }
            delay_ms(100);
            if (antennaB.arm()) {
                Console::log("TEST 2 (Arm): FAIL");
            }
            else {
                Console::log("TEST 2 (Arm): PASS");
            }
            antennaB.report_deployment_status();
            Console::log("Antenna I2C address: %d", ANTENNA_I2C_ADDRESS_MCA);
            Console::log("Antenna 1 deployed: %d", static_cast<int>(antennaB.status.a1s));
            Console::log("Antenna 2 deployed: %d", static_cast<int>(antennaB.status.a2s));
            Console::log("Antenna 3 deployed: %d", static_cast<int>(antennaB.status.a3s));
            Console::log("Antenna 4 deployed: %d", static_cast<int>(antennaB.status.a4s));
            Console::log("Antenna 1 deployment system ready: %d", static_cast<int>(antennaB.status.a1b));
            Console::log("The antenna system independent burn status: %d", static_cast<int>(antennaB.status.indb));
            Console::log("Is the antenna system currently ignoring the antenna deployment switches: %d", static_cast<int>(antennaB.status.ig));
            Console::log("Is the antenna system armed: %d \n", static_cast<int>(antennaB.status.arm));
            delay_ms(100);
            if (antennaB.disarm()) {
                Console::log("TEST 3 (Disarm): FAIL");
            }
            else {
                Console::log("TEST 3 (Disarm): PASS");
            }
            if (antennaB.report_temperature()) {
                Console::log("TEST 4 (Temp): FAIL");
            }
            else {
                Console::log("Test 4 (Temp): PASS       temp = %d (millidegree C)",  static_cast<int>(antennaB.temperature.temp*1000));
            }
            if (antennaB.temp_above_threshold()) {
                Console::log("Test 5 (Temp_Threshold): PASS     Temp_Thresohold = %d (millidegree C)", static_cast<int>(ANTENNA_TEMPERATURE_THRESHOLD*1000));
            }
            else {
                Console::log("Test 5 (Temp_Threshold): FAIL");
            }
            if (antennaB.report_deployment_activation_count()) {
                Console::log("Test 6 (Deploy_Count): FAIL");
            }
            else {
                Console::log("Test 6 (Deploy_Count): PASS");
                for (int i=0; i<4; i++) {
                    Console::log("Antenna %d deploy activation count: %d", i, antennaB.deployment_activation_count.count[i]);
                }
            }
            if (antennaB.report_deployment_activation_time()) {
                Console::log("Test 7 (Deploy_Time): FAIL");
            }
            else {
                Console::log("Test 7 (Deploy_Time): PASS");
                for (int i=0; i<4; i++) {
                    Console::log("Antenna %d deploy activation time: %d", i, antennaB.deployment_activation_time.time[i]);
                }
            }
            Console::log("Antenna B test finished\n\n");


            delay_ms(1000);
        }
        delay_ms(5000);
        loop_iter++;
    }
}
