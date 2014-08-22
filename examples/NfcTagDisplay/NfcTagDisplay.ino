/* Example: NfcTagDisplay (Displays the content of the EEPROM after changing the NDef record with a NFC phone/Smartcard reader)
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

*optional* (if you want to connect a TFT and display the NDEF record on it):
4. download Adafruit_GFX lib
   https://github.com/adafruit/Adafruit-GFX-Library
   and store files under ~/sketchbook/libraries
   
   download Adafruit_ST7735 lib
   https://github.com/adafruit/Adafruit-ST7735-Library
   and store files under ~/sketchbook/libraries

5. connect TFT
   Arduino      TFT
   ---------------------
   5V (VCC)     LED, VCC
   GND          GND
   D13 (SCK)    SCK
   D11 (MOSI)   SDA 
   D10 (SS)     CS   
   D9           A0
   D8           RESET
*/

// next 5 lines are required (due to Arduino bug) if _TFT_SUPPORT_ is undefined
// BOF preprocessor bug prevent - insert me on top of your arduino-code
// From: http://www.a-control.de/arduino-fehler/?lang=en
#if 1
__asm volatile ("nop");
#endif

//uncomment next line if you have a TFT connected to your arduino
//#define _TFT_SUPPORT_

#ifdef _TFT_SUPPORT_
  #include <Adafruit_GFX.h>    // Adafruit graphic-lib 
  #include <Adafruit_ST7735.h> // Adafruit ST7735-lib
  #include <SPI.h>
                  // Arduino 5V (VCC)  to TFT: LED, VCC
                  // Arduino GND to TFT: GND
                  // Arduino D13 (SCK) to TFT: SCK
                  // Arduino D11 (MOSI) to TFT: 
  #define cs  10  // Arduino D10 (SS) to TFT: CS   
  #define dc   9  // Arduino D9 to TFT: A0
  #define rst  8  // Arduino-D8 to TFT: RESET
  Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
#endif

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

#ifdef _TFT_SUPPORT_
  tft.initR(INITR_BLACKTAB);   // ST7735-Chip initialize
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.fillScreen(ST7735_BLACK);
#endif

  displayFreeRAM();
  m24sr.displaySystemFile();
#ifdef _TFT_SUPPORT_
  Serial.println(F("\r\nwith TFT"));
#else
  Serial.println(F("\r\nwithout TFT"));
#endif
  Serial.print(F("Write/change NFC tag (with your NFC phone) now!"));
}

void loop()
{
    if (m24sr.checkGPOTrigger()) {
        displayNDefRecord();
    }
    delay(200);
}

void displayNDefRecord() {
    //read NDef message from memory
    NdefMessage* pNDefMsg = m24sr.getNdefMessage();
    displayFreeRAM();
    if (pNDefMsg != NULL) {
       pNDefMsg->print();
       NdefRecord rec = pNDefMsg->getRecord(0);
       String txt = rec.toString();
       Serial.print(F("NDefRecord: "));
       Serial.println(txt);
       
#ifdef _TFT_SUPPORT_       
       //display text on TFT
       tft.fillScreen(ST7735_BLACK);
       tft.setCursor(0,0);
       tft.setTextColor(ST7735_WHITE);
       char szBuf[txt.length()+1];
       txt.getBytes((unsigned char*)szBuf, txt.length()+1);
       tft.print(szBuf);
#endif       
       delete pNDefMsg;
    }
}

void displayFreeRAM() {
  Serial.print(F("\r\nfree RAM: "));
  Serial.println(freeRam(), DEC);
}

