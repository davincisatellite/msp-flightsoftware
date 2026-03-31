#include "../src/EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy
#include "../src/Console.h"
#include "../src/DelfiPQcore.h"
#include "../src/delay.h"
#include <cstdio>
#include "EPS_print.h"

/*
     For each config data type we will test the boundaries and a random value between them.
*/
bool test_getConfigParameterType() {
    int there_is_an_error=0;
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1000))!=Int8 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1A3C))!=Int8 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x1FFF))!=Int8) {
        Console::log("It fails on the ones with the top hex digit 1 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x2000))!=UInt8 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x203A))!=UInt8 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x2FFF))!=UInt8) {
        Console::log("It fails on the ones with the top hex digit 2 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x3000))!=Int16 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x33F7))!=Int16 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x3FFF))!=Int16) {
        Console::log("It fails on the ones with the top hex digit 3 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4000))!=UInt16 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4D1A))!=UInt16 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x4FFF))!=UInt16) {
        Console::log("It fails on the ones with the top hex digit 4 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x5000))!=Int32 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x509D))!=Int32 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x5FFF))!=Int32) {
        Console::log("It fails on the ones with the top hex digit 5 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x6000))!=UInt32 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x663B))!=UInt32 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x6FFF))!=UInt32) {
        Console::log("It fails on the ones with the top hex digit 6 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x7000))!=Float ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x70AA))!=Float ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x7FFF))!=Float) {
        Console::log("It fails on the ones with the top hex digit 7 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8000))!=Int64 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8D1A))!=Int64 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x8FFF))!=Int64) {
        Console::log("It fails on the ones with the top hex digit 8 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9000))!=UInt64 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9F61))!=UInt64 ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x9FFF))!=UInt64) {
        Console::log("It fails on the ones with the top hex digit 9 in getConfigParameterType");
        there_is_an_error=1;
    }
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0xA000))!=Double ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xA5E8))!=Double ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xAFFF))!=Double) {
        Console::log("It fails on the ones with the top hex digit A in getConfigParameterType");
        there_is_an_error=1;
    }
    //test some invalid values
    if (EPS::getConfigParameterType(static_cast<ConfigParameter>(0xB000))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0x0))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xFFF))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xC068))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xD3A2))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xEF03))!=Invalid ||
        EPS::getConfigParameterType(static_cast<ConfigParameter>(0xFFFF))!=Invalid) {
        Console::log("It fails on the invalid values in getConfigParameterType");
        there_is_an_error=1;
    }
    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_getAccessType() {
    int there_is_an_error=0;
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_LOTHR in getaccesstype");
        there_is_an_error=1;
    }
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_HITHR in getaccesstype");
        there_is_an_error=1;
    }
    //SAFETY_VOLT_LOTHR and SAFETY_VOLT_HITHR_USED have the same address so the result should be the same, ReadWrite.
    //See the Documentation for more details. The main idea is that they are treated the same.
    if (EPS::getAccessType(ConfigParameter::SAFETY_VOLT_LOTHR_USED) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_LOTHR_USED in getaccesstype");
        there_is_an_error=1;
    }
    if(EPS::getAccessType(ConfigParameter::SAFETY_VOLT_HITHR_USED) != ReadWrite) {
        Console::log("fail SAFETY_VOLT_HITHR_USED in getaccesstype");
        there_is_an_error=1;
    }
    //test the other values. if 0x?8?? -> ReadOnly, else ReadWrite

    //0x1???
    for (int i = 0x1000; i <= 0x100A; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x1000 and 0x100A in getaccesstype");
            there_is_an_error=1;
        }
    if(EPS::getAccessType(ConfigParameter::CONF_PARAM_CHANGED) != ReadOnly) {//0x1800
        Console::log("fail CONF_PARAM_CHANGED in getaccesstype");
        there_is_an_error=1;
    }
    //0x2???
    for (int i = 0x2000; i <= 0x2002; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x2000 and 0x2002 in getaccesstype");
            there_is_an_error=1;
        }
    for (int i = 0x2800; i <= 0x2803; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
            Console::log("It fails between 0x2800 and 0x2803 in getaccesstype");
            there_is_an_error=1;
        }
    //0x3???
    for (int i = 0x3000; i <= 0x3029; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x3000 and 0x3029 in getaccesstype");
            there_is_an_error=1;
        }
    for (int i = 0x3800; i <= 0x3801; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
            Console::log("It fails between 0x3800 and 0x3801 in getaccesstype");
            there_is_an_error=1;
        }
    //0x4???
    for (int i = 0x4000; i <= 0x4027; ++i)
        if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadWrite) {
            Console::log("It fails between 0x4000 and 0x4027 in getaccesstype");
            there_is_an_error=1;
        }
    //0x480D and 0x480E are ReadWrite, and we tested them at the beginning
    for (int i = 0x4800; i <= 0x481E; ++i)
        if (i!=0x480D && i!=0x480E)
            if(EPS::getAccessType(static_cast<ConfigParameter>(i)) != ReadOnly) {
                Console::log("It fails between 0x4800 and 0x481E (excluding 0x480D and 0x480E)  in getaccesstype");
                there_is_an_error=1;
            }
    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_get_param_length() {
    if( EPS::get_param_length(Int8) == 1 &&
        EPS::get_param_length(UInt8) == 1 &&
        EPS::get_param_length(Int16) == 2 &&
        EPS::get_param_length(UInt16) == 2 &&
        EPS::get_param_length(Int32) == 4 &&
        EPS::get_param_length(UInt32) == 4 &&
        EPS::get_param_length(Float) == 4 &&
        EPS::get_param_length(Int64) == 8 &&
        EPS::get_param_length(UInt64) == 8 &&
        EPS::get_param_length(Double) == 8 &&
        EPS::get_param_length(Invalid) == 0) {
        return true;
    }
    Console::log("fail test_get_param_length");
    return false;

}


// Each test is completely independent and can be compiled separately
// I used i2c_address = 0x20, which seems to be ok.

// HOW TO TEST:
// 1. Compile the project with one test at a time (e.g., rename main1 to main)
// 2. Each test follows the correct I2C communication pattern:
//    a) Initialize DWire (setFastMode, begin)
//    b) Write command (beginTransmission, write, endTransmission)
//    c) Wait for EPS to process (delay_ms)
//    d) Request data from slave (requestFrom)  <-- THIS WAS MISSING!
//    e) Read data (read)

// OBS: without requestFrom(), the RX buffer is empty and read() returns 0.
//
// VERIFICATION:
// - Each test returns: 1 = PASS, 0 = FAIL
// - You can check the return value even if Console is unreliable
// - If Console shows weird results, check the return value instead

/*
 * IMPORTANT DWire I2C RULES (MSP432):
 * - NEVER call requestFrom(addr, 1)
 * - ALWAYS read >= 2 bytes
 * - DWire.begin() must be called for I2C configuration
 * - Use endTransmission(false) for register reads
 * - Prefer burst reads
 *
 * Violating these WILL break I2C on real peripherals.
 */
void print_4_bytes_command(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid) {
    Console::log("Command: stid %x  ivid %x  cc %x  bid %x\n",stid,ivid,cc,bid);
}
void print_5_bytes_response(uint8_t stid, uint8_t ivid, uint8_t rc, uint8_t bid, uint8_t stat) {
    Console::log("Response: stid %x  ivid %x  rc %x  bid %x  stat %x\n",stid,ivid,rc,bid,stat);
}
void print_5_bytes_reply(EPS::standard_reply reply) {
    Console::log("Response: stid %x  ivid %x  rc %x  bid %x  stat %x  error %x\n",reply.stid,reply.ivid,reply.rc,reply.bid,reply.stat, reply.error);
}

// Test 6: Test NO_OPERATION command using EPS function
// Returns: 1 on success, 0 on failure
int main6() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 6 NO_OPERATION starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);
    if (!reply.error && reply.rc == 0x03 && reply.stat == 0x80) {
        Console::log("Test 6: PASS - NO_OPERATION command works\n");
        return 1; // Success
    } else {
        Console::log("Test 6: FAIL - NO_OPERATION returned error or wrong values\n");
        return 0; // Failure
    }
}

// Test 7: Test OUTPUT_BUS_CHANNEL_ON command using EPS method
// Returns: 1 on success, 0 on failure
int main7() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 7 OUTPUT_BUS_CHANNEL_ON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::output_bus_channel_on(wire,i2c_address, 0x03);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x17 && reply.stat == 0x80) {
        Console::log("Test 7: PASS - OUTPUT_BUS_CHANNEL_ON command on index 3 works\n");
        return 1; // Success
    } else {
        Console::log("Test 7: FAIL - OUTPUT_BUS_CHANNEL_ON command rejected\n");
        return 0; // Failure
    }
}

// Test 8: Test OUTPUT_BUS_CHANNEL_OFF command using EPS method
// Returns: 1 on success, 0 on failure
int main8() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 8 OUTPUT_BUS_CHANNEL_OFF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply0 = EPS::output_bus_channel_on(wire,i2c_address, 0x03);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    delay_ms(3000);

    EPS::standard_reply reply = EPS::output_bus_channel_off(wire,i2c_address, 0x03);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x19 && reply.stat == 0x80) {
        Console::log("Test 8: PASS - OUTPUT_BUS_CHANNEL_OFF command on index 1 works\n");
        return 1; // Success
    } else {
        Console::log("Test 8: FAIL - OUTPUT_BUS_CHANNEL_OFF command rejected\n");
        return 0; // Failure
    }
}




// Test 9: Test WATCHDOG command
// Returns: 1 on success, 0 on failure
int main9() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 9 WATCHDOG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::watchdog(wire, i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x07) {
        Console::log("Test 9: PASS - WATCHDOG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 9: FAIL - WATCHDOG returned error or wrong RC\n");
        return 0; // Failure
    }
}

// Test 10: Test CANCEL_OPERATION command
// Returns: 1 on success, 0 on failure
int main10() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 10 CANCEL_OPERATION starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::cancel_operation(wire, i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x05 && reply.stat == 0x80) {
        Console::log("Test 10: PASS - CANCEL_OPERATION command works\n");
        return 1; // Success
    } else {
        Console::log("Test 10: FAIL - CANCEL_OPERATION returned error\n");
        return 0; // Failure
    }
}

// Test 11: Test SYSTEM_RESET command
// Returns: 1 on success, 0 on failure
int main11() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 11 SYSTEM_RESET starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::system_reset(wire, i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    //in case we don't manage to read the response, we should take a look at the output (we should see 0xFF values)
    if (!reply.error && ((reply.rc == 0xAB && reply.stat == 0x80) || (reply.rc==0xFF && reply.stat == 0xFF))) {
        Console::log("Test 11: PASS - SYSTEM_RESET command works\n");
        return 1; // Success
    } else {
        Console::log("Test 11: FAIL - SYSTEM_RESET returned error\n");
        return 0; // Failure
    }
}
//Test turn on channel 3 + system reset
int main11_2(){
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 11_2 SYSTEM_RESET starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //turn on channel 3
    EPS::standard_reply reply2 = EPS::output_bus_channel_on(wire,i2c_address, 0x03);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply2);

    delay_ms(2000);
    Console::log("...\n");

    EPS::standard_reply reply = EPS::system_reset(wire, i2c_address);

    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);
    if (!reply.error && ((reply.rc == 0xAB && reply.stat == 0x80) || (reply.rc==0xFF && reply.stat == 0xFF))) {
        Console::log("Test 11_2: PASS - SYSTEM_RESET command works\n");
        return 1; // Success
    } else {
        Console::log("Test 11_2: FAIL - SYSTEM_RESET returned error\n");
        return 0; // Failure
    }
}

//Test 12: Test check in point
//not an actual test, just a method to see what is currently in the EPS.
int main12() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test Checking EPS State starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();


    delay_ms(20); //important

    uint8_t bytes_received = wire.requestFrom(i2c_address, 5);
    if (bytes_received == 5) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        uint8_t stat = wire.read();
        print_5_bytes_response(stid,ivid,rc,bid,stat);
        //safety check
        if (stid == 0xFF || rc == 0xFF) {
            Console::log("No valid response yet (0xFF). Waiting 100ms.");
            delay_ms(100);
        }
        Console::log("Re reading...\n");
        print_5_bytes_response(stid,ivid,rc,bid,stat);
        Console::log("Test 12: Pass\n");
        return 1;

    }
    else {
        Console::log("Test 12: Fail Too few Bytes received: %d\n", bytes_received);
        return 0;
    }
}


// Test 13: Test OUTPUT_BUS_GROUP_ON command using EPS method on channels 2,3,4,6,7 (no 0,1,5 because they were already on)
// Returns: 1 on success, 0 on failure
int main13() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 13 OUTPUT_BUS_GROUP_ON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply2 = EPS::system_reset(wire, i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply2);
    if (!reply2.error && ((reply2.rc == 0xAB && reply2.stat == 0x80) || (reply2.rc==0xFF && reply2.stat == 0xFF))) {
        Console::log("Test 13: update: SYSTEM_RESET command worked");
        // return 1; // Success
    } else {
        Console::log("Test 13: FAIL in the middle of the test: SYSTEM_RESET returned error");
        return 0; // Failure
    }
    Console::log("...\n");
    delay_ms(3000);

    //0b00000001 11111111 -> is good
    EPS::standard_reply reply = EPS::output_bus_group_on(wire,i2c_address, 0x01FF);//0x00DC

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x11 && reply.stat == 0x80) {
        Console::log("Test 13: PASS - OUTPUT_BUS_GROUP_ON command works\n");
        return 1; // Success
    } else {
        Console::log("Test 13: FAIL - OUTPUT_BUS_GROUP_ON command rejected\n");
        return 0; // Failure
    }
}

// Test 14: Test OUTPUT_BUS_GROUP_OFF command using EPS method on channels 2,3,4,6,7 (no 0,1,5)
// Returns: 1 on success, 0 on failure
int main14() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 14 OUTPUT_BUS_GROUP_OFF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //0b00000001 11011100 -> is good
    EPS::standard_reply reply = EPS::output_bus_group_off(wire,i2c_address, 0x01DC);//0x00DC

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x13 && reply.stat == 0x80) {
        Console::log("Test 14: PASS - OUTPUT_BUS_GROUP_OFF command works\n");
        return 1; // Success
    } else {
        Console::log("Test 14: FAIL - OUTPUT_BUS_GROUP_OFF command rejected\n");
        return 0; // Failure
    }
}
// Test 15: Test OUTPUT_BUS_GROUP_STATE command using EPS method on channels 2,3,4,6,7,8 (no 0,1,5)
// Returns: 1 on success, 0 on failure
int main15() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 15 OUTPUT_BUS_GROUP_STATE starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //0b00000001 01100111 -> 0,1,2,5,6,8 are on and 3,4 7 are off
    EPS::standard_reply reply = EPS::output_bus_group_state(wire,i2c_address, 0x0167);//0x0167

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x15 && reply.stat == 0x80) {
        Console::log("Test 15: PASS - OUTPUT_BUS_GROUP_STATE command works\n");
        return 1; // Success
    } else {
        Console::log("Test 15: FAIL - OUTPUT_BUS_GROUP_STATE command rejected\n");
        return 0; // Failure
    }
}
/*

EPS Test 13 OUTPUT_BUS_GROUP_ON starting

read: 255 255 255 255 255
Response: stid FF  ivid FF  rc FF  bid FF  stat FF  error 0

...

Response: stid 30  ivid 30  rc 0  bid 0  stat 4  error 1

Test 13: FAIL - OUTPUT_BUS_GROUP_ON command rejected

*/
// Test 17: Test GET_SYSTEM_STATUS command using EPS method
// Returns: 1 on success, 0 on failure
int main17() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 17 GET_SYSTEM_STATUS starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::system_status_reply reply = EPS::get_system_status(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[36];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,36);
    print_array(eps_buffer, 36);

    print_system_status(reply);

    if (!reply.error && reply.rc == 0x41 && reply.stat == 0x80) {
        Console::log("Test 17: PASS - GET_SYSTEM_STATUS command works\n");
        return 1; // Success
    } else {
        Console::log("Test 17: FAIL - GET_SYSTEM_STATUS command rejected\n");
        return 0; // Failure
    }
}
// Test 18: Test GET_OVERCURRENT_FAULT_STATE command using EPS method
// Returns: 1 on success, 0 on failure
int main18() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 18 GET_OVERCURRENT_FAULT_STATE starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::overcurrent_reply reply = EPS::get_overcurrent_fault_state(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[42];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 42);
    print_array(eps_buffer, 42);
    print_overcurrent_reply(reply);

    if (!reply.error && reply.rc == 0x43 && reply.stat == 0x80) {
        Console::log("Test 18: PASS - GET_OVERCURRENT_FAULT_STATE command works\n");
        return 1; // Success
    } else {
        Console::log("Test 18: FAIL - GET_OVERCURRENT_FAULT_STATE command rejected\n");
        return 0; // Failure
    }
}
// Test 19: Test GET_PBU_ABF_PLACED_STATE command using EPS method
// Returns: 1 on success, 0 on failure
int main19() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 19 GET_PBU_ABF_PLACED_STATE starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pbu_abf_placed_state reply = EPS::get_pbu_abf_placed_state(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[8];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 8);
    print_array(eps_buffer, 8);

    print_pbu_abf_placed_state(reply);

    if (!reply.error && reply.rc == 0x45 && reply.stat == 0x80) {
        Console::log("Test 19: PASS - GET_PBU_ABF_PLACED_STATE command works\n");
        return 1; // Success
    } else {
        Console::log("Test 19: FAIL - GET_PBU_ABF_PLACED_STATE command rejected\n");
        return 0; // Failure
    }
}
// Test 20: Test GET_PDU_HOUSEKEEPING_DATA_RAW command using EPS method
// Returns: 1 on success, 0 on failure
int main20() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 20 GET_PDU_HOUSEKEEPING_DATA_RAW starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_raw(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[158];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 158);
    print_array(eps_buffer, 158);

    print_pdu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x51 && reply.stat == 0x80) {
        Console::log("Test 20: PASS - GET_PDU_HOUSEKEEPING_DATA_RAW command works\n");
        return 1; // Success
    } else {
        Console::log("Test 20: FAIL - GET_PDU_HOUSEKEEPING_DATA_RAW command rejected\n");
        return 0; // Failure
    }
}
// Test 21: Test GET_PDU_HOUSEKEEPING_DATA_ENG command using EPS method
// Returns: 1 on success, 0 on failure
int main21() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 21 GET_PDU_HOUSEKEEPING_DATA_ENG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_eng(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[158];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 158);
    print_array(eps_buffer, 158);

    print_pdu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x53 && reply.stat == 0x80) {
        Console::log("Test 21: PASS - GET_PDU_HOUSEKEEPING_DATA_ENG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 21: FAIL - GET_PDU_HOUSEKEEPING_DATA_ENG command rejected\n");
        return 0; // Failure
    }
}
// Test 22: Test GET_PDU_HOUSEKEEPING_DATA_AVG command using EPS method
// Returns: 1 on success, 0 on failure
int main22() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 22 GET_PDU_HOUSEKEEPING_DATA_AVG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pdu_housekeeping_data_reply reply = EPS::get_pdu_housekeeping_data_avg(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[158];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 158);
    print_array(eps_buffer, 158);

    print_pdu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x55 && reply.stat == 0x80) {
        Console::log("Test 22: PASS - GET_PDU_HOUSEKEEPING_DATA_AVG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 22: FAIL - GET_PDU_HOUSEKEEPING_DATA_AVG command rejected\n");
        return 0; // Failure
    }
}
// Test 23: Test GET_PBU_HOUSEKEEPING_DATA_RAW command using EPS method
// Returns: 1 on success, 0 on failure
int main23() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 23 GET_PBU_HOUSEKEEPING_DATA_RAW starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_raw(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[84];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 84);
    print_array(eps_buffer, 84);

    print_pbu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x61 && reply.stat == 0x80) {
        Console::log("Test 23: PASS - GET_PBU_HOUSEKEEPING_DATA_RAW command works\n");
        return 1; // Success
    } else {
        Console::log("Test 23: FAIL - GET_PBU_HOUSEKEEPING_DATA_RAW command rejected\n");
        return 0; // Failure
    }
}
// Test 24: Test GET_PBU_HOUSEKEEPING_DATA_ENG command using EPS method
// Returns: 1 on success, 0 on failure
int main24() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 24 GET_PBU_HOUSEKEEPING_DATA_ENG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_eng(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[84];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 84);
    print_array(eps_buffer, 84);

    print_pbu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x63 && reply.stat == 0x80) {
        Console::log("Test 24: PASS - GET_PBU_HOUSEKEEPING_DATA_ENG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 24: FAIL - GET_PBU_HOUSEKEEPING_DATA_ENG command rejected\n");
        return 0; // Failure
    }
}
// Test 25: Test GET_PBU_HOUSEKEEPING_DATA_AVG command using EPS method
// Returns: 1 on success, 0 on failure
int main25() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 25 GET_PBU_HOUSEKEEPING_DATA_AVG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pbu_housekeeping_data_reply reply = EPS::get_pbu_housekeeping_data_avg(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[84];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 84);
    print_array(eps_buffer, 84);

    print_pbu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x65 && reply.stat == 0x80) {
        Console::log("Test 25: PASS - GET_PBU_HOUSEKEEPING_DATA_AVG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 25: FAIL - GET_PBU_HOUSEKEEPING_DATA_AVG command rejected\n");
        return 0; // Failure
    }
}
// Test 26: Test GET_PCU_HOUSEKEEPING_DATA_RAW command using EPS method
// Returns: 1 on success, 0 on failure
int main26() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 26 GET_PCU_HOUSEKEEPING_DATA_RAW starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_raw(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[72];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 72);
    print_array(eps_buffer, 72);

    print_pcu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x71 && reply.stat == 0x80) {
        Console::log("Test 26: PASS - GET_PCU_HOUSEKEEPING_DATA_RAW command works\n");
        return 1; // Success
    } else {
        Console::log("Test 26: FAIL - GET_PCU_HOUSEKEEPING_DATA_RAW command rejected\n");
        return 0; // Failure
    }
}
// Test 27: Test GET_PCU_HOUSEKEEPING_DATA_ENG command using EPS method
// Returns: 1 on success, 0 on failure
int main27() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 27 GET_PCU_HOUSEKEEPING_DATA_ENG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_eng(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[72];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 72);
    print_array(eps_buffer, 72);

    print_pcu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x73 && reply.stat == 0x80) {
        Console::log("Test 27: PASS - GET_PCU_HOUSEKEEPING_DATA_ENG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 27: FAIL - GET_PCU_HOUSEKEEPING_DATA_ENG command rejected\n");
        return 0; // Failure
    }
}
// Test 28: Test GET_PCU_HOUSEKEEPING_DATA_AVG command using EPS method
// Returns: 1 on success, 0 on failure
int main28() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 28 GET_PCU_HOUSEKEEPING_DATA_AVG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::pcu_housekeeping_data_reply reply = EPS::get_pcu_housekeeping_data_avg(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[72];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, 72);
    print_array(eps_buffer, 72);

    print_pcu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0x75 && reply.stat == 0x80) {
        Console::log("Test 28: PASS - GET_PCU_HOUSEKEEPING_DATA_AVG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 28: FAIL - GET_PCU_HOUSEKEEPING_DATA_AVG command rejected\n");
        return 0; // Failure
    }
}
// Test 29: Test GET_PIU_HOUSEKEEPING_DATA_RAW command using EPS method
// Returns: 1 on success, 0 on failure
int main29() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 29 GET_PIU_HOUSEKEEPING_DATA_RAW starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::piu_housekeeping_data_reply reply = EPS::get_piu_housekeeping_data_raw(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[116];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,116);
    print_array(eps_buffer, 116);

    print_piu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0xA1 && reply.stat == 0x80) {
        Console::log("Test 29: PASS - GET_PIU_HOUSEKEEPING_DATA_RAW command works\n");
        return 1; // Success
    } else {
        Console::log("Test 29: FAIL - GET_PIU_HOUSEKEEPING_DATA_RAW command rejected\n");
        return 0; // Failure
    }
}
// Test 30: Test GET_PIU_HOUSEKEEPING_DATA_ENG command using EPS method
// Returns: 1 on success, 0 on failure
int main30() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 30 GET_PIU_HOUSEKEEPING_DATA_ENG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::piu_housekeeping_data_reply reply = EPS::get_piu_housekeeping_data_eng(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[116];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,116);
    print_array(eps_buffer, 116);

    print_piu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0xA3 && reply.stat == 0x80) {
        Console::log("Test 30: PASS - GET_PIU_HOUSEKEEPING_DATA_ENG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 30: FAIL - GET_PIU_HOUSEKEEPING_DATA_ENG command rejected\n");
        return 0; // Failure
    }
}
// Test 31: Test GET_PIU_HOUSEKEEPING_DATA_AVG command using EPS method
// Returns: 1 on success, 0 on failure
int main31() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 31 GET_PIU_HOUSEKEEPING_DATA_AVG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::piu_housekeeping_data_reply reply = EPS::get_piu_housekeeping_data_avg(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[116];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,116);
    print_array(eps_buffer, 116);

    print_piu_housekeeping_data_reply(reply);

    if (!reply.error && reply.rc == 0xA5 && reply.stat == 0x80) {
        Console::log("Test 31: PASS - GET_PIU_HOUSEKEEPING_DATA_AVG command works\n");
        return 1; // Success
    } else {
        Console::log("Test 31: FAIL - GET_PIU_HOUSEKEEPING_DATA_AVG command rejected\n");
        return 0; // Failure
    }
}
// Test 32G: Test GET_CONF_PARAM on TTC_WDG_TIMEOUT command using EPS method
// Returns: 1 on success, 0 on failure
int main32G() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 32G GET_CONF_PARAM on TTC_WDG_TIMEOUT starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::config_reply reply = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x83 && reply.stat == 0x80 && reply.par_id==ConfigParameter::TTC_WDG_TIMEOUT) {
        Console::log("Test 32G: PASS - GET_CONF_PARAM on TTC_WDG_TIMEOUT command works\n");
        return 1; // Success
    } else {
        Console::log("Test 32G: FAIL - GET_CONF_PARAM on TTC_WDG_TIMEOUT command rejected\n");
        return 0; // Failure
    }
}
// Test 32S: Test SET_CONF_PARAM on TTC_WDG_TIMEOUT command using EPS method
// Returns: 1 on success, 0 on failure
int main32S() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 32S SET_CONF_PARAM on TTC_WDG_TIMEOUT starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    ReturnType param_value{};
    param_value.ui16 = 21600;//6hours
    EPS::config_reply reply = EPS::set_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT, param_value);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x85 && reply.stat == 0x80 && reply.par_id==ConfigParameter::TTC_WDG_TIMEOUT) {
        Console::log("Test 32S: PASS - SET_CONF_PARAM on TTC_WDG_TIMEOUT command works\n");
        //return 1; // Success
    } else {
        Console::log("Test 32S: FAIL - SET_CONF_PARAM on TTC_WDG_TIMEOUT command rejected\n");
        return 0; // Failure
    }

    //try to get it:
    EPS::config_reply reply2 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply2);
    if (!reply2.error && reply2.rc == 0x83 && reply2.stat == 0x80 && reply2.par_id==ConfigParameter::TTC_WDG_TIMEOUT && reply2.par_value.ui16==21600) {
        Console::log("Test 32S: PASS - GET_CONF_PARAM correctly updated TTC_WDG_TIMEOUT\n");
        return 1; // Success
    } else {
        Console::log("Test 32S: FAIL - GET_CONF_PARAM (TTC_WDG_TIMEOUT) was not updated/read command rejected\n");
        return 0; // Failure
    }
}
// Test 32R: Test RESET_CONF_PARAM on TTC_WDG_TIMEOUT command using EPS method
// Returns: 1 on success, 0 on failure
int main32R() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 32R RESET_CONF_PARAM on TTC_WDG_TIMEOUT starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::config_reply reply = EPS::reset_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x87 && reply.stat == 0x80 && reply.par_id==ConfigParameter::TTC_WDG_TIMEOUT) {
        Console::log("Test 32R: PASS - SET_CONF_PARAM on TTC_WDG_TIMEOUT command works\n");
        //return 1; // Success
    } else {
        Console::log("Test 32R: FAIL - SET_CONF_PARAM on TTC_WDG_TIMEOUT command rejected\n");
        return 0; // Failure
    }

    //try to see if it was reset:
    EPS::config_reply reply2 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply2);
    //300 is the default value
    if (!reply2.error && reply2.rc == 0x83 && reply2.stat == 0x80 && reply2.par_id==ConfigParameter::TTC_WDG_TIMEOUT && reply2.par_value.ui16==300) {
        Console::log("Test 32R: PASS - GET_CONF_PARAM correctly updated TTC_WDG_TIMEOUT\n");
        return 1; // Success
    } else {
        Console::log("Test 32R: FAIL - GET_CONF_PARAM (TTC_WDG_TIMEOUT) was not updated/read command rejected\n");
        return 0; // Failure
    }
}
// Test 33: Test SWITCH to safety mode command using EPS method
// Returns: 1 on success, 0 on failure
int main33() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 33 switch_safety_mode starting\n");
    print_command(STID,IVID,static_cast<uint16_t>(CommandCode::SWITCH_TO_SAFETY_MODE),BID);

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::switch_safety_mode(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply);

    if (!reply.error && reply.rc == 0x33 && reply.stat == 0x80) {
        Console::log("Test 33: PASS - switch_safety_mode command works\n");
        return 1; // Success
    } else {
        Console::log("Test 33: FAIL - switch_safety_mode command rejected\n");
        return 0; // Failure
    }
    //use main17 to test system_status
}
// Test 34: Test SWITCH to nominal mode command using EPS method
// Returns: 1 on success, 0 on failure
int main34() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 34 switch_nominal_mode starting\n");
    print_command(STID,IVID,static_cast<uint16_t>(CommandCode::SWITCH_NOMINAL_MODE),BID);
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::switch_nominal_mode(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply);

    if (!reply.error && reply.rc == 0x31 && reply.stat == 0x80) {
        Console::log("Test 34: PASS - switch_nominal_mode command works\n");
        return 1; // Success
    } else {
        Console::log("Test 34: FAIL - switch_nominal_mode command rejected\n");
        return 0; // Failure
    }
    //use main17 to test system_status
}
// Test 35: Test load_configuration command to see if it is accepted
// Returns: 1 on success, 0 on failure
int main35() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 35 load_configuration starting\n");
    print_command(STID,IVID,static_cast<uint16_t>(CommandCode::LOAD_CONFIGURATION),BID);
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::load_configuration(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply);

    if (!reply.error && reply.rc == 0x93 && reply.stat == 0x80) {
        Console::log("Test 35: PASS - load_configuration command works\n");
        return 1; // Success
    } else {
        Console::log("Test 35: FAIL - load_configuration command rejected\n");
        return 0; // Failure
    }
}
// Test 36: Test reset_configuration command to see if it is accepted
// Returns: 1 on success, 0 on failure
int main36() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 36 reset_configuration starting\n");
    print_command(STID,IVID,static_cast<uint16_t>(CommandCode::RESET_CONFIGURATION),BID);
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::reset_configuration(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply);

    if (!reply.error && reply.rc == 0x91 && reply.stat == 0x80) {
        Console::log("Test 36: PASS - reset_configuration command works\n");
        return 1; // Success
    } else {
        Console::log("Test 36: FAIL - reset_configuration command rejected\n");
        return 0; // Failure
    }
}
// Test 37: Test save_configuration command to see if it is accepted
// Returns: 1 on success, 0 on failure
int main37() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 37 save_configuration starting\n");
    Console::log("STID: %x | IVID: %x | CC: %x | BID: %x | CONF_KEY: %x | CHECKSUM_2_bytes: %x%x", STID, IVID, CommandCode::SAVE_CONFIGURATION, BID, CONF_KEY, 0,0);
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::save_configuration(wire,i2c_address);

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply);

    if (!reply.error && reply.rc == 0x95 && reply.stat == 0x80) {
        Console::log("Test 37: PASS - save_configuration command works\n");
        return 1; // Success
    } else {
        Console::log("Test 37: FAIL - save_configuration command rejected\n");
        return 0; // Failure
    }
}
// Test C1: Try to modify the watchdog parameter to 200 instead of 300 and then save the config. load it and see if it was saved
// Returns: 1 on success, 0 on failure
int mainC1() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Complex Test c1: modify watchdog param, save it and load it starting\n");
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //initial get
    EPS::config_reply reply1 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    uint8_t eps_buffer[16];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply1);
    if(reply1.error || reply1.rc!=0x83 || reply1.stat!=0x80){
        Console::log("Test C1: FAIL - initial get param command for watchdog failed\n");
        return 0; // Failure
    }

    //modify the param
    ReturnType param_value{};
    param_value.ui16 = 200;
    EPS::config_reply reply2 = EPS::set_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT, param_value);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply2);
    if(reply2.error || reply2.rc!=0x85 || reply2.stat!=0x80){
        Console::log("Test C1: FAIL - set param command for watchdog failed\n");
        return 0; // Failure
    }
    //save the configuration
    EPS::standard_reply reply3 = EPS::save_configuration(wire,i2c_address);

    delay_ms(100);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_standard_reply(reply3);
    if(reply3.error || reply3.rc!=0x95 || reply3.stat!=0x80){
        Console::log("Test C1: FAIL - save configuration command failed\n");
        return 0; // Failure
    }
    delay_ms(1000);
    //load the configuration
    EPS::standard_reply reply4 = EPS::load_configuration(wire,i2c_address);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_standard_reply(reply4);
    if(reply4.error || reply4.rc!=0x93 || reply4.stat!=0x80){
        Console::log("Test C1: FAIL - load configuration command failed\n");
        return 0; // Failure
    }
    //final get
    EPS::config_reply reply5 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply5);
    if(reply5.error || reply5.rc!=0x83 || reply5.stat!=0x80){
        Console::log("Test C1: FAIL - final get param command for watchdog failed\n");
        return 0; // Failure
    }
    Console::log("Test C1: PASS - The parameter was successfully read,modified,saved,loaded and read\n");
    return 1;
}

// Test C2: Try to turn on all the buses, switch to safety mode and see if the buses are off.
// Returns: 1 on success, 0 on failure
int mainC2() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Complex Test C2: Try to turn on all the buses, switch to safety mode and see if the buses are of\n");
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //turn on the buses
    EPS::standard_reply reply1 = EPS::output_bus_group_on(wire,i2c_address, 0x01FF);//0x00DC

    delay_ms(50);
    uint8_t eps_buffer[5];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply1);

    if (reply1.error || reply1.rc != 0x11 || reply1.stat != 0x80){
        Console::log("Test C2: FAIL - turning on buses failed\n");
        return 0; // Failure
    }

    delay_ms(5000);
    //go to safety
    EPS::standard_reply reply2 = EPS::switch_safety_mode(wire,i2c_address);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply2);

    if (reply2.error | reply2.rc != 0x33 || reply2.stat != 0x80) {
        Console::log("Test C2: FAIL - switching to safety failed\n");
        return 0; // Failure
    }
    return 1;
}
// Test C3: Test SET_CONF_PARAM on TTC_WDG_TIMEOUT command using EPS method
// Returns: 1 on success, 0 on failure
int mainC3() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test C3. SET watchdog timer to 70 then turn on the buses then wait 70 seconds to see if they are off because of the watchdog\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //set watchdog timer
    ReturnType param_value{};
    param_value.ui16 = 70;
    EPS::config_reply reply = EPS::set_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT, param_value);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x85 && reply.stat == 0x80 && reply.par_id==ConfigParameter::TTC_WDG_TIMEOUT) {
        Console::log("Test C3: PASS - SET_CONF_PARAM on TTC_WDG_TIMEOUT command works\n");
        //return 1; // Success
    } else {
        Console::log("Test C3: FAIL - SET_CONF_PARAM on TTC_WDG_TIMEOUT command rejected\n");
        return 0; // Failure
    }
    //turn on all buses
    EPS::standard_reply reply2 = EPS::output_bus_group_on(wire,i2c_address, 0x01FF);//0x00DC

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_5_bytes_reply(reply2);

    if (reply2.error || reply2.rc != 0x11 || reply.stat != 0x80){
        Console::log("Test C3: FAIL - turning buses on failed\n");
        return 0; // Failure
    }
    Console::log("Test C3: waiting 70 seconds - check bus channels!");
    //wait 70 and see if they are off
    delay_ms(70000);
    Console::log("Test C3: PASS - check bus channels!\n");
    return 1;
}
// Test C4: Fter TestC1, run this to reset_configuration instead of 300 and then save the config. load it and see if it was saved
// Returns: 1 on success, 0 on failure
int mainC4() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Complex Test c4: modify watchdog param, save it and load it starting\n");
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //initial get
    EPS::config_reply reply1 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply1);
    if(reply1.error || reply1.rc!=0x83 || reply1.stat!=0x80){
        Console::log("Test C4: FAIL - initial get param command for watchdog failed\n");
        return 0; // Failure
    }
    //reset the configuration
    EPS::standard_reply reply3 = EPS::reset_configuration(wire,i2c_address);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply3);
    if(reply3.error || reply3.rc!=0x91 || reply3.stat!=0x80){
        Console::log("Test C4: FAIL - reset configuration command failed\n");
        return 0; // Failure
    }
    delay_ms(1000);
    //second get
    EPS::config_reply reply35 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply35);
    if(reply35.error || reply35.rc!=0x83 || reply35.stat!=0x80){
        Console::log("Test C4: FAIL - second get param command for watchdog failed\n");
        return 0; // Failure
    }
    //save to hard disk
    // //save the configuration
    // EPS::standard_reply reply41 = EPS::save_configuration(wire,i2c_address);
    // print_standard_reply(reply41);
    // if(reply41.error || reply41.rc!=0x95 || reply41.stat!=0x80){
    //     Console::log("Test C4: FAIL - save configuration command failed");
    //     return 0; // Failure
    // }
    // //third get
    // EPS::config_reply reply351 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);
    // print_config_reply(reply351);
    // if(reply351.error || reply351.rc!=0x83 || reply351.stat!=0x80){
    //     Console::log("Test C4: FAIL - third get param command for watchdog failed");
    //     return 0; // Failure
    // }
    //load the configuration
    EPS::standard_reply reply4 = EPS::load_configuration(wire,i2c_address);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,5);
    print_array(eps_buffer, 5);

    print_standard_reply(reply4);
    if(reply4.error || reply4.rc!=0x93 || reply4.stat!=0x80){
        Console::log("Test C4: FAIL - load configuration command failed\n");
        return 0; // Failure
    }
    //final get
    EPS::config_reply reply5 = EPS::get_config_param(wire,i2c_address,ConfigParameter::TTC_WDG_TIMEOUT);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply5);
    if(reply5.error || reply5.rc!=0x83 || reply5.stat!=0x80){
        Console::log("Test C4: FAIL - final get param command for watchdog failed\n");
        return 0; // Failure
    }
    Console::log("Test C4: PASS - The parameter was successfully read,conf reset,saved,loaded and read\n");
    return 1;
}

// Test 40S: Test SET_CONF_PARAM on a read only parameter command using EPS method
// Returns: 1 on success, 0 on failure
int main40S() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 40S SET_CONF_PARAM on RST_CNTR_PWRON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    ReturnType param_value{};
    param_value.ui16 = 70;
    EPS::config_reply reply = EPS::set_config_param(wire,i2c_address,ConfigParameter::RST_CNTR_PWRON, param_value);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x85 && reply.stat == 0x84) {
        Console::log("Test 40S: PASS - SET_CONF_PARAM on RST_CNTR_PWRON was rejected because we cannot write on read-only data\n");
        //return 1; // Success
    } else {
        Console::log("Test 40S: FAIL - SET_CONF_PARAM on RST_CNTR_PWRON either worked when it should not, or there is another error, %d\n", reply.stat);
        return 0; // Failure
    }
}
struct ConfigEntry {
    const char* name;
    ConfigParameter value;
};

constexpr ConfigEntry configParamTable[] = {
{"BOOT_RESUME_ENA       ", ConfigParameter::BOOT_RESUME_ENA},
{"OBUS_VD1_ALWAYS_ENA   ", ConfigParameter::OBUS_VD1_ALWAYS_ENA},
{"OBUS_VD2_ALWAYS_ENA   ", ConfigParameter::OBUS_VD2_ALWAYS_ENA},
{"OBUS_VD1_ALWAYS_DISA  ", ConfigParameter::OBUS_VD1_ALWAYS_DISA},
{"OBUS_VD2_ALWAYS_DISA  ", ConfigParameter::OBUS_VD2_ALWAYS_DISA},
{"AUTO_HEAT_ENA_BP1     ", ConfigParameter::AUTO_HEAT_ENA_BP1},
{"AUTO_HEAT_ENA_BP2     ", ConfigParameter::AUTO_HEAT_ENA_BP2},
{"AUTO_HEAT_ENA_BP3     ", ConfigParameter::AUTO_HEAT_ENA_BP3},
{"AUTO_BAL_ENA_BP1      ", ConfigParameter::AUTO_BAL_ENA_BP1},
{"AUTO_BAL_ENA_BP2      ", ConfigParameter::AUTO_BAL_ENA_BP2},
{"AUTO_BAL_ENA_BP3      ", ConfigParameter::AUTO_BAL_ENA_BP3},
{"CONF_PARAM_CHANGED    ", ConfigParameter::CONF_PARAM_CHANGED},
{"BOARD_IDENTIFIER      ", ConfigParameter::BOARD_IDENTIFIER},
{"BOARD_IDENTIFIER_KEY  ", ConfigParameter::BOARD_IDENTIFIER_KEY},
{"RAVG_STRENGTH_P2      ", ConfigParameter::RAVG_STRENGTH_P2},
{"STID                  ", ConfigParameter::STID},
{"IVID                  ", ConfigParameter::IVID},
{"BID_USED              ", ConfigParameter::BID_USED},
{"BOOT_RESUME_SHORT     ", ConfigParameter::BOOT_RESUME_SHORT},
{"LOTHR_BAT_HEATER_01   ", ConfigParameter::LOTHR_BAT_HEATER_01},
{"LOTHR_BAT_HEATER_02   ", ConfigParameter::LOTHR_BAT_HEATER_02},
{"LOTHR_BAT_HEATER_03   ", ConfigParameter::LOTHR_BAT_HEATER_03},
{"HITHR_BAT_HEATER_01   ", ConfigParameter::HITHR_BAT_HEATER_01},
{"HITHR_BAT_HEATER_02   ", ConfigParameter::HITHR_BAT_HEATER_02},
{"HITHR_BAT_HEATER_03   ", ConfigParameter::HITHR_BAT_HEATER_03},
{"LOTHR_BMON_UNBAL_01   ", ConfigParameter::LOTHR_BMON_UNBAL_01},
{"LOTHR_BMON_UNBAL_02   ", ConfigParameter::LOTHR_BMON_UNBAL_02},
{"LOTHR_BMON_UNBAL_03   ", ConfigParameter::LOTHR_BMON_UNBAL_03},
{"HITHR_BMON_UNBAL_01   ", ConfigParameter::HITHR_BMON_UNBAL_01},
{"HITHR_BMON_UNBAL_02   ", ConfigParameter::HITHR_BMON_UNBAL_02},
{"HITHR_BMON_UNBAL_03   ", ConfigParameter::HITHR_BMON_UNBAL_03},
{"MCU_TEMP_BIAS         ", ConfigParameter::MCU_TEMP_BIAS},
{"MCU_TEMP_PREMUL       ", ConfigParameter::MCU_TEMP_PREMUL},
{"MCU_TEMP_POSDIV       ", ConfigParameter::MCU_TEMP_POSDIV},
{"BP1_TEMP1_BIAS        ", ConfigParameter::BP1_TEMP1_BIAS},
{"BP1_TEMP2_BIAS        ", ConfigParameter::BP1_TEMP2_BIAS},
{"BP1_TEMP3_BIAS        ", ConfigParameter::BP1_TEMP3_BIAS},
{"BP2_TEMP1_BIAS        ", ConfigParameter::BP2_TEMP1_BIAS},
{"BP2_TEMP2_BIAS        ", ConfigParameter::BP2_TEMP2_BIAS},
{"BP2_TEMP3_BIAS        ", ConfigParameter::BP2_TEMP3_BIAS},
{"BP3_TEMP1_BIAS        ", ConfigParameter::BP3_TEMP1_BIAS},
{"BP3_TEMP2_BIAS        ", ConfigParameter::BP3_TEMP2_BIAS},
{"BP3_TEMP3_BIAS        ", ConfigParameter::BP3_TEMP3_BIAS},
{"BP1_TEMP1_PREMUL      ", ConfigParameter::BP1_TEMP1_PREMUL},
{"BP1_TEMP2_PREMUL      ", ConfigParameter::BP1_TEMP2_PREMUL},
{"BP1_TEMP3_PREMUL      ", ConfigParameter::BP1_TEMP3_PREMUL},
{"BP2_TEMP1_PREMUL      ", ConfigParameter::BP2_TEMP1_PREMUL},
{"BP2_TEMP2_PREMUL      ", ConfigParameter::BP2_TEMP2_PREMUL},
{"BP2_TEMP3_PREMUL      ", ConfigParameter::BP2_TEMP3_PREMUL},
{"BP3_TEMP1_PREMUL      ", ConfigParameter::BP3_TEMP1_PREMUL},
{"BP3_TEMP2_PREMUL      ", ConfigParameter::BP3_TEMP2_PREMUL},
{"BP3_TEMP3_PREMUL      ", ConfigParameter::BP3_TEMP3_PREMUL},
{"BP1_TEMP1_POSDIV      ", ConfigParameter::BP1_TEMP1_POSDIV},
{"BP1_TEMP2_POSDIV      ", ConfigParameter::BP1_TEMP2_POSDIV},
{"BP1_TEMP3_POSDIV      ", ConfigParameter::BP1_TEMP3_POSDIV},
{"BP2_TEMP1_POSDIV      ", ConfigParameter::BP2_TEMP1_POSDIV},
{"BP2_TEMP2_POSDIV      ", ConfigParameter::BP2_TEMP2_POSDIV},
{"BP2_TEMP3_POSDIV      ", ConfigParameter::BP2_TEMP3_POSDIV},
{"BP3_TEMP1_POSDIV      ", ConfigParameter::BP3_TEMP1_POSDIV},
{"BP3_TEMP2_POSDIV      ", ConfigParameter::BP3_TEMP2_POSDIV},
{"BP3_TEMP3_POSDIV      ", ConfigParameter::BP3_TEMP3_POSDIV},
{"ADC_MCU_TEMP_V25T30   ", ConfigParameter::ADC_MCU_TEMP_V25T30},
{"ADC_MCU_TEMP_V25T85   ", ConfigParameter::ADC_MCU_TEMP_V25T85},
{"TTC_WDG_TIMEOUT       ", ConfigParameter::TTC_WDG_TIMEOUT},
{"TTC_WDG_TIMEOUT_KEY   ", ConfigParameter::TTC_WDG_TIMEOUT_KEY},
{"OBUS_STARTUP_ENA_BF   ", ConfigParameter::OBUS_STARTUP_ENA_BF},
{"OBUS_STARTUP_KEY      ", ConfigParameter::OBUS_STARTUP_KEY},
{"OBUS_LATCHOFF_ENA_BF  ", ConfigParameter::OBUS_LATCHOFF_ENA_BF},
{"OBUS_LATCHOFF_KEY     ", ConfigParameter::OBUS_LATCHOFF_KEY},
{"OBUS_STARTUP_DELAY_01 ", ConfigParameter::OBUS_STARTUP_DELAY_01},
{"OBUS_STARTUP_DELAY_02 ", ConfigParameter::OBUS_STARTUP_DELAY_02},
{"OBUS_STARTUP_DELAY_03 ", ConfigParameter::OBUS_STARTUP_DELAY_03},
{"OBUS_STARTUP_DELAY_04 ", ConfigParameter::OBUS_STARTUP_DELAY_04},
{"OBUS_STARTUP_DELAY_05 ", ConfigParameter::OBUS_STARTUP_DELAY_05},
{"OBUS_STARTUP_DELAY_06 ", ConfigParameter::OBUS_STARTUP_DELAY_06},
{"OBUS_STARTUP_DELAY_07 ", ConfigParameter::OBUS_STARTUP_DELAY_07},
{"OBUS_STARTUP_DELAY_08 ", ConfigParameter::OBUS_STARTUP_DELAY_08},
{"OBUS_STARTUP_DELAY_09 ", ConfigParameter::OBUS_STARTUP_DELAY_09},
{"OBUS_STARTUP_DELAY_10 ", ConfigParameter::OBUS_STARTUP_DELAY_10},
{"OBUS_STARTUP_DELAY_11 ", ConfigParameter::OBUS_STARTUP_DELAY_11},
{"OBUS_STARTUP_DELAY_12 ", ConfigParameter::OBUS_STARTUP_DELAY_12},
{"OBUS_STARTUP_DELAY_13 ", ConfigParameter::OBUS_STARTUP_DELAY_13},
{"OBUS_STARTUP_DELAY_14 ", ConfigParameter::OBUS_STARTUP_DELAY_14},
{"OBUS_STARTUP_DELAY_15 ", ConfigParameter::OBUS_STARTUP_DELAY_15},
{"OBUS_STARTUP_DELAY_16 ", ConfigParameter::OBUS_STARTUP_DELAY_16},
{"OBUS_LATCHOFF_DELAY_01", ConfigParameter::OBUS_LATCHOFF_DELAY_01},
{"OBUS_LATCHOFF_DELAY_02", ConfigParameter::OBUS_LATCHOFF_DELAY_02},
{"OBUS_LATCHOFF_DELAY_03", ConfigParameter::OBUS_LATCHOFF_DELAY_03},
{"OBUS_LATCHOFF_DELAY_04", ConfigParameter::OBUS_LATCHOFF_DELAY_04},
{"OBUS_LATCHOFF_DELAY_05", ConfigParameter::OBUS_LATCHOFF_DELAY_05},
{"OBUS_LATCHOFF_DELAY_06", ConfigParameter::OBUS_LATCHOFF_DELAY_06},
{"OBUS_LATCHOFF_DELAY_07", ConfigParameter::OBUS_LATCHOFF_DELAY_07},
{"OBUS_LATCHOFF_DELAY_08", ConfigParameter::OBUS_LATCHOFF_DELAY_08},
{"OBUS_LATCHOFF_DELAY_09", ConfigParameter::OBUS_LATCHOFF_DELAY_09},
{"OBUS_LATCHOFF_DELAY_10", ConfigParameter::OBUS_LATCHOFF_DELAY_10},
{"OBUS_LATCHOFF_DELAY_11", ConfigParameter::OBUS_LATCHOFF_DELAY_11},
{"OBUS_LATCHOFF_DELAY_12", ConfigParameter::OBUS_LATCHOFF_DELAY_12},
{"OBUS_LATCHOFF_DELAY_13", ConfigParameter::OBUS_LATCHOFF_DELAY_13},
{"OBUS_LATCHOFF_DELAY_14", ConfigParameter::OBUS_LATCHOFF_DELAY_14},
{"OBUS_LATCHOFF_DELAY_15", ConfigParameter::OBUS_LATCHOFF_DELAY_15},
{"OBUS_LATCHOFF_DELAY_16", ConfigParameter::OBUS_LATCHOFF_DELAY_16},
{"SAFETY_VOLT_LOTHR     ", ConfigParameter::SAFETY_VOLT_LOTHR},
{"SAFETY_VOLT_HITHR     ", ConfigParameter::SAFETY_VOLT_HITHR},
{"TTC_I2C_SLAVE_ADDR    ", ConfigParameter::TTC_I2C_SLAVE_ADDR},
{"CONF_NVM_SAVE_CNTR    ", ConfigParameter::CONF_NVM_SAVE_CNTR},
{"CONF_NVM_SAVE_CHKS    ", ConfigParameter::CONF_NVM_SAVE_CHKS},
{"RST_CAUSE             ", ConfigParameter::RST_CAUSE},
{"RST_CNTR_PWRON        ", ConfigParameter::RST_CNTR_PWRON},
{"RST_CNTR_WDG          ", ConfigParameter::RST_CNTR_WDG},
{"RST_CNTR_CMD          ", ConfigParameter::RST_CNTR_CMD},
{"RST_CNTR_MCU          ", ConfigParameter::RST_CNTR_MCU},
{"RST_CNTR_EMLOPO       ", ConfigParameter::RST_CNTR_EMLOPO},
{"RST_CODE_MCU_RAW      ", ConfigParameter::RST_CODE_MCU_RAW},
{"EMLOPO_VOLT_LOTHR     ", ConfigParameter::EMLOPO_VOLT_LOTHR},
{"EMLOPO_VOLT_HITHR     ", ConfigParameter::EMLOPO_VOLT_HITHR},
{"EMLOPO_PERIOD         ", ConfigParameter::EMLOPO_PERIOD},
{"SAFETY_VOLT_LOTHR_USED", ConfigParameter::SAFETY_VOLT_LOTHR_USED},
{"SAFETY_VOLT_HITHR_USED", ConfigParameter::SAFETY_VOLT_HITHR_USED},
{"SAFETY_LINGER         ", ConfigParameter::SAFETY_LINGER},
{"TTC_WDG_TIMOUT_USED   ", ConfigParameter::TTC_WDG_TIMOUT_USED},
{"TTC_PREVCMD_ELAPSED   ", ConfigParameter::TTC_PREVCMD_ELAPSED}, //everything after this param is not accessible
{"OB_FORCE_ENA_USE_BF   ", ConfigParameter::OB_FORCE_ENA_USE_BF},
{"OB_STARTUP_ENA_USE_BF ", ConfigParameter::OB_STARTUP_ENA_USE_BF},
{"OB_LATCHOFF_ENA_USE_BF", ConfigParameter::OB_LATCHOFF_ENA_USE_BF},
{"VD0_CH_BF             ", ConfigParameter::VD0_CH_BF},
{"VD1_CH_BF             ", ConfigParameter::VD1_CH_BF},
{"VD2_CH_BF             ", ConfigParameter::VD2_CH_BF},
{"VD3_CH_BF             ", ConfigParameter::VD3_CH_BF},
{"VD4_CH_BF             ", ConfigParameter::VD4_CH_BF},
{"VD5_CH_BF             ", ConfigParameter::VD5_CH_BF},
{"VD6_CH_BF             ", ConfigParameter::VD6_CH_BF}
};
//Test all 131 config params
int mainCP131(){
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test CP131 GET_CONF_PARAM on all parameters starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    delay_ms(50);
    uint8_t eps_buffer[16];

    Console::log("Param -> Value:\n");
    //OB_FORCE_ENA_USE_BF, OB_STARTUP_ENA_USE_BF, OB_LATCHOFF_ENA_USE_BF
    for(uint8_t i = 0; i < 131; i++)
    {
        uint8_t param_l = EPS::get_param_length(EPS::getConfigParameterType(configParamTable[i].value));
        EPS::config_reply reply = EPS::get_config_param(wire,i2c_address,configParamTable[i].value);

        delay_ms(60);
        EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, param_l);
        print_array(eps_buffer, param_l);

        print_config_reply(reply);

        Console::log("%s -> %d", configParamTable[i].name, configParamTable[i].value);
        if (reply.error || reply.rc != 0x83 || reply.stat != 0x80 || reply.par_id!=configParamTable[i].value){
            Console::log("!!!!!!!!!!!!! Test CP131: FAIL - GET param %s, code: %d", configParamTable[i].name, configParamTable[i].value);
            // return 0; // Failure
        }
        delay_ms(15);
    }
    Console::log("\nending\n");
    return 1;
}
//Test all the last 11 config params
int mainCP11(){
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test CP11 GET_CONF_PARAM on all parameters starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    uint8_t eps_buffer[16];
    Console::log("Param -> Value:\n");
    //OB_FORCE_ENA_USE_BF, OB_STARTUP_ENA_USE_BF, OB_LATCHOFF_ENA_USE_BF
    for(uint8_t i = 121; i < 131; i++)
    {
        uint8_t param_l = EPS::get_param_length(EPS::getConfigParameterType(configParamTable[i].value));
        EPS::config_reply reply = EPS::get_config_param(wire,i2c_address,configParamTable[i].value);

        delay_ms(50);
        EPS::get_EPS_buffer(wire, i2c_address, eps_buffer, param_l);
        print_array(eps_buffer, param_l);

        print_config_reply(reply);

        Console::log("%s -> %d", configParamTable[i].name, configParamTable[i].value);
        if (reply.error || reply.rc != 0x83 || reply.stat != 0x80 || reply.par_id!=configParamTable[i].value){
            Console::log("!!!!!!!!!!!!! Test CP11: FAIL - GET param %s, code: %d", configParamTable[i].name, configParamTable[i].value);
            // return 0; // Failure
        }
        delay_ms(10);
    }
    Console::log("\nending\n");
    return 1;
}
// Test 41S
int main() {//main41S
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("\nEPS Test 41 SET_CONF_PARAM on OB_STARTUP_ENA_USE_BF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    ReturnType param_value{};
    param_value.ui16 = 0x0111;
    EPS::config_reply reply = EPS::set_config_param(wire,i2c_address,ConfigParameter::OB_STARTUP_ENA_USE_BF, param_value);

    delay_ms(50);
    uint8_t eps_buffer[10];
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply);

    if (!reply.error && reply.rc == 0x85 && reply.stat == 0x80 && reply.par_id==ConfigParameter::OB_STARTUP_ENA_USE_BF) {
        Console::log("Test 41S: PASS - SET_CONF_PARAM on OB_STARTUP_ENA_USE_BF command works\n");
        //return 1; // Success
    } else {
        Console::log("Test 41S: FAIL - SET_CONF_PARAM on OB_STARTUP_ENA_USE_BF command rejected\n");
        return 0; // Failure
    }

    //try to get it:
    EPS::config_reply reply2 = EPS::get_config_param(wire,i2c_address,ConfigParameter::OB_STARTUP_ENA_USE_BF);

    delay_ms(50);
    EPS::get_EPS_buffer(wire, i2c_address, eps_buffer,10);
    print_array(eps_buffer, 10);

    print_config_reply(reply2);
    if (!reply2.error && reply2.rc == 0x83 && reply2.stat == 0x80 && reply2.par_id==ConfigParameter::OB_STARTUP_ENA_USE_BF && reply2.par_value.ui16==0x0111) {
        Console::log("Test 41S: PASS - GET_CONF_PARAM correctly updated OB_STARTUP_ENA_USE_BF\n");
        return 1; // Success
    } else {
        Console::log("Test 41S: FAIL - GET_CONF_PARAM (OB_STARTUP_ENA_USE_BF) was not updated/read command rejected\n");
        return 0; // Failure
    }
}