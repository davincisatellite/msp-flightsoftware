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

int main(void)
{
    /* UART baud rate. */
    constexpr unsigned int baudrate = 115200;

    DelfiPQcore::initMCU();

    delay_init();

    /* Initialize UART baud rate. */
    Console::init(baudrate);

    delay_ms(1000);

    /* Enabling interrupts for UART. */
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
    MAP_Interrupt_enableMaster();

    MAP_UART_transmitData(EUSCI_A0_BASE, 'loop');

    /* Busy loop. */
    while (1u)
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, 'testt');

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
