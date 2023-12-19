/**
 * IMAGE TRANSMISSION TEST
 *
 * Author: Marco van Eerden
 * Date: June 2023
 *
 * HOW IT WORKS
 *   This program tests a transmission of an image. The image is transmitted in chunks, each
 *   chunk starts with a two-byte length indicator.
 *
 *   A PC send the image bytes to the MSP (OBC), which transmits it to itself (Transceiver) over I2C.
 *   The MSP (Antenna) then sends the image bytes back to the PC (Ground station).
 *
 *   To communicate with itself over I2C, the MSP needs to act as both a master and a slave.
 *   To this end, one of the I2C buses is used as a master (B0) and the other as a slave (B1).
 *
 * REMARKS
 *   1. We do not use DWire here: their slave mode does not work.
 *   2. The same UART bus is used for sending image data and receiving it: therefore, we cannot
 *      transmit while receiving.
 *   3. The maximum baud rate for the MSP is 1152000. Any higher and it will stop working.
 *
 * HOW CAN WE USE THIS FOR THE FUTURE?
 *   This is a crude proof-of-concept and is not intended to be used in the final satellite.
 *   For starters, the antenna only transmits at 9600bps, which bottlenecks the overall transmission speed
 *   of an image. So high speeds are not necessary.
 *   Secondly, we probably want some sort of error detection, such as a CRC, for each image chunk.
 *   Lastly, this is a very empty project. We are not sure how much memory will be free in the end for buffering
 *   the image, or how much processing power we can reserve for this.
 */
#include "msp.h"
#include "Console.h"
#include "DelfiPQcore.h"
#include "delay.h"

/* Slave Address for I2C Slave. */
#define SLAVE_ADDRESS 0x48

/* Length of the buffer. Image chunk size must be smaller than this! */
#define BUFFER_LENGTH 1024

/* Buffer to store data in. */
static uint8_t TXData[BUFFER_LENGTH] = { 0 };

/* Length of the received data. */
static uint16_t TXLength = 0;

/* Number of image bytes received. */
static uint16_t TXByteCtr = 0;

/* Index for transmitting bytes. */
static uint16_t TXByteIndex = 0;

/* Number of iterations in the receiving interrupt. */
static uint16_t TXIter = 0;

unsigned char errorMsg2[] = "Serial port not connected\r\n";

bool isEnabled()
{
    uint8_t status = MAP_GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN2);
    if(status == GPIO_INPUT_PIN_HIGH){
        return true;
    }else{
        return true;
    }
}

int main(void)
{
    /* UART baud rate. */
    constexpr unsigned int baudrate = 115200;


    DelfiPQcore::initMCU();

    delay_init();

    /* Copy from Console::init*/
    MAP_UART_disableModule( EUSCI_A0_BASE );   //disable UART operation for configuration settings
    MAP_UART_disableModule( EUSCI_A2_BASE );   //disable UART 2 operation for configuration settings

    // Selecting P1.2 and P1.3 in UART mode
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                                   GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // Selecting P3.2 and P3.3 in UART mode
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                                                   GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    eUSCI_UART_ConfigV1 Config;

    //Default Configuration, macro found in uart.h
    Config.selectClockSource    = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    Config.parity               = EUSCI_A_UART_NO_PARITY;
    Config.msborLsbFirst        = EUSCI_A_UART_LSB_FIRST;
    Config.numberofStopBits     = EUSCI_A_UART_ONE_STOP_BIT;
    Config.uartMode             = EUSCI_A_UART_MODE;

    unsigned int n = MAP_CS_getSMCLK() / baudrate;

    if (n > 16)
    {
        Config.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION; // Over-sampling
        Config.clockPrescalar = n >> 4;                                      // BRDIV = n / 16
        Config.firstModReg = n - (Config.clockPrescalar << 4);               // UCxBRF = int((n / 16) - int(n / 16)) * 16
    }
    else
    {
        Config.overSampling = EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION; // Low-frequency mode
        Config.clockPrescalar = n;                                            // BRDIV = n
        Config.firstModReg = 0;                                               // UCxBRF not used
    }

    Config.secondModReg = 0;    // UCxBRS = 0

    MAP_UART_initModule( EUSCI_A0_BASE, &Config );
    MAP_UART_initModule( EUSCI_A2_BASE, &Config );


    /* Enable UART module */
    MAP_UART_enableModule( EUSCI_A0_BASE );
    MAP_UART_enableModule( EUSCI_A2_BASE );

    // in case the serial port is not detected, print an error message:
    // if there was a connection problem, the message will help debugging,
    // if there is no serial port attached, the message will not be seen
    if (!isEnabled())
    {
        for(int k = 0; errorMsg2[k] != 0; k++)
        {
            MAP_UART_transmitData( EUSCI_A0_BASE, errorMsg2[k] );
        }
    }

    delay_ms(1000);

//    /* Enabling interrupts for UART. */
//    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
//    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
//    MAP_Interrupt_enableMaster();


    /* Busy loop. */
    while (1u)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, 'testt');
        MAP_UART_transmitData(EUSCI_A2_BASE, 'testk');


        delay_ms(1000);
    }
}

/**
 * UART receive interrupt.
 *
 * Each message received starts with a two-byte length followed by that many
 * image bytes.
 */
extern "C" void EUSCIA0_IRQHandler(void)
{
    uint_fast8_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    /* Clear interrupt flags so it can be triggered again. */
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        /* Get first length byte. */
        if (TXIter == 0)
        {
            TXLength = MAP_UART_receiveData(EUSCI_A0_BASE);
        }
            /* Get second length byte. */
        else if (TXIter == 1)
        {
            TXLength |= MAP_UART_receiveData(EUSCI_A0_BASE) << 8;
        }
            /* Get all image bytes. */
        else
        {
            TXData[TXByteCtr++] = MAP_UART_receiveData(EUSCI_A0_BASE);
        }

        /* Initiate I2C transfer if all bytes have been received. */
        if (TXIter > 1 && TXByteCtr == TXLength)
        {
            TXByteIndex = 1;
            MAP_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, TXData[0]);
        }

        TXIter++;
    }
}
