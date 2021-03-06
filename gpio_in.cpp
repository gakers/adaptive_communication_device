// Pin numbers to their button
#define up 17 // GPIO17, pin 11
#define down 27 // GPIO27, pin 13
#define left 22 // GPIO22, pin 15
#define right 23 // GPIO23, pin 16
#define select 24 // GPIO24, pin 18

#include <wiringPi.h>
#include <stdio.h>

int main(void)
{
	//Intitialize GPIO
	wiringPiSetupGpio();

	//Initialize input pins
	pinMode(up,INPUT);
	pinMode(down,INPUT);
	pinMode(left,INPUT);
	pinMode(right,INPUT);
	pinMode(select,INPUT);
	
	//Enable pull-down resistor
	pullUpDnControl(up, PUD_DN);
	pullUpDnControl(down, PUD_DN);
	pullUpDnControl(left, PUD_DN);
	pullUpDnControl(right, PUD_DN);
	pullUpDnControl(select, PUD_DN);
	
	//wait for button press
	while(1){

		if(digitalRead(up))
		{
			delay(800);
			return up;
		}
		else if(digitalRead(down))
		{
			delay(800);
			return down;
		}
		else if(digitalRead(left))
		{
			delay(800);
			return left;
		}
		else if(digitalRead(right))
		{
			delay(800);
			return right;
		}
		else if(digitalRead(select))
		{
			delay(800);
			return select;
		}
	}

}
