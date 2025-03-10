# SBK\_WRISTBLASTER\_CORE 🚀

## Overview

The **SBK\_WRISTBLASTER\_CORE** is a firmware for a custom-built wrist blaster prop, designed to control LED animations, sound effects, and mechanical features through an Arduino Nano Every.

This project is designed specifically for prop enthusiasts, cosplayers, and makers who want to build a realistic and interactive wrist blaster with dynamic lighting effects and customizable sound.

## Features

- 🎶 **Sound Effects:** Controlled using a DFPlayer Mini.
- 💡 **LED Animations:** Custom LED bar meters, cycling lights, and pulsating effects.
- 🔥 **Heat Level Simulation:** LED bar that rises with "heat level" and cools down when not in use.
- 🎛 **Potentiometer Control:** Adjust volume and firing strobe hue.
- 📟 **MAX7219 or HT16K33 Support:** Custom bar meter display using MAX7219 or HT16K33 leds driver.
- 🎮 **Run without player:** code can run without a functionnal player, easier to DEBUG.
- 📊 **Serial Debugging:** Real-time debugging through the Arduino IDE.

## Hardware Requirements

- Arduino Nano Every (do not confond with regular Nano)
- DFPlayer Mini DFR0299 (genuine) and SD card
- A fully populated custom SBK Wristblaster PCB (please email here for inquirires : SmartBuildsKits@gmail.com)
- 2x B10K Potentiometers (for analog inputs)
- 4x mini blue toggle switch
- 5x WS2812 single led pixels
- 2x WS2812 7 leds jewel
- 1x 16mm Momentary Push Button with yellow led indicator R16-503
- 1x 28segments bar meter BL28-3000-Sx04Y
- Proper led driver PCBs for bar meter (HT16K33 or MAX72xx driver) : we suggest using the SBK_BG_xx_DRIVER and SBK_BG_28SEG PCBs using MAX7221 leds driver.
- Power pack 6-12V : we suggest using a 6V NiMh flat pack that fits the battery holder 3D model in this project, but it's up to your choice.

## Installation

1. **Clone the repository**:

```bash
git clone https://github.com/SmartBuildsKits/SBK_WRISTBLASTER_CORE.git
cd SBK_WRISTBLASTER_CORE
```
2.  **Move the files in the right folders**

- Put **SBK_WRISTBLATER_CORE.ino** and **SBK_WRISTBLATER_CONFIG.h** files in a folder name "SBK_WRISTBLATER_CORE" in your Arduino IDE **sketch** folder.
- Put the **SBK_WristBlaster_lib** folder in the Arduino Ide **sketch/library** folder.

3. **Install required libraries**:
   
   Open **Arduino IDE** and install those libraries *with all dependancies* with the Libraries Manager :
    **Adafruit_NeoPixel.h**
    **DFPlayerMini_Fast.h**

3. **Upload the code**:

- Open the **SBK_WRISTBLATER_CORE.ino** file in **Arduino IDE**.
- You'll see two tabs in your Arduino IDE : one is the configuration file, the other one is the main code file.
- You don't need to modify the main code file **SBK_WRISTBLATER_CORE.ino**, leave it as it is.
- Verify your configuration and options in the **SBK_WRISTBLATER_CONFIG.h** file.
- Plug your Arduino Nano Every board to your computer with the rigth USB cable.
- Select the COM port and **Arduino Nano Every** as the board.
- Upload the sketch.

## Usage

- **Power On:** Flip the main power switch.
- **Fire Mode:** Press the trigger button to initiate the firing sequence with LED and sound effects.
- **Cooling Mode:** The bar graph slowly reduces after firing, simulating cooldown.
- **Party Mode:** Engage a looping lighting effect with synchronized sounds.

## Schematic



## Demo Video



## Contributing

Pull requests are welcome! If you'd like to contribute to improving this project, please fork the repository and submit your changes.

## License

This project is licensed under the **MIT License**.

## Contact

📧 **Email:** [SmartBuildsKits@gmail.com](mailto\:SmartBuildsKits@gmail.com)

🌐 **Website:** [https://github.com/SmartBuildsKits](https://github.com/SmartBuildsKits)

## Support

💻 **Inquiries for PCB orders** can be sent to [SmartBuildsKits@gmail.com](mailto\:SmartBuildsKits@gmail.com).

💸 This project can also be supported by **purchasing a PCB** or **donating via PayPal** to help continue development.

---

Enjoy building your wrist blaster! 🚀💥

