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
    I2Cinternal.begin();

    Console::log("AN12 Testing Program\n\n");

        if (TEST_ANTENNA)
        {
        Console::log("AN12 test starting...\n");
        Console::log("Antenna address 0x31");
        }
        Console::log("\n");



        // delay before testing
        delay_ms(1000);



        // 0 : Reset Antenna (return PASS / FAIL)
        if (antennaA.reset()) {
            Console::log("Reset: FAIL\n");
        }
        else {
            Console::log("Reset: PASS\n");
        }
        Console::log("\n");



        // 1 : Check Temperature 
        if (antennaA.ping()) {
            Console::log("Ping - Temperature Sensor Check: FAIL\n");
        }
        else {
            Console::log("Ping - Temperature Sensor Check: PASS\n");
            Console::log("Antenna Temperature: %.2f C\n", antennaA.temperature.temp);
        }
        Console::log("\n");



        // 2.1 : Report Deployment Status 
        antennaA.report_deployment_status();
        
        // 2.2 Check Antenna is not armed
        if (antennaA.status.arm) {
            Console::log("Report arm Status now is armed - Arm Check: FAIL\n");
        }
        else {
            Console::log("Report arm Status now is not armed - Arm Check: PASS\n");
        }
        // 2.3 Check if all antennas are deployed
        if (antennaA.status.a1s == 0){
            Console::log("Antenna 1 deployed: PASS\n");
        }
        if (antennaA.status.a1s == 1){
            Console::log("Antenna 1 is not deployed: FAIL\n");
        }
        if (antennaA.status.a2s == 0){
            Console::log("Antenna 2 deployed: PASS\n");
        }
        if (antennaA.status.a2s == 1){
            Console::log("Antenna 2 is not deployed: FAIL\n");
        }
        if (antennaA.status.a3s == 0){
            Console::log("Antenna 3 deployed: PASS\n");
        }
        if (antennaA.status.a3s == 1){
            Console::log("Antenna 3 is not deployed: FAIL\n");
        }
        if (antennaA.status.a4s == 0){
            Console::log("Antenna 4 deployed: PASS\n");
        }
        if (antennaA.status.a4s == 1){
            Console::log("Antenna 4 is not deployed: FAIL\n");
        }

        // 2.4 Check that deployment systems are NOT ACTIVE
        if (antennaA.status.a1b == 0){
            Console::log("Antenna 1 deployment system is NOT ACTIVE: PASS\n");
        }
        if (antennaA.status.a1b == 1){
            Console::log("Antenna 1 deployment system is ACTIVE: FAIL\n");
        }
        if (antennaA.status.a2b == 0){
            Console::log("Antenna 2 deployment system is NOT ACTIVE: PASS\n");
        }
        if (antennaA.status.a2b == 1){
            Console::log("Antenna 2 deployment system is ACTIVE: FAIL\n");
        }
        if (antennaA.status.a3b == 0){
            Console::log("Antenna 3 deployment system is NOT ACTIVE: PASS\n");
        }
        if (antennaA.status.a3b == 1){
            Console::log("Antenna 3 deployment system is ACTIVE: FAIL\n");
        }
        if (antennaA.status.a4b == 0){
            Console::log("Antenna 4 deployment system is NOT ACTIVE: PASS\n");
        }
        if (antennaA.status.a4b == 1){
            Console::log("Antenna 4 deployment system is ACTIVE: FAIL\n");
        }
        Console::log("\n");



        // 3 : Report Deployment Activation Count it should be zero for all antennas (Using report_deployment_activation_time() )
        antennaA.report_deployment_activation_count();
        if (antennaA.deployment_activation_count.count[0] != 0 ||
            antennaA.deployment_activation_count.count[1] != 0 ||
            antennaA.deployment_activation_count.count[2] != 0 ||
            antennaA.deployment_activation_count.count[3] != 0) {
            Console::log("Report Deployment Activation Count Check: FAIL\n");
            Console::log("Antenna 1 deployment activation count: %d\n", antennaA.deployment_activation_count.count[0]);
            Console::log("Antenna 2 deployment activation count: %d\n", antennaA.deployment_activation_count.count[1]);
            Console::log("Antenna 3 deployment activation count: %d\n", antennaA.deployment_activation_count.count[2]);
            Console::log("Antenna 4 deployment activation count: %d\n", antennaA.deployment_activation_count.count[3]);
        }
        else {
            Console::log("Report Deployment Activation Count Check: PASS\n");
        }
        Console::log("\n");



        // 4 : Arm antenna system
        if (antennaA.arm()) {
            Console::log("Arm: FAIL\n");
        }
        else {
            Console::log("Arm: PASS\n");
        }
        Console::log("\n");


        // 5 : Start deployment
        Console::log("Starting automated sequential deployment...\n");
        antennaA.deploy_sequential(3);
        Console::log("Antenna automated deployment initiated.\n");
        delay_ms(3000);
        antennaA.report_deployment_status();
        int deployment_system_active_status = antennaA.status.a1b | antennaA.status.a2b | antennaA.status.a3b | antennaA.status.a4b;
        if (deployment_system_active_status == 0) {
            Console::log("Deployment system is not active\n");
        }
        while(deployment_system_active_status>0) {
            antennaA.report_deployment_status();
            bool IGNORED = antennaA.status.ig;
            Console::log("IGNORED status: %d\n, should be 0", IGNORED);
            Console::log("Antenna 1 deployment system active status: %d\n", antennaA.status.a1b);
            Console::log("Antenna 2 deployment system active status: %d\n", antennaA.status.a2b);
            Console::log("Antenna 3 deployment system active status: %d\n", antennaA.status.a3b);
            Console::log("Antenna 4 deployment system active status: %d\n", antennaA.status.a4b);
            delay_ms(2000);
        }
        Console::log("\n");


        // 6 : Print the temperature and check if above threshold
        if (antennaA.temp_above_threshold()) {
            Console::log("Temperature Above Threshold: FAIL\n");
        }
        else {
            Console::log("Temperature Above Threshold: PASS\n");
            Console::log("Antenna Temperature: %.2f C\n", antennaA.temperature.temp);
        }
        Console::log("\n");



        // 7 : For each antenna, do overide deployment and check IGNORED status and deployment status
        Console::log("Starting overide deployment for each antenna...\n");
        for (int i = 1; i <= 4; i++) {
            Console::log("Overide deploying Antenna %d...\n", i);
            antennaA.deploy(i, true, 3);
            delay_ms(3500);
            antennaA.report_deployment_status();
            bool IGNORED = antennaA.status.ig;
            Console::log("IGNORED status after overide deployment: %d\n", static_cast<int>(IGNORED));
            Console::log("Antenna %d deployed status: %d\n", i, 
                (i == 1) ? static_cast<int>(antennaA.status.a1s) :
                (i == 2) ? static_cast<int>(antennaA.status.a2s) :
                (i == 3) ? static_cast<int>(antennaA.status.a3s) :
                            static_cast<int>(antennaA.status.a4s));
            Console::log("\n");
        }



        // 8.1 : Print current status of all antennas
        antennaA.report_deployment_status();
        Console::log("Final Antenna Status Report:\n");
        Console::log("Antenna 1 deployed status: %d, should be 0\n", static_cast<int>(antennaA.status.a1s));
        Console::log("Antenna 2 deployed status: %d, should be 0\n", static_cast<int>(antennaA.status.a2s));
        Console::log("Antenna 3 deployed status: %d, should be 0\n", static_cast<int>(antennaA.status.a3s));
        Console::log("Antenna 4 deployed status: %d, should be 0\n", static_cast<int>(antennaA.status.a4s));
        Console::log("Antenna system armed status: %d, should be 1\n", static_cast<int>(antennaA.status.arm));

        // 8.2 Check all temperatures are printed
        antennaA.report_temperature();
        Console::log("Final Antenna Temperature Report:\n");
        Console::log("Antenna Temperature: %.2f C\n", antennaA.temperature.temp); 
        Console::log("\n");

        

        // 9 : Disarm antenna system
        if (antennaA.disarm()) {
            Console::log("Disarm: FAIL\n");
        }
        else {
            Console::log("Disarm: PASS\n");
        }
        Console::log("\n");



        // 10 : Check activation count (it should be > 0 for all antennas)
        antennaA.report_deployment_activation_count();
        Console::log("Final Deployment Activation Count Report:\n");
        Console::log("Antenna 1 deployment activation count: %d, should be >0\n", antennaA.deployment_activation_count.count[0]);
        Console::log("Antenna 2 deployment activation count: %d, should be >0\n", antennaA.deployment_activation_count.count[1]);
        Console::log("Antenna 3 deployment activation count: %d, should be >0\n", antennaA.deployment_activation_count.count[2]);
        Console::log("Antenna 4 deployment activation count: %d, should be >0\n", antennaA.deployment_activation_count.count[3]);
        Console::log("\n");


        // Finishing test
        Console::log("\nAN12 finished\n");
        // delay_ms(5000);

        while (1) {
            delay_ms(1000);
        }

    
}
