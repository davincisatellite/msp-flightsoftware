#ifndef BITFLIP_H_
#define BITFLIP_H_

#include "DWire.h"

class Bitflip {
    public:
    enum Command {
        COMMAND_STATUS = 0x00,
        COMMAND_WRITE = 0x01, // PLACEHOLDER, TBD in ICD by payload team
        COMMAND_READ = 0x02, // PLACEHOLDER, TBD in ICD by payload team
        COMMAND_GET_TEMP = 0x03 // PLACEHOLDER, TBD in ICD by payload team
    };

    enum Status {
        STATUS_UNDEFINED = 0xFF,
        STATUS_INIT = 0x00, 
        STATUS_OK = 0x01,
        STATUS_FAIL = 0xF1
    };

    enum Error {
        ERROR_NO_ERROR = 0,
        ERROR_I2C_READ_ERROR = 1
    };

    private:
    DWire &wire;
    unsigned char i2c_address;

    public:
    Bitflip(DWire&, int);

    Bitflip::Error getStatus(Bitflip::Status&);
    int read(int, int, int, int);
    int write(int, int);
};

#endif // BITFLIP_H_