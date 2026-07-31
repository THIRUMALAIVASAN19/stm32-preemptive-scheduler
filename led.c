/*
 * led.c
 *
 *  Created on: Jan 31, 2026
 *      Author: Admin
 */


#include <stdint.h>
#include "led.h"




void led_init_all(void){
	uint32_t *pRccahblenr=(uint32_t *)0x40023830;
	uint32_t *pGpiomodelreg=(uint32_t *)0x40020C00;

	*pRccahblenr |=(1 << 3);
	*pGpiomodelreg |=(1<<(2*LED_GREEN));
	*pGpiomodelreg |=(1<<(2*LED_ORANGE));
	*pGpiomodelreg |=(1<<(2*LED_RED));
	*pGpiomodelreg |=(1<<(2*LED_BLUE));
	led_off(LED_GREEN);
	led_off(LED_ORANGE);
	led_off(LED_RED);
	led_off(LED_BLUE);

}

void led_on(uint8_t led_no){
	uint32_t *pGpiodDatereg =(uint32_t *)0x40020C14;
	*pGpiodDatereg |=(1<<led_no);
}

void led_off(uint8_t led_no){

	uint32_t *pGpiodDatereg=(uint32_t *)0x40020C14;
	*pGpiodDatereg &=~(1<<led_no);

}
