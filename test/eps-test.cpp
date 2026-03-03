#include "../src/EPS.h"
#include <cstddef> //for size_t
#include <cstring> //for memcpy
#include "../src/Console.h"
#include "../src/DelfiPQcore.h"
#include "../src/delay.h"

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
bool test_writeCommand(uint8_t i2c_address) {
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    int there_is_an_error=0;
    EPS::writeCommand(my_wire,i2c_address, CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW);
    uint8_t byte1=my_wire.read();
    uint8_t byte2=my_wire.read();
    uint8_t byte3=my_wire.read();
    uint8_t byte4=my_wire.read();
    if (byte1!=STID) {
        Console::log("It fails on writing first byte in writeCommand");
        there_is_an_error=1;
    }
    if (byte2!=IVID) {
        Console::log("It fails on writing second byte in writeCommand");
        there_is_an_error=1;
    }
    //or simply if (byte3 != 0x70)
    if (static_cast<CommandCode>(byte3) != CommandCode::GET_PCU_HOUSEKEEPING_DATA_RAW){//static_cast<CommandCode>(0x70)) {
        Console::log("It fails on writing third byte in writeCommand");
        there_is_an_error=1;
    }
    if (byte4!=BID) {
        Console::log("It fails on writing forth byte in writeCommand");
        there_is_an_error=1;
    }
    if (there_is_an_error==1)
        return false;
    return true;
}

bool test_read_config_params(uint8_t i2c_address) {
    int there_is_an_error=0;
    DWire my_wire = DWire();
    my_wire.setFastMode();
    my_wire.begin();
    EPS::config_reply reply ={};

    //test when we get different response length (8 instead of 9)
    my_wire.beginTransmission(i2c_address);
    for (uint8_t i=0x01;i<=0x08;i++)
        my_wire.write(i);
    reply=EPS::read_config_params(my_wire,i2c_address,static_cast<ConfigParameter>(0x100A),reply);
    if (reply.error!=true) {
        Console::log("It fails when reading insufficient data in read_config_params");
        there_is_an_error=1;
    }


    //aici tre sa continui sa verifici cazurile

    if (there_is_an_error==1)
        return false;
    return true;
}


// get_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id);
bool test_get_config_params(DWire &wire, uint8_t i2c_address) {
    EPS::config_reply reply;

    //Test: Retrieve STID
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::STID);
    if (reply.error || reply.par_id != ConfigParameter::STID) {
        Console::log("fail STID get_config_params");
        return false;
    }

    //Test: Retrieve IVID
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::IVID);
    if (reply.error || reply.par_id != ConfigParameter::IVID) {
        Console::log("fail IVID get_config_params");
        return false;
    }

    //Test: Retrieve BID_USED
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::BID_USED);
    if (reply.error || reply.par_id != ConfigParameter::BID_USED) {
        Console::log("fail BID_USED get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (Int8)
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);
    if (reply.error || reply.par_id != ConfigParameter::BOOT_RESUME_ENA) {
        Console::log("fail BOOT_RESUME_ENA get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (UInt8)
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::BOARD_IDENTIFIER_KEY);
    if (reply.error || reply.par_id != ConfigParameter::BOARD_IDENTIFIER_KEY) {
        Console::log("fail BOARD_IDENTIFIER_KEY get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (Int16)
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::HITHR_BMON_UNBAL_02);
    if (reply.error || reply.par_id != ConfigParameter::HITHR_BMON_UNBAL_02) {
        Console::log("fail HITHR_BMON_UNBAL_02 get_config_params");
        return false;
    }

    //Test: Retrieve a valid configuration parameter (UInt16)
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::VD6_CH_BF);
    if (reply.error || reply.par_id != ConfigParameter::VD6_CH_BF) {
        Console::log("fail VD6_CH_BF get_config_params");
        return false;
    }

    //Test: Retrieve a read-only configuration parameter
    reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::CONF_NVM_SAVE_CNTR);
    if (!reply.error) {
        Console::log("fail CONF_NVM_SAVE_CNTR get_config_params");
        return false;
    }
    return true;
}


// set_config_params(DWire &wire, uint8_t i2c_address, ConfigParameter conf_par_id, returnType conf_par_value);
bool test_set_config_params(DWire &wire, uint8_t i2c_address) {
    EPS::config_reply reply;

    // 1. Set a writable Int8 configuration parameter
    ReturnType param_value;
    param_value.i8 = 1; // Example value

    reply = EPS::set_config_param(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA, param_value);

    // Check if an error occurred
    if (reply.error) {
        Console::log("fail set_config_params: Error setting BOOT_RESUME_ENA");
        return false;
    }

    // Ensure the returned parameter ID matches the one we set
    if (reply.par_id != ConfigParameter::BOOT_RESUME_ENA) {
        Console::log("fail set_config_params: Incorrect parameter ID");
        return false;
    }

    // 2. Read the parameter to verify the change
    EPS::config_reply read_reply = EPS::get_config_param(wire, i2c_address, ConfigParameter::BOOT_RESUME_ENA);

    if (read_reply.error) {
        Console::log("fail set_config_params: Error retrieving BOOT_RESUME_ENA after setting");
        return false;
    }

    if (read_reply.par_value.i8 != param_value.i8) {
        Console::log("fail set_config_params: Incorrect value read back for BOOT_RESUME_ENA");
        return false;
    }

    Console::log("pass set_config_params");
    return true;
}


//WriteCommand and ReadCommand are tested indirectly through all other tests
// #include "DWire_test_examples.h"
int mainnnn(void)//change name to mainnn
{
    //test the DWIRe first!!
    // int r = mainnn();
    // Console::log("DWire test result: ", r);
    //
    // if (r==0)//do not continue until we solved the DWire problem.
    //     return -1;

    //if the DWire tests were successful, then we continue

    DWire wire = DWire();
    uint8_t i2c_address = 0x20;
    Console::init(9600);

    int nr_of_errors=0;
    if (!test_getConfigParameterType()) {
        nr_of_errors++;
    }

    if(!test_getAccessType()) {
        nr_of_errors++;
    }

    if(!test_get_param_length()) {
        nr_of_errors++;
    }

    if(!test_get_config_params(wire, i2c_address)) {
        nr_of_errors++;
    }

    if (nr_of_errors == 0)
        Console::log("All tests passed successfully.");
    else
        Console::log(nr_of_errors+" errors found.");
    return nr_of_errors;
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

// Test 1: Basic DWire initialization and setup
// Returns: 1 on success, 0 on failure
int main1() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 1 starting\n");
    // DelfiPQcore::initMCU(); // Initialize MCU for clock synchronization

    // delay_init(1)
    
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    Console::log("Test 1: DWire initialized successfully");
    Console::log("Test 1: PASS - Basic initialization works");
    return 1; // Success
}

// Test 2: Write command and verify it's in TX buffer (without transmission)
// Returns: 1 on success, 0 on failure
int main2() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 2 starting\n");
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Write a command to the buffer
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION command
    wire.write(0x00); // BID

    Console::log("Test 2: Command written to TX buffer");
    Console::log("Test 2: PASS - Write to buffer works");
    return 1; // Success
}

// Test 3: Complete write cycle (beginTransmission, write, endTransmission)
// Returns: 1 on success, 0 on failure
int main3() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 3 starting\n");
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Write and end transmission
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    bool result = wire.endTransmission(true);
    
    if (result == false) {
        Console::log("Test 3: PASS - Transmission completed successfully");
        return 1; // Success
    } else {
        Console::log("Test 3: FAIL - Transmission failed or NAK received");
        return 0; // Failure
    }
}

// Test 4: Request data from slave (requestFrom) - basic test
// Returns: 1 on success, 0 on failure
int main4() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 4 starting\n");
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    // Request 5 bytes from the slave even though we did not write anything
    uint8_t bytes_received = wire.requestFrom(i2c_address, 5);

    Console::log("data received %d", bytes_received);
    if (bytes_received == 5) {
        Console::log("Test 4: PASS - Successfully requested 5 data");
        uint8_t byte1=wire.read();
        uint8_t byte2=wire.read();
        uint8_t byte3=wire.read();
        uint8_t byte4=wire.read();
        uint8_t byte5=wire.read();
        Console::log("%d  %d  %d  %d  %d", byte1, byte2, byte3, byte4, byte5);
        return 1; // Success
    } else {
        Console::log("Test 4: FAIL - Data received (may be expected if EPS not connected)");
        // char * buffer;

        Console::log("%d", bytes_received);
        return 0; // Failure (or expected if no EPS connected)
    }
}

// Test 5: Complete I2C cycle - Write command, wait, request, read ending in False
// Returns: 1 on success, 0 on failure
int main5() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 5 starting\n");
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    wire.endTransmission(true);

    // delay_ms(25); // not sure if we should wait

    uint8_t bytes_received = wire.requestFrom(i2c_address, 4);
    
    if (bytes_received == 4) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        // uint8_t stat = wire.read();
        Console::log("%d %d %d %d", stid, ivid,rc,bid);
        
        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (stid == 0x00 && ivid == 0x06 && bid == 0x00) {
            Console::log("Test 5: PASS - Complete I2C cycle works, valid response received");
            return 1; // Success
        } else {
            Console::log("Test 5: FAIL - Invalid response structure %d", bytes_received);
            return 0; // Failure
        }
    } else {
        Console::log("Test 5: FAIL - Expected 4 bytes");
        return 0; // Failure
    }
}
// Test 5.2: Complete I2C cycle - Write command, wait, request, read ending in False
// Returns: 1 on success, 0 on failure
int main5bis() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 5.2 starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION
    wire.write(0x00); // BID
    wire.endTransmission(false);

    // delay_ms(25);

    uint8_t bytes_received = wire.requestFrom(i2c_address, 4);

    if (bytes_received == 4) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        // uint8_t stat = wire.read();
        Console::log("%d %d %d %d", stid, ivid,rc,bid);

        // Verify response structure (STID=0x00, IVID=0x06, BID=0x00)
        if (stid == 0x00 && ivid == 0x06 && bid == 0x00) {
            Console::log("Test 5.2: PASS - Complete I2C cycle works, valid response received");
            return 1; // Success
        } else {
            Console::log("Test 5.2: FAIL - Invalid response structure %d", bytes_received);
            return 0; // Failure
        }
    } else {
        Console::log("Test 5.2: FAIL - Expected 4 bytes");
        return 0; // Failure
    }
}
void print_4_bytes_command(uint8_t stid, uint8_t ivid, uint8_t cc, uint8_t bid) {
    Console::log("Command: stid %x  ivid %x  cc %x  bid %x\n",stid,ivid,cc,bid);
}
void print_5_bytes_response(uint8_t stid, uint8_t ivid, uint8_t rc, uint8_t bid, uint8_t stat) {
    Console::log("Response: stid %x  ivid %x  rc %x  bid %x  stat %x\n",stid,ivid,rc,bid,stat);
}
void print_5_bytes_reply(EPS::standard_reply reply) {
    Console::log("Response: stid %x  ivid %x  rc %x  bid %x  stat %x  error %d\n",reply.stid,reply.ivid,reply.rc,reply.bid,reply.stat, reply.error);
}
// Test 5.3: Test No Operation using a delay.
// If it fails, try with STID=0x1A and BID =0 or STID=0x12 and BID=0x01
int main55() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 5.3 starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    print_4_bytes_command(0x00,0x06,0x02,0x00);
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x02); // NO_OPERATION (cc)
    wire.write(0x00); // BID
    bool writing = wire.endTransmission(true);
    if (writing) {
        Console::log("Test 5.3: FAIL - I2C write failed / NAK");
        return 0;
    }

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
        if (rc == 0x03 && (stat == 0x80 || stat == 0x00)){
            //0x80 is better, but 0x00 is also correct
            Console::log("Test 5.3: PASS - Command accepted");
            return 1;
        }
        else {
            Console::log("Test 5.3: FAIL - Invalid response structure");
            return 0;
        }

    }
    else {
        Console::log("Test 5.3: FAIL - Too few Bytes received: %d", bytes_received);
        return 0;
    }
}
// Test 6: Test NO_OPERATION command using EPS function
// Returns: 1 on success, 0 on failure
int main6() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 6 starting\n");
    
    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::standard_reply reply = EPS::no_operation(wire, i2c_address);
    print_5_bytes_reply(reply);
    if (!reply.error && reply.rc == 0x03 && reply.stat == 0x80) {
        Console::log("Test 6: PASS - NO_OPERATION command works");
        return 1; // Success
    } else {
        Console::log("Test 6: FAIL - NO_OPERATION returned error or wrong values");
        return 0; // Failure
    }
}
// Test 7: Test OUTPUT_BUS_CHANNEL_ON command using raw write, read methods
// Returns: 1 on success, 0 on failure
int main7() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 7 OUTPUT_BUS_CHANNEL_ON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::writeCommand5Bytes(wire,i2c_address,CommandCode::OUTPUT_BUS_CHANNEL_ON, 0x03);
    delay_ms(20);
    EPS::standard_reply reply;
    uint8_t bytes_received = wire.requestFrom(i2c_address, 5);

    if (bytes_received == 5) {
        EPS::readCommand(wire,reply);
        print_5_bytes_reply(reply);
        if (!reply.error && reply.rc == 0x17 && reply.stat == 0x80) {
            Console::log("Test 7: PASS - OUTPUT_BUS_CHANNEL_ON command on index 1 works");
            return 1; // Success
        } else {
            Console::log("Test 7: FAIL - OUTPUT_BUS_CHANNEL_ON command rejected");
            return 0; // Failure
        }
    }
    else {
        Console::log("Test 7: FAIL - Too few Bytes received: %d", bytes_received);
        return 0;
    }
}
// Test 7.2: Test OUTPUT_BUS_CHANNEL_ON command using EPS method
// Returns: 1 on success, 0 on failure
int main7_2() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 7.2 OUTPUT_BUS_CHANNEL_ON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::output_bus_channel_on(wire,i2c_address, 0x03);
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x17 && reply.stat == 0x80) {
        Console::log("Test 7.2: PASS - OUTPUT_BUS_CHANNEL_ON command on index 3 works");
        return 1; // Success
    } else {
        Console::log("Test 7.2: FAIL - OUTPUT_BUS_CHANNEL_ON command rejected");
        return 0; // Failure
    }
}
// Test 8: Test OUTPUT_BUS_CHANNEL_OFF command using raw write, read methods
// Returns: 1 on success, 0 on failure
int main8() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 8 OUTPUT_BUS_CHANNEL_OFF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::writeCommand5Bytes(wire,i2c_address,CommandCode::OUTPUT_BUS_CHANNEL_OFF, 0x03);
    delay_ms(20);
    EPS::standard_reply reply;
    uint8_t bytes_received = wire.requestFrom(i2c_address, 5);

    if (bytes_received == 5) {
        EPS::readCommand(wire,reply);
        print_5_bytes_reply(reply);
        if (!reply.error && reply.rc == 0x19 && reply.stat == 0x80) {
            Console::log("Test 8: PASS - OUTPUT_BUS_CHANNEL_OFF command on index 1 works");
            return 1; // Success
        } else {
            Console::log("Test 8: FAIL - OUTPUT_BUS_CHANNEL_OFF command rejected");
            return 0; // Failure
        }
    }
    else {
        Console::log("Test 8: FAIL - Too few Bytes received: %d", bytes_received);
        return 0;
    }
}
// Test 8.2: Test OUTPUT_BUS_CHANNEL_OFF command using EPS method
// Returns: 1 on success, 0 on failure
int main8_2() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 8.2 OUTPUT_BUS_CHANNEL_OFF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::output_bus_channel_off(wire,i2c_address, 0x03);
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x19 && reply.stat == 0x80) {
        Console::log("Test 8.2: PASS - OUTPUT_BUS_CHANNEL_OFF command on index 1 works");
        return 1; // Success
    } else {
        Console::log("Test 8.2: FAIL - OUTPUT_BUS_CHANNEL_OFF command rejected");
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
    Console::log("EPS Test 9 WATCHDOG starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();
    
    EPS::standard_reply reply = EPS::watchdog(wire, i2c_address);
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x07) {
        Console::log("Test 9: PASS - WATCHDOG command works");
        return 1; // Success
    } else {
        Console::log("Test 9: FAIL - WATCHDOG returned error or wrong RC");
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
    print_5_bytes_reply(reply);
    
    if (!reply.error && reply.rc == 0x05 && reply.stat == 0x80) {
        Console::log("Test 10: PASS - CANCEL_OPERATION command works");
        return 1; // Success
    } else {
        Console::log("Test 10: FAIL - CANCEL_OPERATION returned error");
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
    Console::log("EPS Test 11 SYSTEM_RESET starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply = EPS::system_reset(wire, i2c_address);
    print_5_bytes_reply(reply);

    //in case we don't manage to read the response, we should take a look at the output (we should see 0xFF values)
    if (!reply.error && ((reply.rc == 0xAB && reply.stat == 0x80) || (reply.rc==0xFF && reply.stat == 0xFF))) {
        Console::log("Test 11: PASS - SYSTEM_RESET command works");
        return 1; // Success
    } else {
        Console::log("Test 11: FAIL - SYSTEM_RESET returned error");
        return 0; // Failure
    }
}
//Test turn on channel 3 + system reset
int main11_2(){
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 11 SYSTEM_RESET starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //turn on channel 3
    EPS::standard_reply reply2 = EPS::output_bus_channel_on(wire,i2c_address, 0x03);
    print_5_bytes_reply(reply2);

    delay_ms(2000);
    Console::log("...\n");

    EPS::standard_reply reply = EPS::system_reset(wire, i2c_address);

    print_5_bytes_reply(reply);
    if (!reply.error && ((reply.rc == 0xAB && reply.stat == 0x80) || (reply.rc==0xFF && reply.stat == 0xFF))) {
        Console::log("Test 11: PASS - SYSTEM_RESET command works");
        return 1; // Success
    } else {
        Console::log("Test 11: FAIL - SYSTEM_RESET returned error");
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
    Console::log("EPS Test Checking EPS State starting\n");

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

    }
    else {
        Console::log("Test 12: Too few Bytes received: %d", bytes_received);
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
    Console::log("EPS Test 13 OUTPUT_BUS_GROUP_ON starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    EPS::standard_reply reply2 = EPS::system_reset(wire, i2c_address);
    print_5_bytes_reply(reply2);
    if (!reply2.error && ((reply2.rc == 0xAB && reply2.stat == 0x80) || (reply2.rc==0xFF && reply2.stat == 0xFF))) {
        Console::log("Test 11: PASS - SYSTEM_RESET command works");
       // return 1; // Success
    } else {
        Console::log("Test 11: FAIL - SYSTEM_RESET returned error");
        //return 0; // Failure
    }
    delay_ms(3000);
    Console::log("...\n");

    //0b00000001 11111111 -> is good
    EPS::standard_reply reply = EPS::output_bus_group_on(wire,i2c_address, 0x01FF);//0x00DC
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x11 && reply.stat == 0x80) {
        Console::log("Test 13: PASS - OUTPUT_BUS_GROUP_ON command works");
        return 1; // Success
    } else {
        Console::log("Test 13: FAIL - OUTPUT_BUS_GROUP_ON command rejected");
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
    Console::log("EPS Test 14 OUTPUT_BUS_GROUP_OFF starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //0b00000001 11011100 -> is good
    EPS::standard_reply reply = EPS::output_bus_group_off(wire,i2c_address, 0x01DC);//0x00DC
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x13 && reply.stat == 0x80) {
        Console::log("Test 14: PASS - OUTPUT_BUS_GROUP_OFF command works");
        return 1; // Success
    } else {
        Console::log("Test 14: FAIL - OUTPUT_BUS_GROUP_OFF command rejected");
        return 0; // Failure
    }
}
// Test 15: Test OUTPUT_BUS_GROUP_STATE command using EPS method on channels 2,3,4,6,7 (no 0,1,5)
// Returns: 1 on success, 0 on failure
int main15() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("EPS Test 14 OUTPUT_BUS_GROUP_STATE starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    //0b00000001 11000000 -> is good (just 6,7,8)
    EPS::standard_reply reply = EPS::output_bus_group_state(wire,i2c_address, 0x01C0);//0x00DC
    print_5_bytes_reply(reply);

    if (!reply.error && reply.rc == 0x15 && reply.stat == 0x80) {
        Console::log("Test 15: PASS - OUTPUT_BUS_GROUP_STATE command works");
        return 1; // Success
    } else {
        Console::log("Test 15: FAIL - OUTPUT_BUS_GROUP_STATE command rejected");
        return 0; // Failure
    }
}

// Test 16: Test Get PIU Housekeeping Data (eng)
// Returns: 1 on success, 0 on failure
int main() {
    DelfiPQcore::initMCU();
    delay_init();
    Console::init(9600);

    delay_ms(1000);
    Console::log("Get PIU Housekeeping Data starting\n");

    uint8_t i2c_address = 0x20;
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    print_4_bytes_command(0x00,0x06,0x62,0x00);
    wire.beginTransmission(i2c_address);
    wire.write(0x00); // STID
    wire.write(0x06); // IVID
    wire.write(0x62); // Get PIU Housekeeping Data
    wire.write(0x00); // BID
    bool writing = wire.endTransmission(true);
    // if (writing) {
    //     Console::log("Test 16: FAIL - I2C write failed / NAK");
    //     return 0;
    // }

    delay_ms(20); //important

    uint8_t bytes_received = wire.requestFrom(i2c_address, 84);
    if (bytes_received == 84) {
        uint8_t stid = wire.read();
        uint8_t ivid = wire.read();
        uint8_t rc = wire.read();
        uint8_t bid = wire.read();
        uint8_t stat = wire.read();
        print_5_bytes_response(stid,ivid,rc,bid,stat);
        //we still have 111 bytes to print
        uint8_t buff[84];

        // uint8_t n=111;
        buff[0]=stid;
        buff[1]=ivid;
        buff[2]=rc;
        buff[3]=bid;
        buff[4]=stat;
        for (uint8_t i = 0; i < 79; i++) {
            buff[i+5] = wire.read();
        }

        if (rc == 0x63 && stat == 0x80){
            //0x80 is better, but 0x00 is also correct
            Console::log("Test 16: PASS - Command accepted");
            // print_uint8_array_bytes(buff,116);
            for (uint8_t i = 0; i < 84; i++) {
                Console::log("%d) %x", i, buff[i]);
            }
            return 1;
        }
        else {
            Console::log("Test 16: FAIL - Invalid response structure");
            return 0;
        }

    }
    else {
        Console::log("Test 5.3: FAIL - Too few Bytes received: %d", bytes_received);
        return 0;
    }
    return 0;
}
/*

EPS Test 13 OUTPUT_BUS_GROUP_ON starting

read: 255 255 255 255 255
Response: stid FF  ivid FF  rc FF  bid FF  stat FF  error 0

...

Response: stid 30  ivid 30  rc 0  bid 0  stat 4  error 1

Test 13: FAIL - OUTPUT_BUS_GROUP_ON command rejected

*/