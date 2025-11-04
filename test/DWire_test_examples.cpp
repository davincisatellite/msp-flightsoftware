#include "../src/EPS.h"
#include "../src/DWire.h"
#include "../src/Console.h"
#include "../src/DelfiPQcore.h"


uint8_t try_simple(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.beginTransmission(i2c_address);
    wire.write(2);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.write(3);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}

uint8_t try_without_beginTransmission(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.write(4);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_endTransmission_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.write(5);
    wire.endTransmission(true);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_endTransmission(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.beginTransmission(i2c_address);
    wire.write(6);
    wire.endTransmission(true);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_endTransmission_just_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.write(6);
    wire.endTransmission(true);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}


// trying to force a reset before transmitting
uint8_t try_reset_simple(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.endTransmission(false);
    wire.beginTransmission(i2c_address);
    wire.write(7);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_reset_after_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.endTransmission(false);
    wire.beginTransmission(i2c_address);
    wire.write(8);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_reset_after_begin_with_again_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.endTransmission(false);
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.write(9);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_reset_after_beginTransmission(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.endTransmission(false);
    wire.beginTransmission(i2c_address);
    wire.write(10);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_reset_after_beginTransmission_again(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.endTransmission(false);
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.write(11);
    wire.endTransmission(true);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_reset_after_beginTransmission_again_plus_end(uint8_t i2c_address)
{
    DWire wire = DWire();
    // wire.setFastMode();
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.endTransmission(false);
    wire.begin();
    wire.beginTransmission(i2c_address);
    wire.write(12);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}

/////////////////////////////tests with initializing the DWire
uint8_t try_initializing_DWire(uint8_t i2c_address)
{
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    uint8_t val=13;
    wire.write(val);

    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_initializing_DWire_End_start_again(uint8_t i2c_address)
{
    DWire wire = DWire();
    wire.setFastMode(); //we initialize the DWire
    wire.begin();

    wire.beginTransmission(i2c_address);
    uint8_t val=14;
    wire.write(val);
    wire.endTransmission();

    uint8_t ans=wire.read();
    Console::log("first received: ", ans);
    //try again to see if it is ok to close the transmission
    wire.beginTransmission(i2c_address);
    val=15;
    wire.write(val);
    wire.endTransmission();
    ans=wire.read();
    Console::log("second received: ", ans);
    return ans;
}
uint8_t try_initializing_DWire_without_end(uint8_t i2c_address)
{
    DWire wire = DWire();
    wire.setFastMode();
    wire.begin();

    wire.beginTransmission(i2c_address);
    uint8_t val=16;
    wire.write(val);
    // wire.endTransmission(); //do not close the transmission

    uint8_t ans=wire.read();
    Console::log("first received: ", ans);
    //try again to see if it is ok to close the transmission
    wire.beginTransmission(i2c_address);
    val=17;
    wire.write(val);
    wire.endTransmission();
    ans=wire.read();
    Console::log("second received: ", ans);
    return ans;
}
//experimental
int main_test_DWire() {
    uint8_t i2c_address = 0x20;
    Console::init(9600);

    //try to also run the test without this line
    DelfiPQcore::initMCU();//this is to synchronize the clocks
    //reference: https://github.com/DelfiSpace/EPS_FlightSoftware/blob/8d87a9f21220d9e965f6c331e65ffee714c0338d/main.cpp#L226


    uint8_t sol = 0;
    //add break points at each line that starts with "sol=..."

    //try with initializing first:
    sol = try_initializing_DWire(i2c_address);
    Console::log("sol after try 1: ", sol);
    sol = try_initializing_DWire_End_start_again(i2c_address);
    Console::log("sol after try 2: ", sol);
    sol = try_initializing_DWire_without_end(i2c_address);
    Console::log("sol after try 3: ", sol);

    //if they did not work, try these

    sol = try_simple(i2c_address);
    Console::log("sol after try 4: ", sol);
    sol = try_with_begin(i2c_address);
    Console::log("sol after try 5: ", sol);
    sol = try_without_beginTransmission(i2c_address);
    Console::log("sol after try 6: ", sol);
    sol = try_with_endTransmission_begin(i2c_address);
    Console::log("sol after try 7: ", sol);
    sol = try_with_endTransmission(i2c_address);
    Console::log("sol after try 8: ", sol);
    sol = try_with_endTransmission_just_begin(i2c_address);
    Console::log("sol after try 9: ", sol);
    //with forced reset
    sol = try_reset_simple(i2c_address);
    Console::log("sol after try 10: ", sol);
    sol = try_reset_after_begin(i2c_address);
    Console::log("sol after try 11: ", sol);
    sol = try_reset_after_begin_with_again_begin(i2c_address);
    Console::log("sol after try 12: ", sol);
    sol = try_reset_after_beginTransmission(i2c_address);
    Console::log("sol after try 13: ", sol);
    sol = try_reset_after_beginTransmission_again(i2c_address);
    Console::log("sol after try 14: ", sol);
    sol = try_reset_after_beginTransmission_again_plus_end(i2c_address);
    Console::log("sol after try 15: ", sol);

    //if sol is !=0 then a method succeeded!!! It is possible that we enter an infinite loop at some point.
    //So, a good suggestion is to use the debugger.
    return sol;
}
