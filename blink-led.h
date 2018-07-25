/*
 * blink-led.h
 *
 *  Created on: Jul 17, 2018
 *      Author: bachleu
 */

#ifndef BLINK_LED_H_
#define BLINK_LED_H_

// See page 155 of SPRUH73H (AM335X) October 2011
// Base address of GPIO0
#define AM335X_GPIO0_BASE		    0x44E07000
// Base address of GPIO1
#define AM335X_GPIO1_BASE		    0x4804C000
// Base address of GPIO2
#define AM335X_GPIO2_BASE		    0x481AC000
// Base address of GPIO3
#define AM335X_GPIO3_BASE		    0x481AE000
// GPIO register size
#define AM335X_GPIO_SIZE		    0x1000
// This register is used to enable the pins output capabilities.
#define AM335X_GPIO_OE		    	0x134
//This register is used to register the data that is read from the GPIO pins.
#define AM335X_GPIO_DATAIN			0x138
// This register is used for setting the value of the GPIO output pins
#define AM335X_GPIO_DATAOUT			0x13C
// This register is used to register the data that is read from the GPIO pins.
#define AM335X_GPIO_SETDATAOUT		0x190
// This register is used for setting the value of the GPIO output pins
#define AM335X_GPIO_CLEARDATAOUT	0x194

#define MAX_NO_OF_GPIO_BANKS        (4)
#define GPIO_MODULE0                (0)
#define GPIO_MODULE1                (1)
#define GPIO_MODULE2                (2)
#define GPIO_MODULE3                (3)

#define USR0_LED_D1 (4)
#define USR1_LED_D2 (5)
#define USR2_LED_D3 (7)
#define USR3_LED_D4 (6)


#define USR0_KEY_SW1 (3)
#define USR1_KEY_SW2 (2)
#define USR2_KEY_SW3 (30)
#define USR3_KEY_SW4 (5)

#define OUT          (0)
#define IN           (1)

#ifdef __cplusplus
extern "C" {
#endif

void gpio_set(int gpio_module_no, int pin);
void gpio_reset(int gpio_module_no, int pin);
int gpio_read(int gpio_module_no, int direction, int pin);
void gpio_set_direction(int gpio_module_no, int direction, int pin);
void setup_io();
int set_trigger (void);
void initial_iosetup();
int unmap_io(void);

#ifdef __cplusplus
}
#endif

#endif /* BLINK_LED_H_ */
