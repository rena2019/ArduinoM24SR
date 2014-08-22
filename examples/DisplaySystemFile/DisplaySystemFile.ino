/* Example: DisplaySystemFile (displays system properties of the NFC chip)
   Arduino library for ST M24SR Dynamic NFC/RFID tag IC with EEPROM, NFC Forum Type 4 Tag and I2C interface
   (c) 2014 by ReNa http://regnerischernachmittag.wordpress.com/ 
*/

#include <PN532.h>
#include <NfcAdapter.h>
#include <Wire.h>
#include <crc16.h>
#include <M24SR.h>

/*

How to use/install:
1. connect M24SR
   Pinout:
   -------------------------------------------------------------------------------
   M24SR             -> Arduino / resistor / antenna
   -------------------------------------------------------------------------------
   1 RF disable      -> TODO not used
   2 AC0 (antenna)   -> Antenna
   3 AC1 (antenna)   -> Antenna
   4 VSS (GND)       -> Arduino Gnd
   5 SDA (I2C data)  -> Arduino A4
   6 SCL (I2C clock) -> Arduino A5
   7 GPO             -> Arduino D7 + Pull-Up resistor (>4.7kOhm) to VCC
   8 VCC (2...5V)    -> Arduino 5V
   -------------------------------------------------------------------------------
2. download CRC lib
   http://code.google.com/p/arduino/source/browse/avrdude-5.4-arduino/crc16.c
   http://code.google.com/p/arduino/source/browse/avrdude-5.4-arduino/crc16.h
   and store the two files in ~/sketchbook/libraries/crc16

3. download NDEF/PN532 Lib 
   https://github.com/don/NDEF has no toString support !!! 
   TODO use my patched lib
*/

#define gpo_pin 7
M24SR m24sr(gpo_pin);

//http://playground.arduino.cc/Code/AvailableMemory
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void setup() 
{
   Serial.begin(9600);
   //for debug purpose
   //m24sr._verbose = true;
   //m24sr._cmds = true;
   m24sr._setup();

   displayFreeRAM();
   m24sr.displaySystemFile();
}

void loop() 
{
}

void displayFreeRAM() {
  Serial.print(F("free RAM: "));
  Serial.println(freeRam(), DEC);
}

