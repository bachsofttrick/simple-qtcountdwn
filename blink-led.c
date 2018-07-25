/* by Harsha J K (http://e2e.ti.com/support/arm/sitara_arm/f/791/p/291494/1894870)
 * I have written using the GPIOs memory mapped to userspace.
 * This program enables the user to fiddle with GPIO pins.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "blink-led.h"

uint32_t gpio_base[] = {
	AM335X_GPIO0_BASE,
	AM335X_GPIO1_BASE,
	AM335X_GPIO2_BASE,
	AM335X_GPIO3_BASE,
	0/*NULL*/
};

void *gpio_addr[MAX_NO_OF_GPIO_BANKS] = { NULL };
unsigned int *gpio_oe_addr = NULL;
unsigned int *gpio_dataout_addr = NULL;
unsigned int *gpio_datain_addr = NULL;
unsigned int *gpio_setdataout_addr = NULL;
unsigned int *gpio_cleardataout_addr = NULL;

void *pinmux_addr = NULL;
unsigned int *used_pinmux_addr = NULL;

int  mem_fd;
int gpio_module = -1;


void gpio_set(int gpio_module_no, int pin)
{
	unsigned int reg;
	gpio_dataout_addr = gpio_addr[gpio_module_no] + AM335X_GPIO_DATAOUT;
	reg = *gpio_dataout_addr;
	reg = reg | (1 << pin);
	*gpio_dataout_addr = reg;
    //printf("gpio_set: pin value = %d\treg value = 0x%X\n",pin, reg);
}

void gpio_reset(int gpio_module_no, int pin)
{
	unsigned int reg;
	gpio_dataout_addr = gpio_addr[gpio_module_no] + AM335X_GPIO_DATAOUT;
	reg = *gpio_dataout_addr;
	reg = reg & ~(1 << pin);
	*gpio_dataout_addr = reg;
    //printf("gpio_reset: pin value = %d\treg value = 0x%X\n",pin, reg);
}

int gpio_read(int gpio_module_no, int direction, int pin)
{
	int pin_value = 0;
	unsigned int reg;
	if (direction == 0) {
		gpio_dataout_addr = gpio_addr[gpio_module_no] + AM335X_GPIO_DATAOUT;
		reg = *gpio_dataout_addr;
		if (reg & (1 << pin)) {
			pin_value = 1;
		}
	}
	else if (direction == 1) {
		gpio_datain_addr = gpio_addr[gpio_module_no] + AM335X_GPIO_DATAIN;
		reg = *gpio_datain_addr;
		if (reg & (1 << pin)) {
			pin_value = 1;
		}
	}
	else{
		pin_value = -1;
	}
	if (pin_value == 1)
		printf("gpio_read: pin value = %d\n",pin_value);
	return pin_value;
}

void gpio_set_direction(int gpio_module_no, int direction, int pin)
{
	unsigned int reg;
	gpio_oe_addr = gpio_addr[gpio_module_no] + AM335X_GPIO_OE;
	reg = *gpio_oe_addr;
	printf("gpio_set_direction: reg initial = 0x%X\n",reg);
	if (direction == 0) {
		reg = reg & ~(1 << pin);
		*gpio_oe_addr = reg;
	} else {
		reg = reg | (1 << pin);
		*gpio_oe_addr = reg;
	}
	printf("gpio_set_direction: reg after setting dir = 0x%X\n",reg);
}

/*
 * Set up a memory regions to access GPIO
 */
void setup_io()
{
	int i = 0;
	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
		printf("main: can't open /dev/mem \n");
		exit(-1);
	}
	printf("main: mem_fd = %d\n",mem_fd);
	/* mmap GPIO Bank1 */
	gpio_addr[GPIO_MODULE0]
		= mmap(
				NULL,                   //Any adddress in our space will do
				AM335X_GPIO_SIZE,       //Map length
				PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
				MAP_SHARED,             //Shared with other processes
				mem_fd,                 //File to map
				gpio_base[GPIO_MODULE0] //Offset to GPIO peripheral
		      );
	/* mmap GPIO Bank2*/
	gpio_addr[GPIO_MODULE1]
		= mmap(
				NULL,                   //Any adddress in our space will do
				AM335X_GPIO_SIZE,       //Map length
				PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
				MAP_SHARED,             //Shared with other processes
				mem_fd,                 //File to map
				gpio_base[GPIO_MODULE1] //Offset to GPIO peripheral
		      );
	/* mmap GPIO Bank3*/
	gpio_addr[GPIO_MODULE2]
		= mmap(
				NULL,                   //Any adddress in our space will do
				AM335X_GPIO_SIZE,       //Map length
				PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
				MAP_SHARED,             //Shared with other processes
				mem_fd,                 //File to map
				gpio_base[GPIO_MODULE2] //Offset to GPIO peripheral
		      );
	/* mmap GPIO Bank4*/
	gpio_addr[GPIO_MODULE3]
		= mmap(
				NULL,                   //Any adddress in our space will do
				AM335X_GPIO_SIZE,       //Map length
				PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
				MAP_SHARED,             //Shared with other processes
				mem_fd,                 //File to map
				gpio_base[GPIO_MODULE3] //Offset to GPIO peripheral
		      );				
    //printf("main: gpio_base[%d] = 0x%X\n", gpio_module, gpio_base[gpio_module]);
	close(mem_fd); //No need to keep mem_fd open after mmap
	for ( i = 0; i < MAX_NO_OF_GPIO_BANKS; i++ ) {
		if (gpio_addr[i] == MAP_FAILED) {
			printf("mmap error %d\n", (int)gpio_addr[i]);//errno also set!
			exit(-1);
		}
	}    
} // setup_io

/*
 * Unbind LEDs D3 & D4
 */
int set_trigger (void) {
	FILE *fp = NULL;
	/* create a variable to store what we are sending */
	char set_value[20]; 

	/* Using sysfs we need to write "none" to /sys/class/leds/evmsk:green:heartbeat
	   /trigger */  
	if ((fp = fopen("/sys/class/leds/evmsk:green:heartbeat/trigger",
					"ab")) == NULL) {
		printf("Cannot open trigger file.\n");
		exit(1);
	}
    /* Set pointer to beginning of the file */
	rewind(fp);
	/* Write our value of "heartbeat" to the file */
	strcpy(set_value,"none");
	fwrite(&set_value, sizeof(char), 4, fp);
	fclose(fp);

	/* Using sysfs we need to write "none" to /sys/class/leds/evmsk:green:mmc0
	   /trigger */  
	if ((fp = fopen("/sys/class/leds/evmsk:green:mmc0/trigger",
					"ab")) == NULL) {
		printf("Cannot open trigger file.\n");
		exit(1);
	}
    /* Set pointer to beginning of the file */
	rewind(fp);
	/* Write our value of "heartbeat" to the file */
	strcpy(set_value,"none");
	fwrite(&set_value, sizeof(char), 4, fp);
	fclose(fp);

	return 0;
}

void initial_iosetup ()
{
    /* Unbind LEDs D3 & D4 */
    set_trigger();

    setup_io();

    /* Set LEDs as output */
    gpio_set_direction(GPIO_MODULE1, OUT, USR0_LED_D1);
    gpio_set_direction(GPIO_MODULE1, OUT, USR1_LED_D2);
    gpio_set_direction(GPIO_MODULE1, OUT, USR2_LED_D3);
    gpio_set_direction(GPIO_MODULE1, OUT, USR2_LED_D3);

    printf("main: LEDs set as outputs successfully\n");

    /* Set Keys as inputs */
    gpio_set_direction(GPIO_MODULE2, IN, USR0_KEY_SW1);
    gpio_set_direction(GPIO_MODULE2, IN, USR1_KEY_SW2);
    gpio_set_direction(GPIO_MODULE0, IN, USR2_KEY_SW3);
    gpio_set_direction(GPIO_MODULE2, IN, USR3_KEY_SW4);

    printf("main: Keys set as inputs successfully\n");

    //This part is for reference for action of button and LED.
    /*while (1) {
        if (1 == gpio_read(GPIO_MODULE2, IN, USR0_KEY_SW1) )
            gpio_set(GPIO_MODULE1, USR0_LED_D1);
        else gpio_reset(GPIO_MODULE1, USR0_LED_D1);

        if (1 == gpio_read(GPIO_MODULE2, IN, USR1_KEY_SW2) )
            gpio_set(GPIO_MODULE1, USR1_LED_D2);
        else gpio_reset(GPIO_MODULE1, USR1_LED_D2);

        if (1 == gpio_read(GPIO_MODULE0, IN, USR2_KEY_SW3) )
        {	        gpio_set(GPIO_MODULE1, USR2_LED_D3);printf("Hello Boss!!\n");}
        else gpio_reset(GPIO_MODULE1, USR2_LED_D3);

        if (1 == gpio_read(GPIO_MODULE2, IN, USR3_KEY_SW4) )
            gpio_set(GPIO_MODULE1, USR3_LED_D4);
        else gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    }
    */
}

int unmap_io(void){
    //Turn off LED
    gpio_reset(GPIO_MODULE1, USR0_LED_D1);
    gpio_reset(GPIO_MODULE1, USR1_LED_D2);
    gpio_reset(GPIO_MODULE1, USR2_LED_D3);
    gpio_reset(GPIO_MODULE1, USR3_LED_D4);
    printf("Reset all LED.\n");

    //Restore previous function of heartbeat and mmc0
    FILE *fp = NULL;
    /* create a variable to store what we are sending */
    char set_value[20];

    /* Using sysfs we need to write "heartbeat" to /sys/class/leds/evmsk:green:heartbeat
       /trigger */
    if ((fp = fopen("/sys/class/leds/evmsk:green:heartbeat/trigger",
                    "ab")) == NULL) {
        printf("Cannot open trigger file.\n");
        exit(1);
    }
    /* Set pointer to beginning of the file */
    rewind(fp);
    /* Write our value of "heartbeat" to the file */
    strcpy(set_value,"heartbeat");
    fwrite(&set_value, sizeof(char), 9, fp);
    fclose(fp);

    /* Using sysfs we need to write "mmc0" to /sys/class/leds/evmsk:green:mmc0
       /trigger */
    if ((fp = fopen("/sys/class/leds/evmsk:green:mmc0/trigger",
                    "ab")) == NULL) {
        printf("Cannot open trigger file.\n");
        exit(1);
    }
    /* Set pointer to beginning of the file */
    rewind(fp);
    /* Write our value of "heartbeat" to the file */
    strcpy(set_value,"mmc0");
    fwrite(&set_value, sizeof(char), 4, fp);
    fclose(fp);

    //Unmap memory
    int i = 0;
    for ( i = 0; i < MAX_NO_OF_GPIO_BANKS; i++ ) {
        munmap((void *)gpio_addr[i], AM335X_GPIO_SIZE);
    }

    printf("Unmap completed.\n");
    return 0;
}
