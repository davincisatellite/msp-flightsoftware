/*
 * delay.h
 *
 *  Created on: Mar 24, 2023
 *      Author: marco
 */

#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

void delay_init();

void delay_ms(uint32_t ms);

#endif /* DELAY_H_ */
