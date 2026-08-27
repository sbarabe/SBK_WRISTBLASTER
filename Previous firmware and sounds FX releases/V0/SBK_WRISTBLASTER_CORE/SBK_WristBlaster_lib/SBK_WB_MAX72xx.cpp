/*
 *    SBK_WB_MAX72xx.cpp  
 *    Adapted from the original MAX72xx library by Eberhard Fahle:  
 *    <https://github.com/wayoda/LedControl>
 * 
 *    Original library:  
 *    MAX72xx.cpp - A library for controlling LEDs with a MAX7219/MAX7221  
 *    Copyright (c) 2007 Eberhard Fahle  
 * 
 *    The purpose of this modified version is solely to drive the bar meter 
 *    using the MAX72xx LED driver for the SBK Wrist Blaster project.
 * 
 *    All credit for the original library goes to Eberhard Fahle.  
 *    This modified version is provided under the same license as the original.  
 */

 #include "SBK_WB_MAX72xx.h"

 // MAX7219 / MAX7221 opcodes
 #define OP_NOOP        0
 #define OP_DIGIT0      1
 #define OP_DIGIT1      2
 #define OP_DIGIT2      3
 #define OP_DIGIT3      4
 #define OP_DIGIT4      5
 #define OP_DIGIT5      6
 #define OP_DIGIT6      7
 #define OP_DIGIT7      8
 #define OP_DECODEMODE  9
 #define OP_INTENSITY   10
 #define OP_SCANLIMIT   11
 #define OP_SHUTDOWN    12
 #define OP_DISPLAYTEST 15
 
 MAX72xx::MAX72xx(int dataPin, int clkPin, int csPin, int numDevices) {
     SPI_MOSI = dataPin;
     SPI_CLK  = clkPin;
     SPI_CS   = csPin;
     maxDevices = (numDevices > 0 && numDevices <= 8) ? numDevices : 8;
 
     pinMode(SPI_MOSI, OUTPUT);
     pinMode(SPI_CLK, OUTPUT);
     pinMode(SPI_CS, OUTPUT);
     digitalWrite(SPI_CS, HIGH);
 
     // Initialize all devices
     for (int i = 0; i < 8; i++) {
         status[i] = 0x00;
     }
     for (int i = 0; i < maxDevices; i++) {
         spiTransfer(i, OP_DISPLAYTEST, 0);  // Disable test mode
         setScanLimit(i, 7);                 // Set scan limit to full 8 digits
         spiTransfer(i, OP_DECODEMODE, 0);   // Use raw values (no BCD decoding)
         clearDisplay(i);                    // Clear display
         shutdown(i, true);                   // Start in shutdown mode
     }
 }
 
 int MAX72xx::getDeviceCount() {
     return maxDevices;
 }
 
 void MAX72xx::shutdown(int addr, bool status) {
     if (addr >= 0 && addr < maxDevices) {
         spiTransfer(addr, OP_SHUTDOWN, status ? 0 : 1);
     }
 }
 
 void MAX72xx::setScanLimit(int addr, int limit) {
     if (addr >= 0 && addr < maxDevices && limit >= 0 && limit < 8) {
         spiTransfer(addr, OP_SCANLIMIT, limit);
     }
 }
 
 void MAX72xx::setIntensity(int addr, int intensity) {
     if (addr >= 0 && addr < maxDevices && intensity >= 0 && intensity < 16) {
         spiTransfer(addr, OP_INTENSITY, intensity);
     }
 }
 
 void MAX72xx::clearDisplay(int addr) {
     if (addr < 0 || addr >= maxDevices) return;
 
     for (int i = 0; i < 8; i++) {
         status[i] = 0;
         spiTransfer(addr, i + 1, 0);
     }
 }
 
 void MAX72xx::setLed(int addr, int row, int col, boolean state) {
     if (addr < 0 || addr >= maxDevices || row < 0 || row > 7 || col < 0 || col > 7) return;
 
     byte mask = B10000000 >> col;
     if (state) {
         status[row] |= mask;
     } else {
         status[row] &= ~mask;
     }
     spiTransfer(addr, row + 1, status[row]);
 }
 
 void MAX72xx::setRow(int addr, int row, byte value) {
     if (addr < 0 || addr >= maxDevices || row < 0 || row > 7) return;
 
     status[row] = value;
     spiTransfer(addr, row + 1, value);
 }
 
 void MAX72xx::setColumn(int addr, int col, byte value) {
     if (addr < 0 || addr >= maxDevices || col < 0 || col > 7) return;
 
     for (int row = 0; row < 8; row++) {
         setLed(addr, row, col, (value >> (7 - row)) & 0x01);
     }
 }
 
 void MAX72xx::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
     if (addr < 0 || addr >= maxDevices) return;
 
     digitalWrite(SPI_CS, LOW);
     shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, opcode);
     shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, data);
     digitalWrite(SPI_CS, HIGH);
 }
 