# SBK_WRISTBLASTER_CORE
Code for lights and sound effects of a Wrist Blaster replica or other props.

## Installation

After downloading, rename 'src' folder to 'SBK_WRISTBLASTER_CORE' and copy it into Arduino IDE sketches folder. Then open File->Sketchbook->SBK_WRISTBLASTER_CORE->SBK_WRISTBLASTER_CORE.ino.

### Library

Depending on your project hardware, the following library and their dependencies are required :

- For bar meter driver :
    -  MAX71xx >>>   MAX72xx.h (https://github.com/wayoda/MAX72xx)
    -  HT16K33 >>>   HT16K33.h (https://github.com/MikeS11/ProtonPack/tree/master/Source/Libraries/ht16k33-arduino-master)
- For WS2812 LEDs :       Adafruit_NeoPixel.h (https://github.com/adafruit/Adafruit_NeoPixel)
- For DFPlayer Pro (not fully tested yet) : DFRobot_DF1201S.h (https://github.com/DFRobot/DFRobot_DF1201S)
- For DFPlayer Mini :
    - DFRobotDFPlayerMini.h (https://github.com/DFRobot/DFRobotDFPlayerMini)
    - DFPlayerMine_Fast.h (https://github.com/PowerBroker2/DFPlayerMini_Fast)


## Features

 *    DFPlayer mini sound board.
 *    BAR METER : up to 28 segments (configurable) controlled by MAX72xx or HT16K33 drivers
 *    Wrist Blaster Lights (WS2812) :
         *  Cyclotron : GB1/GB2 or Frozen Empire style
         *  Vent  
 *    Lights (WS2812):
         *    Vent
         *    Slowblow indicator
         *    Top white indicator
         *    Top Yellow indicator
         *    Front orange indicator
         *    Firing Rod Indicator
 *    All LED pixel numbers and index are configurable.       
 *    LEDs animations are defined in object functions. Class objects and functions are in separated files to keep the main sketch as short as possible.
 *    There is an option for volume potentiometer that uses software volume control with the audio player. If not define, the volume will be the one declare in the AUDIO PLAYER configuration section.
 *    There is the following regular switches and buttons :
      -    Startup switch : power on/shutting down the wrist blaster by calling starting up / shutting down sequence.
      -    Charging switch :    when wnad charged, shooting is possible.
      -    Theme switch :    for playing themes song instead of the regular wrist blaster function.
      -    Fire button :    it does what it says... It also skips to next themes in themes playing mode.
      -    Fire2 button :    same as fire button but play previous themes in themes playing mode.
 *    Other options :
         -    Smoke module
         -    Rumble module


- ### Configurable

  The SBK_WRISTBLASTER_CONFIG.h file contains all the definitions and options : pins, player module, LEDs index, audio tracks, etc. This is the file you want to customize to adjust your pins setting, LEDs chain and index, audio tracks info and other options. If you like the animations and the wrist blaster workflow, you should not have to change anything else then the config file.

  Be careful when working/changing animations times/speed regarding your audio tracks/pact states durations. They are all gathered in the getLEDsSchemeForThisState() function after the Main Loop in the core file. CAUTION : playing with times and speeds can really mess up the animations flow. It is highly suggested taking notes of the original values and changing a few of them at the time and see the effects.

    
  #### *** ADVANCED USERS ***
  
  If you want to change animations styles and colors, you need to go in the engines files and modified the associated functions or create new ones. Then you will have to implement them in the getLEDsSchemeForThisState() function in the core file.

  If you want to change the wrist blaster state switch/cases workflow, you will have to modify wrist blaster states list and audio tracks list/length/looping in the CONFIG file, and the Main Loop pact states switch/cases contents.

  Sketch mechanic works with different wrist blaster states and transitions in the Wrist Blaster state switches/cases defined in Main Loop. Each wrist blaster state has is initialization stage (stageFlag 0) and looping stage (stageFlag 1). The looping stage includes exit(s) to other stages : switches actions, buttons actions and audio track ending.


- ### Code splited in organized files  

  This code is based on objects programming, objects are defined in their *.h and *.cpp files. Objects instances are created and used in the core file SBK_WRISTBLASTER_CORE.ino. There is also a config file, SBK_WRISTBLASTER_CONFIG.h, where all basic parameters can be changed according to hardware used. This fully compartmented code helps keeping code cleaner and easier to update/maintain. This also prevent having an endless code hard to follow.

- ### Compatible HARDWARE

  This code is intended to be use with any PCB and MCU that fits minimum pins requirements :
  - 5 digital inputs for the switches/buttons, with internal pull up resistor
  - 2 digital outputs for WS2812 LEDs chains
  - 2 pins for serial communications with the audio player (hardware or software)
  - 2 or 3 pins for the bar meter driver : 2 for I2C (SDA/SCL), 3 pins for MAX72xx type (DATA, CLOCK, LOAD)
  - 1 digital output for smoker (optional)
  - 1 digital output for rumbler (optional)
  - 1 analog input for a potentiometer to control volume by audio player : software control (optional)
 
  LEDs should be WS2812 or any supported LEDs by the library.

  Bar meter driver could be MAX7219/7221 SPI/serial driver or the HTHT16K33 I2C driver.
  Bar meter could be 8 to 12 segements common cathode bar meter, or the 28 segements common cathode

  Supported audio player is the DFPlayer Mini, others could be added latter uppon request.

  The actual SBK_WRISTBLASTER_CONFIG.h file as it is should be setup to work with :
  - MikeS11 pcb
  - HT16K33.h with 28 segments bar meter with common cathode
  - 1 LEDs strip for the wrist blaster : vent, indicators and firing jewel, cyclotron
  Always check the PINs definition and LEDs index to fit yours.

- ### MCU Board tested

  This code has been fully tested on the following board, that doesn't mean it's the only boards working.

  - Arduino Nano
  - Arduino Nano Every

## Sound effects

Has examples, you'll find here some sounds effects that have been remixed for this code. The actual config file uses those track numbers and lengths. Note that the exact sources of the original sound files are unknowed, so it is impossible to way if they are copyrighted, but probably are in some way. Use these sound effects examples at your own risk : https://mega.nz/folder/GZ8TFIzK#W5bunWSMubMsOIHVNrYEIA

Note : the clipped files have been reduce in gain to lower distorsion on small speakers.
    
## Give back

The library is free, you don’t have to pay for anything. The author invests time and resources providing this open source code and open-source hardware, please support him with a little contribution at : [paypal.me/sbarab](https://www.paypal.com/paypalme/sbarab). 
You could also consider using the author specific pcb board for your project, they'll be available soon.

## Requests

Please post a new issue if you have request for this code, like new animations. It the best way to keep track and follow requests.

## Contribute

You have a great idea for this project, you can fork this project and make a pull request !

## GNU Lesser General Public License

SBK_WRISTBLASTER_CORE is free software: you can redistribute it and/or  modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
SBK_WRISTBLASTER_CORE is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.en.html) for more details.
You should have received a copy of the GNU Lesser General Public License along with SBK_WRISTBLASTER_CORE.  If not, see [this](https://www.gnu.org/licenses/)

## Credits

This library is written by Samuel Barabé, inspired by other members of the open source community in the GB props replica, like https://github.com/MikeS11, https://github.com/CountDeMonet and many others .


