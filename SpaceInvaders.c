// SpaceInvaders.c
// Runs on LM4Flag120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// This virtual Nokia project only runs on the real board, this project cannot be simulated
// Instead of having a real Nokia, this driver sends Nokia
//   commands out the UART to TExaSdisplay
// The Nokia5110 is 48x84 black and white
// pixel LCD to display text, images, or other information.

// April 19, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Required Hardware I/O connections*******************
// PA1, PA0 UART0 connected to PC through USB cable
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "stdlib.h"

void Moving_TargetCar(void);
void Moving_Another_Car1(void);
void Moving_Another_Car2(void);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds
void PortF_Init(void);
unsigned long TimerCount;
unsigned long Semaphore;


// *************************** Images ***************************
// main car that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10
const unsigned char TargetCar[] ={
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x0F,
 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};



// the car which the main car racing it that starts at the top of the screen (arms/mouth open)
// width=16 x height=10
const unsigned char AnotherCar[] ={
 0x42, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0xFF,
 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0F,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,

};




// *************************** Capture image dimensions out of BMP**********
#define TargetCarW     ((unsigned char)TargetCar[18])
#define TargetCarH     ((unsigned char)TargetCar[22])
#define AnotherCarW    ((unsigned char)AnotherCar[18])
#define AnotherCarH    ((unsigned char)AnotherCar[22])
	

int FinalScore1=0 , FinalScore2=0;
int routes1[3]={18,28,38};   // the routes in X
int routes2[3]={38,28,18};
int  TargetCarPlayerH=10, TargetCarPlayerX=0, TargetCarPlayerY=28;  //the positions of targetcar's player and its height 
int AnotherCarX, AnotherCarY, AnotherCarMoving1, AnotherCarMoving2;    //another cars position

int AnotherCarX2, AnotherCarY2, AnotherCarMoving12, AnotherCarMoving22; 
int Flag1=0, Flag2=0;

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PFlag2,PFlag1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}

int main(void){ 

PortF_Init();
AnotherCarMoving1=84 ;
AnotherCarMoving2=84 ;
AnotherCarY= rand()%4;

AnotherCarMoving12=70 ;
AnotherCarMoving22=70 ;
AnotherCarY2= rand()%4;	
	
TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
Random_Init(1);
Nokia5110_Init();
Nokia5110_ClearBuffer();
Nokia5110_DisplayBuffer(); 	
	
/*----------------------- intialized screen------------------------------*/	
	Nokia5110_Clear();
  Nokia5110_SetCursor(2, 1);
  Nokia5110_OutString("CarRacing      Game");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutString("BY OMNIA     LASHEN");

	
  while(1){
		
		if(((GPIO_PORTF_DATA_R&0x10) == 0))  // for SW1
			break;
	}  
/*------------------------ displaying the targrt car ---------------------*/
	Nokia5110_ClearBuffer();
	Nokia5110_PrintBMP(TargetCarPlayerX, TargetCarPlayerY, TargetCar, 0);
	Nokia5110_DisplayBuffer();
	Delay100ms(50);

while(1){
		Nokia5110_ClearBuffer();		
		
								
Moving_TargetCar();
Moving_Another_Car1();
Moving_Another_Car2();	



		
if((AnotherCarMoving2 <= TargetCarPlayerX+TargetCarW-2 && routes1[AnotherCarY] == TargetCarPlayerY) || (AnotherCarMoving22 <= TargetCarPlayerX+TargetCarW-2 && routes2[AnotherCarY2] == TargetCarPlayerY )){
			break;
}
		AnotherCarMoving2--;
		AnotherCarMoving22--;
				
//***************** Screen displaying ********************//
		Nokia5110_DisplayBuffer();
		Delay100ms(1);
	}

	Delay100ms(50);
	
//Delay before gameover screen

	Nokia5110_Clear();
  Nokia5110_SetCursor(1, 1);
  Nokia5110_OutString("GAME OVER");
  Nokia5110_SetCursor(1, 2);
  Nokia5110_OutString("Try again");
  Nokia5110_SetCursor(1, 3);
  Nokia5110_OutString("the score:");
  Nokia5110_SetCursor(2, 4);
  Nokia5110_OutUDec(FinalScore1+FinalScore2);  // the no. of cars without clashing it
  Nokia5110_SetCursor(0, 0); 
}

void Moving_TargetCar(void){
	
		if((((GPIO_PORTF_DATA_R&0x10) != 0) || ((GPIO_PORTF_DATA_R&0x01)  != 0))){
			if((GPIO_PORTF_DATA_R&0x10) == 0 ){
				if(TargetCarPlayerY > 18 && Flag1==0) {     
					TargetCarPlayerY -= 10;
					Flag1=1;
				}
			}
			else{
			Flag1=0;}
			

			if(((GPIO_PORTF_DATA_R&0x01) == 0)){
				if((TargetCarPlayerY < 38) && Flag2==0) {      
					TargetCarPlayerY += 10;
					Flag2=1;
				}
			}
			else{
			Flag2=0;}
		}		
		Nokia5110_PrintBMP(TargetCarPlayerX, TargetCarPlayerY, TargetCar, 0);
}

void Moving_Another_Car1(void){
if(AnotherCarMoving1 != 0 ){
			Nokia5110_PrintBMP(AnotherCarMoving1, routes1[AnotherCarY], AnotherCar, 0);
			AnotherCarMoving1--;			
		}
		if(AnotherCarMoving1 == 0 ){
			
			if(AnotherCarMoving2 == 0){
				FinalScore1++;					
				AnotherCarY = rand()%4;
				AnotherCarMoving2 = 84;
				
			}
			
			Nokia5110_PrintBMP(AnotherCarMoving2, routes1[AnotherCarY], AnotherCar, 0);
		}

}

void Moving_Another_Car2(void){
	
if( AnotherCarMoving12 !=0 ){
			Nokia5110_PrintBMP(AnotherCarMoving12, routes2[AnotherCarY2], AnotherCar, 0);
			AnotherCarMoving12--;
		}
		
			
		  if (AnotherCarMoving22 ==0){
				FinalScore2++;					
        AnotherCarY2 = rand()%4;
				AnotherCarMoving22 = 84; 
			
			}
			Nokia5110_PrintBMP(AnotherCarMoving22, routes2[AnotherCarY2], AnotherCar, 0);
		}



// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void Timer2A_Handler(void){ 	
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  TimerCount++;
	Semaphore = 1;
	 // trigger
}
void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 7272408/100;  
    while(time){
	  	time--;
    }
    count--;
  }
}
