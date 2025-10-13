#include "../src/EPS.h"
#include "../src/DWire.h"
#include "../src/Console.h"
#include "../src/Console.h"


uint8_t try_simple(uint8_t i2c_address)
{
    DWire wire = DWire();
    wire.beginTransmission(i2c_address);
    wire.write(2);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
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
    wire.begin();
    wire.write(4);
    uint8_t ans=wire.read();
    Console::log("received: ", ans);
    return ans;
}
uint8_t try_with_endTransmission_begin(uint8_t i2c_address)
{
    DWire wire = DWire();
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
int main_test_DWire() {
    uint8_t i2c_address = 0x20;
    Console::init(9600);
    uint8_t sol = 0;
    sol = try_simple(i2c_address);
    sol = try_with_begin(i2c_address);
    sol = try_without_beginTransmission(i2c_address);
    sol = try_with_endTransmission_begin(i2c_address);
    sol = try_with_endTransmission(i2c_address);
    sol = try_with_endTransmission_just_begin(i2c_address);
    //with forced reset
    sol = try_reset_simple(i2c_address);
    sol = try_reset_after_begin(i2c_address);
    sol = try_reset_after_begin_with_again_begin(i2c_address);
    sol = try_reset_after_beginTransmission(i2c_address);
    sol = try_reset_after_beginTransmission_again(i2c_address);
    sol = try_reset_after_beginTransmission_again_plus_end(i2c_address);

    //if sol is !=0 then a method succeeded!!! It is possible that we enter an infinite loop at some point.
    //So, a good suggestion is to use the debugger.
    return sol;
}
