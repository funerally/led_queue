#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"




// Configure Queue

#define QUEUE_TYPE	int
#define QUEUE_SIZE 	5
#define ARRAY_SIZE	(QUEUE_SIZE + 1)

static QUEUE_TYPE	queue[ARRAY_SIZE];
static size_t	front = 1;
static size_t	rear = 0;


void insert(QUEUE_TYPE value) {
	
	rear = (rear + 1) % ARRAY_SIZE;
	queue[rear] = value;

}	

void delete(void){

	front = (front + 1) % ARRAY_SIZE;

}

int is_empty(void){

	return (rear + 1) % ARRAY_SIZE == front;
}

int is_full(void){

	return (rear + 2) % ARRAY_SIZE == front;

}

void init_GPIO() {
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStruct;
		NVIC_InitTypeDef NVIC_InitStruct;
		
		// Clock for LEDs
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		// Clock for SYSCFG
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		// Clock for User Button
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		// Configure LEDS
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		// Configure User Button
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		//Use PA0 for EXTI_Line0
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
		
		//PA0 connected to EXTI_Line0
		EXTI_InitStruct.EXTI_Line = EXTI_Line0;
		//Enable interrupt
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		//Interrupt mode
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		//Triggers on rising/falling edge
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
		//Add to EXTI
		EXTI_Init(&EXTI_InitStruct);
	
		//Add IRQ vector to NVIC
		NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0;
    	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    	NVIC_Init(&NVIC_InitStruct);
}

//PA0 interrupt handler
void EXTI0_IRQHandler(void){

	if(!!EXTI_GetITStatus(EXTI_Line0) == 1){
		
		insert(1);
		EXTI_ClearITPendingBit(EXTI_Line0);
		
		
	}

}

int main(void) {
	init_GPIO();
	//insert(1);
	
	// LEDs set when User Button on, LEDs reset when User Button off
	while(1){

		if (queue[0] =='\0'){
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
		}

		if (queue[1] != '\0'){

			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}
		if (queue[2] != '\0'){
	
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
		}
		if (queue[3] != '\0'){
	
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
		}
		
	}
   return 0; 

}
