// *************************** Info ***************************
// TUNNEL
// Lab 15 - SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Anson Lai - UT.6.02x Embedded Systems - Shape the World

// May 5, 2015
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
 
/* This is a recreation of Tunnel which was a favorite pass time of mine 
in math class on a Ti-83+ calculator.  The goal is to maneuver the ship 
left and right and avoid the sides of the tunnel.  In this version, the 
ship is controlled by the potentiometer.  The switches are there to 
start the game.  Sound assets from Space Invaders are thrown in the mark 
the start and end of the game.  And also the LEDs are warning signals.


Special shout out to the two professors Dr. Jonathan Valvano and 
Dr. Ramesh Yerraballi for the fantastic course!  And also to Jason King 
who was so incredibly helpful on the forum and worked with a few of the 
quirks in my code.
*/

// ******* Required Hardware I/O connections*******************
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

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "Sound.h"
#include "SubFile1.c"
#include "SubFile2.c"
#include "SubFile3.c"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(unsigned long count); // time delay in 0.1 seconds

// *************************** Images ***************************
// Ship, upside down triangle
// 8x7
const unsigned char Ship[] = {
 0x42, 0x4D, 0x92, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x80, 0xFF, 0x00, 0x80, 
 0x00, 0xFF, 0x00, 0x80, 0x80, 0xFF, 0x80, 0x00, 0x00, 0xFF, 0x80, 0x00, 0x80, 0xFF, 0x80, 0x80, 0x00, 0xFF, 0x80, 0x80, 0x80, 0xFF, 0xC0, 0xC0, 0xC0, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 
 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x0F, 0xFF, 
 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0xFF, 0xF0, 0xFF};

// Wall segment, straight line
// 2x4
const unsigned char Wall[] ={
 0x42, 0x4D, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00,
 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xFF,

};

// Splash screen for the start of the game
// 84x48
const char Splash[] = {
 0xFF, 0xFF, 0x3F, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x8F, 0x8F, 0x8F, 0x8F, 0x87, 0x87, 
 0x07, 0x07, 0x07, 0x07, 0x8F, 0x8F, 0x8F, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x1F, 
 0x1F, 0x1F, 0x0F, 0x8F, 0x8F, 0x8F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x87, 0x87, 0x87, 
 0x0F, 0x0F, 0x0F, 0x0F, 0x8F, 0x8F, 0x9F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x8F, 0x8F, 
 0x8F, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x9F, 0x9F, 0x9F, 0x0F, 0x0F, 0x0F, 0x0F, 
 0x0F, 0x0F, 0x87, 0x87, 0x87, 0x07, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x1F, 0xFF, 0xFF, 
 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x06, 0x07, 0xE7, 0xFF, 0xFF, 0xFF, 0x1F, 0x03, 
 0x03, 0xC0, 0xFC, 0xFF, 0xFF, 0x3F, 0xC7, 0xE0, 0xF8, 0xFE, 0xFE, 0xFE, 0x00, 0x00, 
 0x80, 0xF0, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFC, 0xE0, 0xF8, 0xFF, 0x7F, 0x0F, 0x01, 
 0x00, 0x80, 0xF0, 0xFF, 0xFF, 0x3F, 0xFF, 0xFF, 0xFC, 0xE0, 0xF8, 0xFF, 0x7F, 0x0F, 
 0x01, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xEF, 0xF3, 0xFD, 0xFF, 0x7F, 0x07, 0x00, 0xF0, 
 0xFE, 0xFF, 0xFF, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
 0xFF, 0xFF, 0xC0, 0x80, 0x80, 0x80, 0xC0, 0xCF, 0xCF, 0xCF, 0xCF, 0xC1, 0xE0, 0xE0, 
 0xE0, 0xEF, 0xEF, 0xEF, 0xCF, 0xCF, 0xC7, 0xC1, 0xC7, 0xCF, 0x8F, 0x8F, 0x8C, 0x8E, 
 0x8F, 0x87, 0xC3, 0xC0, 0xC0, 0xC0, 0xC3, 0xCF, 0xEF, 0xEF, 0xE3, 0xE0, 0xE0, 0xE0, 
 0xCE, 0xCF, 0xC7, 0xC3, 0xC0, 0xC0, 0x80, 0x87, 0x8F, 0x8F, 0x8F, 0x81, 0xC0, 0xC0, 
 0xC0, 0xC7, 0xCF, 0xCF, 0xEF, 0xEF, 0xEE, 0xEE, 0xEE, 0xE7, 0xC7, 0xC1, 0xCF, 0xDF, 
 0xDF, 0xDF, 0x8E, 0x8E, 0x86, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE0, 0xFF, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 
 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 
 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 
 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
 0xFF, 0xC0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 
 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xC0, 0xFF, 
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// *************************** Global Variables ***************************
unsigned long ADCdata;              // 12-bit 0 to 4095 sample
unsigned long Score = 0;            // Global score, counts how many rounds passed
unsigned long RateConstant = 10;    // Together Rate speeds up the game every X rounds
unsigned long RateCount = 0;
 
unsigned long DifficultyConstant = 50000;  // Together Difficulty determines the speed of the game
unsigned long DifficultyCount = 0;

unsigned long In;          // input from PB4-5
unsigned long Center = 0;  // boolean indicating if Ship is centered (for Splash only)

int State = 0;             // State of the game {0: Main menu, 1: In Game, 2: Game Over}

// Wall Array notes the left side of the center gap for the ship
// There are 12 steps to the tunnel, initialized as a straight path down the center
int WallArray[12] = {24,24,24,24,24,24,24,24,24,24,24,24};


// *************************** Functions ***************************

// Generates a random integer [min, min+16], will output 0 if negative
int RandomLong(int min) {
	int output;
	output = ((Random())%16)+min;
	if(output<0){
		return 0;
	}
  return output;
}

// Draw method, continually called in SysTick and Main
void Draw(void){
	Nokia5110_ClearBuffer();  // clear the current buffer
	// If on Splash Screen, draw splash screen, ship and instructions to start
	if(State == 0){
		Nokia5110_DrawFullImage(Splash);
		Nokia5110_PrintBMP(ADCdata, 46, Ship, 0); // ADCdata is value from potentiometer, place ship accordingly
		if(Center == 1){
			Nokia5110_SetCursor(3, 4);
			Nokia5110_OutString("PRESS!");
		}else{
		Nokia5110_SetCursor(3, 4);
		Nokia5110_OutString("SLIDE!");
		}
	}
	
	// If game is running, draw the ship and iterate through the wall arrays to create the tunnel
	if(State == 1){
		int i;
		int z;
		Nokia5110_PrintBMP(ADCdata, 46, Ship, 0);
		for(i=0;i<12;i++){                          // iterate through all 12 steps of the tunnel
			for(z=0;z<WallArray[i];z++){              // draw the left side of the tunnel
				Nokia5110_PrintBMP(z, i*4+4, Wall, 0);  // z is horizontal, i is vertical markers
			}
			for(z=WallArray[i]+36;z<85;z++){          // draw the right side of the tunnel
				Nokia5110_PrintBMP(z, i*4+4, Wall, 0);
			}
		}
	}
	
	// If the game is over, show score
	if(State == 2){
		Nokia5110_SetCursor(1, 1);
		Nokia5110_OutString("GAME OVER");
		Nokia5110_SetCursor(1, 2);
		Nokia5110_OutString("");
		Nokia5110_SetCursor(1, 3);
		Nokia5110_OutString("Your Score:");
		Nokia5110_SetCursor(2, 4);
		Nokia5110_OutUDec(Score);
	}
	Nokia5110_DisplayBuffer();     // draw buffer
}


	
// Initialize SysTick interrupts
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;     // reload value
  NVIC_ST_CURRENT_R = 0;        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
  EnableInterrupts();
}



// Handler, runs continuously to collect sample from ADC and to draw
void SysTick_Handler(void){
	ADCdata = ADC0_In()/53.88;   // Potentiometer value normalized to 0-84
	if(State == 1){
		Draw();                    // Draw method called on faster SysTick in game
	}
}

// Main method, initializes and deals with splash and game over states
int main(void){
	int ind;               // index counter for looping over wall array
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // clears the screen
	
	// Initializers
	ADC0_Init();
	Switch_Init();
	Sound_Init();
	LED_Init();
	SysTick_Init(20000000);
  Random_Init(NVIC_ST_CURRENT_R); // Seed the random function
	

  while(1){
		// Main thread handles less demanding draw (splash and game over states)
		if(State == 0 | State == 2){
			Draw();
		}
		
		// Check if ship is centered (for splash screen only)
		Center = 0;  // reset Center
		if(20 < ADCdata & ADCdata < 56){
			Center = 1;  // set flag
		}
		
		// Condition to start game (centered ship, switch pressed)
		In = GPIO_PORTE_DATA_R&0x03;   // read PE0-1
		if(In != 0x00 & Center == 1){  // if either switch is pressed, sound then start
			State = 1;
			Sound_Killed();
		}
		
		// Game starts, game engine
		if(State == 1){		
			DifficultyCount++;  // start counter (speed)
		
			if(DifficultyCount==DifficultyConstant){       // Once condition reached, begin next round
				RateCount++;                                 // Count (when high enough, increase speed)
				WallArray[0] = RandomLong(WallArray[1]-8);   // Set first wall to random value (centered around WallArray[1])
				Score++;                                     // Increment score
				DifficultyCount = 0;                         // Reset speed counter
				for(ind=11;ind>0;ind--){                     // Shift each wall down one level
					WallArray[ind] = WallArray[ind-1];
				}
				if(RateCount == RateConstant){               // If rate count is sufficient, increase the speed by 5%
					RateCount = 0;
					DifficultyConstant = DifficultyConstant / 1.05;
				}
			}
			
			// Collision tests
			if(ADCdata < WallArray[10] +1 | ADCdata > WallArray[10] + 28){     // Collision test, 2 pixel leeway given
				State = 2;                  // Game over
				Sound_Explosion();          // Explosion
			}
			
			// If ship is heading for collision, blink red LED, otherwise keep steady green
			if(ADCdata < WallArray[8] | ADCdata > WallArray[8] + 29){     // Collision test, 2 pixel leeway given
				GPIO_PORTB_DATA_R &= ~0x20;
				GPIO_PORTB_DATA_R ^= 0x10;
			}else{
				GPIO_PORTB_DATA_R &= ~0x10;
				GPIO_PORTB_DATA_R |= 0x20;
			}
			
		}
			
  }

}


