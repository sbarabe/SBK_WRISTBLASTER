# SBK_WRISTBLASTER_CORE 🚀

## Overview

The **SBK_WRISTBLASTER_CORE** is a firmware for a custom-built wrist blaster prop, inspired by the Ghostbusters: Frozen Empire movie. It is designed to control LED animations, sound effects, and mechanical features through an Arduino Nano Every.

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

The provided schematics show the wiring using the SBK custom PCBs, making the assembly easier and cleaner. Using the SBK_BG_xx_DRIVER for the bar meter and the SBK_BG_28SEG with MAX7221 simplifies the LED matrix connection. The fan and vacuum pump are also directly powered from the PCB's dedicated 5V outputs.

## Demo Video



## Contributing

Pull requests are welcome! If you'd like to contribute to improving this project, please fork the repository and submit your changes.

## License

Shield: [![CC BY 4.0][cc-by-shield]][cc-by]

This work is licensed under a
[Creative Commons Attribution 4.0 International License][cc-by].

[![CC BY 4.0][cc-by-image]][cc-by]

[cc-by]: http://creativecommons.org/licenses/by/4.0/
[cc-by-image]: https://i.creativecommons.org/l/by/4.0/88x31.png
[cc-by-shield]: https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg

This means:

✅ You are free to share (copy and redistribute) and adapt (remix, transform, build upon) the material.

❌ Commercial use is prohibited without the author's consent.

✅ You must give appropriate credit and link to the original project.

For more details, see the full license text: CC BY-NC 4.0.

## Author & Collaborator
### Author: Samuel Barabé
- 👨‍💻 Father of four, engineer, outdoor enthusiast, and passionate maker.
- 🛠 Addicted to MCU programming, coding, and 3D design.
- 👻 Inspired by the rugged, prototype-like aesthetic of Ghostbusters devices.
- 💡 This project was born from my love for creative electronics, practical prototyping, and immersive props.
- 🎁 I especially love making props for my children, bringing their favorite fictional worlds to life.

### Collaborator: David Miyakawa
- 🎨 Talented graphic designer with a deep love for the Ghostbusters lore.
- 🎶 Major contributor to sound effects design, prop look development and screen-accurate, device work flow.
- 🛠 Skilled in assembling, painting, and finishing props to high-quality standards.
- 💡 David's input has been invaluable in achieving realistic soundscapes, design accuracy, and practical prop assembly.

## Contact

📧 **Email:** [SmartBuildsKits@gmail.com](mailto:SmartBuildsKits@gmail.com)

🌐 **Website:** [https://github.com/SmartBuildsKits](https://github.com/SmartBuildsKits)

## Support

💻 **Inquiries for PCB orders** can be sent to [SmartBuildsKits@gmail.com](mailto:SmartBuildsKits@gmail.com).

💸 This project can also be supported by **purchasing a PCB** or **donating via PayPal** to help continue development.

---

Enjoy building your wrist blaster! 🚀💥

