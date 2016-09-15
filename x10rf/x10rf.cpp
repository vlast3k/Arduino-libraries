/*
x10rf.cpp 
Library for sending x10 messages by RF.
Created by Pieter Paul Baron (embedded [at] ppbaron.nl), November 2013.
Released into the public domain.

Library to send x10 messages via a cheap 433Mhz OOK device. No X10 Firecracker (CMA17A) necessary.
Decoding messages is not implemented.
This library can emulate x10 switches and security devices and also RFXMeter and RFXSensor devices manufactured by RFXCom. (www.rfxcom.com)

Tested on a TI Stellarpad (LM4F120H5QR) and Energia 0101E0010. This should also work on Arduino (small modifications) or other TI Launchpad devices.
*/

#include <stdlib.h>
#include <stdint.h>
#include <Arduino.h>
//#include <Energia.h>
#include "x10rf.h"

#define X10_RF_SB_LONG          8960 	// Start burts (leader) = 9ms
#define X10_RF_SB_SHORT         4500 	//Start silecence (leader) = 4,5 ms
#define X10_RF_BIT_LONG         1120 	// Bit 1 pulse length
#define X10_RF_BIT_SHORT         560 	// Bit 1 pulse length
#define X10_RF_GAP             40000 	// Length between commands

void x10rf::begin()
{
	pinMode(_tx_pin, OUTPUT);
	if (_led_pin > 0) pinMode(_led_pin, OUTPUT); 
} 

x10rf::x10rf(uint8_t tx_pin, uint8_t led_pin, uint8_t rf_repeats)
{
		_tx_pin = tx_pin;
		_led_pin = led_pin;		
		_rf_repeats = rf_repeats;
}

void x10rf::RFXmeter(uint8_t rfxm_address, uint8_t rfxm_packet_type, long rfxm_value){
	uint8_t x10buff[5]; // Set message buffer
	x10buff[0] = rfxm_address;
	x10buff[1] = (~x10buff[0] & 0xF0) + (x10buff[0] & 0xF); // Calculate byte1 (byte 1 complement upper nibble of byte0)
	if (rfxm_value > 0xFFFFFF) rfxm_value = 0; 	// We only have 3 byte for data. Is overflowed set to 0	
	// Packet type goed into MSB nibble of byte 5. Max 15 (B1111) allowed
	// Use switch case to filter invalid data types
	switch(rfxm_packet_type) {
		case 0x00: //Normal. Put counter values in byte 4,2 and 3
			x10buff[4] = (uint8_t) ((rfxm_value >> 16) & 0xff);
			x10buff[2] = (uint8_t) ((rfxm_value >> 8) & 0xff);
			x10buff[3] = (uint8_t) (rfxm_value & 0xff);
			break;
		case 0x01: // New interval time set. Byte 2 should be filled with interval
			switch(rfxm_value) {
				case 0x01: break;	// 30sec
				case 0x02: break;	// 01min
				case 0x04: break;	// 06min (RFXpower = 05min)
				case 0x08: break;	// 12min (RFXpower = 10min)
				case 0x10: break; 	// 15min
				case 0x20: break; 	// 30min
				case 0x40: break; 	// 45min
				case 0x80: break; 	// 60min
				default:
				rfxm_value = 0x01; // Set to 30 sec if no valid option is found
			}
			x10buff[2] = rfxm_value;
			break;
		case 0x02: // calibrate value in <counter value> in µsec.	
			x10buff[4] = (uint8_t) ((rfxm_value >> 16) & 0xff);
			x10buff[2] = (uint8_t) ((rfxm_value >> 8) & 0xff);
			x10buff[3] = (uint8_t) (rfxm_value & 0xff);		
			break; 
		case 0x03: break;// new address set 
		case 0x04: break; // counter value reset to zero 
		case 0x0B: // counter value set
			x10buff[4] = (uint8_t) ((rfxm_value >> 16) & 0xff);
			x10buff[2] = (uint8_t) ((rfxm_value >> 8) & 0xff);
			x10buff[3] = (uint8_t) (rfxm_value & 0xff);
			break; 
		case 0x0C: break; // set interval mode within 5 seconds
		case 0x0D: break; // calibration mode within 5 seconds
		case 0x0E: break; // set address mode within 5 seconds
		case 0x0F: // identification packet (byte 2 = address, byte 3 = interval)
			switch(rfxm_value) {
				case 0x01: break;	//30sec
				case 0x02: break;	//01min
				case 0x04: break;	//06min (RFXpower = 05min)
				case 0x08: break;	//12min (RFXpower = 10min)
				case 0x10: break; // 15min1
				case 0x20: break; // 30min
				case 0x40: break; // 45min
				case 0x80: break; // 60min
				default:
				rfxm_value = 0x01; // Set to 30 sec if no valid option is found
			}
			x10buff[2] = rfxm_address;
			x10buff[3] = rfxm_value;
			break;
		default: //Unknown packet type. Set packet type to zero and set counter to rfxm_value
			rfxm_packet_type = 0;
			x10buff[4] = (uint8_t) ((rfxm_value >> 16) & 0xff);
			x10buff[2] = (uint8_t) ((rfxm_value >> 8) & 0xff);
			x10buff[3] = (uint8_t) (rfxm_value & 0xff);
	}
	x10buff[5] = (rfxm_packet_type << 4); 		// Packet type goes into byte 5's upper nibble.
	// Calculate parity which
	uint8_t parity = ~(((x10buff[0] & 0XF0) >> 4) + (x10buff[0] & 0XF) + ((x10buff[1] & 0XF0) >> 4) + (x10buff[1] & 0XF) + ((x10buff[2] & 0XF0) >> 4) + (x10buff[2] & 0XF) + ((x10buff[3] & 0XF0) >> 4) + (x10buff[3] & 0XF) + ((x10buff[4] & 0XF0) >> 4) + (x10buff[4] & 0XF) + ((x10buff[5] & 0XF0) >> 4));
	x10buff[5] = (x10buff[5] & 0xf0) + (parity & 0XF);
	SendCommand(x10buff, sizeof(x10buff)); // Send byte to be broadcasted
}

void x10rf::RFXsensor(uint8_t rfxs_address,uint8_t rfxs_type, char rfxs_packet_type, uint8_t rfxs_value){
	uint8_t x10buff[3]; // Set message buffer 4 bytes
	x10buff[0] = (rfxs_address << 2);
	switch (rfxs_type) {
		case 't': break; 	// Temperature (default)
		case 'a': 			// A/D
			x10buff[0] = x10buff[0] + B01;
		break;
		case 'm':			// message
			x10buff[0] = x10buff[0] + B11;
		break;
		case 'v':			// voltage
			x10buff[0] = x10buff[0] + B10;
		break;
		}
	x10buff[1] = (~x10buff[0] & 0xF0) + (x10buff[0] & 0xF); // Calculate byte1 (byte 1 complement MSB nibble of byte0)
	x10buff[2] = rfxs_value;
		switch(rfxs_packet_type) {
			case 't': //temperature sensor (MSB = 0.5 degrees bit off)
				x10buff[3] = 0x00;
			break; 
			case 'T': //emperature sensor (MSB = 0.5 degrees bit on)  
				x10buff[3] = 0x80;
			break; 
			case 'h': //RFU (humidity sensor)  
				x10buff[3] = 0x20;
			break;
			case 'p': //RFU (pressure sensor)
				x10buff[3] = 0x40;
			break; 
			default:
				x10buff[3] = 0x00;
			}
	x10buff[3] << 4;
	uint8_t parity = ~(((x10buff[0] & 0XF0) >> 4) + (x10buff[0] & 0XF) + ((x10buff[1] & 0XF0) >> 4) + (x10buff[1] & 0XF) + ((x10buff[2] & 0XF0) >> 4) + (x10buff[2] & 0XF) + ((x10buff[3] & 0XF0) >> 4));
	x10buff[3] = (x10buff[3] & 0xf0) + (parity & 0XF);	
	SendCommand(x10buff, sizeof(x10buff));
}

void x10rf::x10Switch(char house_code, uint8_t unit_code, uint8_t command){
	uint8_t x10buff[3]; // Set message buffer 4 bytes
	uint8_t bTmp = 0; // Tmp byte
	switch(house_code) {
		case 'a': x10buff[0] = B0110; break;
		case 'b': x10buff[0] = B0111; break;
		case 'c': x10buff[0] = B0100; break;
		case 'd': x10buff[0] = B0101; break;
		case 'e': x10buff[0] = B1000; break;
		case 'f': x10buff[0] = B1001; break;
		case 'g': x10buff[0] = B1010; break;
		case 'h': x10buff[0] = B1011; break;
		case 'i': x10buff[0] = B1110; break;
		case 'j': x10buff[0] = B1111; break;
		case 'k': x10buff[0] = B1100; break;
		case 'l': x10buff[0] = B1101; break;		
		case 'm': x10buff[0] = B0000; break;
		case 'n': x10buff[0] = B0001; break;
		case 'o': x10buff[0] = B0010; break;
		case 'p': x10buff[0] = B0011; break;
		default: x10buff[0] = 0; break;
	}
	x10buff[0] = x10buff[0] << 4; // House code goes into the upper nibble

	switch(command) {
		case 0: x10buff[2] = (x10buff[2] | 0x20); break;
		case 1: x10buff[2] = (x10buff[2] & 0x20); break;
	}
	// Set unit number
	unit_code = unit_code - 1;
	bitWrite(x10buff[2],6,bitRead(unit_code,2));
	bitWrite(x10buff[2],3,bitRead(unit_code,1));
	bitWrite(x10buff[2],4,bitRead(unit_code,0));
	bitWrite(x10buff[0],2,bitRead(unit_code,3));
	// Set parity
	x10buff[1] = ~x10buff[0];
	x10buff[3] = ~x10buff[2];
	SendCommand(x10buff, sizeof(x10buff));
}

void x10rf::x10Security(uint8_t address, uint8_t command){
	uint8_t x10buff[3]; // Set message buffer 4 bytes
	x10buff[0] = address;
	x10buff[1] = (~x10buff[0] & 0xF) + (x10buff[0] & 0xF0); // Calculate byte1 (byte 1 complement 
	x10buff[2] = command;
	x10buff[3] = ~x10buff[2];
	// x10buff[4] = code; // Couldn't get 48 bit security working.
	// if((x10buff[4] % 2) == 0) { x10buff[5] = 0;} //Calc even parity
	// else { x10buff[5] = 0x80;}	
	SendCommand(x10buff, sizeof(x10buff));

}

void x10rf::SendCommand(uint8_t *data, uint8_t size){
	if (_led_pin > 0) digitalWrite(_led_pin, HIGH); 
	for (int i = 0; i < _rf_repeats; i++){
		SEND_HIGH();delayMicroseconds(X10_RF_SB_LONG); 
		SEND_LOW();delayMicroseconds(X10_RF_SB_SHORT); 
		for(int i=0; i <= size; i++) {
			SendX10RfByte(data[i]);
		}
	SendX10RfBit(1); 
	delayMicroseconds(X10_RF_GAP);
	}
	if (_led_pin > 0) digitalWrite(_led_pin, LOW); 
}

void x10rf::SendX10RfByte(uint8_t data){
	int i = 0;
	//Serial.println("\n");
	for (int i=7; i >= 0 ; i--){ // send bits from byte 
		SendX10RfBit((bitRead(data,i)==1));
		//Serial.print(bitRead(data,i));
	}
}

void x10rf::SendX10RfBit(unsigned int databit){
    SEND_HIGH();delayMicroseconds(X10_RF_BIT_SHORT);
    SEND_LOW();delayMicroseconds(X10_RF_BIT_SHORT);
    if (databit) delayMicroseconds(X10_RF_BIT_LONG);    
}

void x10rf::SEND_HIGH() {
	digitalWrite(_tx_pin, HIGH);
}

void x10rf::SEND_LOW(){
	digitalWrite(_tx_pin, LOW);
}











