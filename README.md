# SBK_WRISTBLASTER_CORE 🚀

## Overview

The **SBK_WRISTBLASTER_CORE** is a firmware for a custom-built wrist blaster prop, designed to control LED animations, sound effects, and mechanical features through an Arduino Nano Every.

This project is specifically designed for prop enthusiasts, cosplayers, and makers who want to build a realistic and interactive wrist blaster with dynamic lighting effects and customizable sound.

## Features

- 🎶 **Sound Effects:** Controlled using a DFPlayer Mini.
- 💡 **LED Animations:** Custom LED bar meters, cycling lights, and pulsating effects.
- 🔥 **Heat Level Simulation:** LED bar that rises with the "heat level" and gradually cools down when not in use.
- 🎛 **Potentiometer Control:** Adjust volume and firing strobe hue.
- 📟 **MAX7219 or HT16K33 Support:** Custom bar meter display using MAX7219 or HT16K33 LED drivers.
- 💨 Vacuum Pump and Fan Outputs: Dedicated 5V outputs (0.5A max each) for controlling smoke effects, fully integrated on the PCB.
- 🎮 **Standalone Operation:** The code can run without a functional player, making debugging easier.
- 📊 **Serial Debugging:** Real-time debugging through the Arduino IDE.

## Hardware Requirements

- **Arduino Nano Every** (not to be confused with the regular Nano)
- **DFPlayer Mini DFR0299 (genuine)** and an SD card
- **A fully populated custom SBK Wristblaster PCB** (for inquiries, please email: [SmartBuildsKits@gmail.com](mailto:SmartBuildsKits@gmail.com))
- **2x B10K Potentiometers** (for analog inputs)
- **4x Mini Blue Toggle Switches**
- **5x WS2812 Single LED Pixels**
- **2x WS2812 7-LED Jewels**
- **1x 16mm Momentary Push Button with Yellow LED Indicator** (R16-503)
- **1x 28-Segment Bar Meter** (BL28-3000-Sx04Y)
- **Proper LED Driver PCBs for the Bar Meter** (using HT16K33 or MAX72xx drivers). We recommend using the **SBK_BG_xx_DRIVER** and **SBK_BG_28SEG** PCBs with MAX7221 LED drivers.
- **6-12V Power Pack:** We suggest using a 6V NiMh flat pack that fits the battery holder 3D model in this project, but you may choose your own power source.

## Installation

1. **Download the files:**

- Go to the GitHub repository: [SBK_WRISTBLASTER_CORE](https://github.com/SmartBuildsKits/SBK_WRISTBLASTER_CORE)
- Click on the **Code** button and select **Download ZIP**.
- Extract the ZIP file to your computer.

2. **Move the files to the correct folders:**

- Place **SBK_WRISTBLASTER_CORE.ino** and **SBK_WRISTBLASTER_CONFIG.h** files in a folder named **"SBK_WRISTBLASTER_CORE"** inside your Arduino IDE sketch folder.
- Place the **SBK_WristBlaster_lib** folder inside your Arduino IDE **libraries** folder.

3. **Install required libraries:**

- Open Arduino IDE and use the **Library Manager** to install the following libraries along with their dependencies:
    - **Adafruit_NeoPixel.h**
    - **DFPlayerMini_Fast.h**

4. **Upload the code:**

- Open **SBK_WRISTBLASTER_CORE.ino** in Arduino IDE.
- You'll see two tabs: the **configuration file** and the **main code file**.
- **Do not modify the main code file** (**SBK_WRISTBLASTER_CORE.ino**). It is designed to remain untouched.
- Review and modify your settings in **SBK_WRISTBLASTER_CONFIG.h** according to your build.
- Connect your Arduino Nano Every to your computer using a suitable USB cable.
- Select the correct **COM port** and set **Arduino Nano Every** as your board.
- Upload the sketch.

## Usage

- **Power On:** Flip the main power switch.
- **Fire Mode:** Press the trigger button to initiate the firing sequence with synchronized LED and sound effects.
- **Cooling Mode:** The LED bar graph gradually decreases to simulate cooldown after firing.
- **Party Mode:** Engage a looping lighting effect with synchronized sound playback.

## Schematic



## Demo Video



## Contributing

Pull requests are welcome! If you'd like to contribute to improving this project, please fork the repository and submit your changes.

## License

This project is licensed under the **MIT License**.

## Contact

📧 **Email:** [SmartBuildsKits@gmail.com](mailto:SmartBuildsKits@gmail.com)

🌐 **Website:** [https://github.com/SmartBuildsKits](https://github.com/SmartBuildsKits)

## Support

💻 **Inquiries for PCB orders** can be sent to [SmartBuildsKits@gmail.com](mailto:SmartBuildsKits@gmail.com).

💸 This project can also be supported by **purchasing a PCB** or **donating via PayPal** to help continue development.

---

Enjoy building your wrist blaster! 🚀💥

