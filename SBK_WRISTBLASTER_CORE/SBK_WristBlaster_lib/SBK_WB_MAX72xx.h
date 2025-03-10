/*
 *    SBK_WB_MAX72xx.h  
 *    Derived and adapted from :
 *    
 *    MAX72xx.h library for controling Leds with a MAX7219/MAX7221
 *    Copyright (c) 2007 Eberhard Fahle
 * 
 *    It's only purpose is to drive the bar meter with the MAX72xx
 *    LEDs driver. 
 */

 #ifndef SBK_WB_MAX72xx_H
 #define SBK_WB_MAX72xx_H
 
 #include <Arduino.h>
 #include <avr/pgmspace.h>
 
 class MAX72xx {
     private:
         /* Buffer for SPI data */
         byte spidata[4];   // Reduced from 16 to 4 (enough for 1 command)
         
         /* LED status array (for a single 8x8 display) */
         byte status[8];    // Reduced from 64 to 8 (assuming 1 device)
 
         /* SPI pin configuration */
         int SPI_MOSI;
         int SPI_CLK;
         int SPI_CS;
         
         /* Number of connected MAX72xx devices */
         int maxDevices;
 
         /* Send out a single command to the device */
         void spiTransfer(int addr, byte opcode, byte data);
 
     public:
         /* Constructor */
         MAX72xx(int dataPin, int clkPin, int csPin, int numDevices = 1);
 
         /* Get number of devices */
         int getDeviceCount();
 
         /* Power-saving mode */
         void shutdown(int addr, bool status);
 
         /* Set number of digits/rows to display */
         void setScanLimit(int addr, int limit);
 
         /* Set brightness */
         void setIntensity(int addr, int intensity);
 
         /* Clear display */
         void clearDisplay(int addr);
 
         /* Set a single LED */
         void setLed(int addr, int row, int col, boolean state);
 
         /* Set an entire row */
         void setRow(int addr, int row, byte value);
 
         /* Set an entire column */
         void setColumn(int addr, int col, byte value);
 };
 
 #endif  // MAX72xx_H