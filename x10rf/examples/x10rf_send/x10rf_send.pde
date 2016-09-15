#include <x10rf.h>
#include <Energia.h>
#define tx 11 				// Pin number for the 433mhz OOK transmitter
#define reps 1 				// Number of times that a RF command must be repeated.
#define ledpin BLUE_LED 	// Pin for the led that blinks when a command is send. (0 = no blink)


x10rf myx10 = x10rf(tx,ledpin,reps);

void setup()
{
  myx10.begin();
}

void loop()
{
	// RFXmeter (Google RFXMeter for all possible commands and options) 
	myx10.RFXmeter(12,0,123457); // Send '123457' as meter reading. = ID = 12
	delay(3000); 
	
	// RFXSensor (Google RFXsensor for all possible commands and options) 
	myx10.RFXsensor(1,'t','T',21); //Send temperature 21c (two decimals.) ID = 1
	delay(3000);
	//myx10.RFXsensor(5,'v','t',21); //Send voltage = ID = 5
	delay(3000);

	// x10 Security
	myx10.x10Security(3,0xCC); // Send state 'Normal + Tamper' ID =3
	delay(3000);   
	myx10.x10Security(9,0xF0); // Send state 'Darkness detected' ID =9 
	delay(3000);
  
	// x10Switch
	myx10.x10Switch('B',4, 1); // Switch B4 on
	delay(3000);
	myx10.x10Switch('D',15,0); // Switch D15 off
	delay(3000);
  }