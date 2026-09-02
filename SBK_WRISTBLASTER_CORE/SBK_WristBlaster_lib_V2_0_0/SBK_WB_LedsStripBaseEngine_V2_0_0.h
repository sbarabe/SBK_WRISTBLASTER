 /**
 *  This code is part of SBK_WRISTBLASTER_CORE, a codebase for animations and effects
 *  of a Wrist Blaster prop inspired by the movie Ghostbusters: Frozen Empire.
 * 
 *  @author      Samuel Barabé  
 *  @copyright   Copyright (c) 2025-2026 Samuel Barabé  
 *  @license     MIT License (code)  
 *  @version     2.0.0
 *  @link        https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE
 *
 *  For more information, visit the project page: <https://github.com/sbarabe/SBK_WRISTBLASTER/tree/main/SBK_WRISTBLASTER_CORE>.
 *
* @see         https://opensource.org/licenses/MIT
 *
 *  This code is provided "as-is" without any warranty of any kind, either expressed or implied,
 *  including but not limited to the warranties of merchantability or fitness for a particular purpose.
 */

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/*******************************/
/*    SOME GENERAL HELPERS     */
/*******************************/
#ifndef DISABLE
#define DISABLE 0
#endif
#ifndef ENABLE
#define ENABLE 1
#endif
#ifndef SOLID
#define SOLID 0
#endif
#ifndef NO_RAMP
#define NO_RAMP 0
#endif
#ifndef NO_FADE
#define NO_FADE 0
#endif
// Some colors definition
const uint8_t RED[3] = {255, 0, 0};
const uint8_t WHITE[3] = {255, 255, 255};
const uint8_t WARM_WHITE[3] = {255, 200, 150};
const uint8_t BLUE[3] = {0, 0, 255};
const uint8_t YELLOW[3] = {255, 200, 0};
const uint8_t GREEN[3] = {12, 189, 24};
const uint8_t ORANGE[3] = {255, 100, 0};
const uint8_t COOL_BLUE[3] = {50, 50, 255};
const uint8_t BLACK[3] = {0, 0, 0};

class LedsStrip
{
public:
    LedsStrip(Adafruit_NeoPixel *strip);

    bool update();
    bool update(uint32_t now);

protected:
    void _setColorAll(uint8_t start, uint8_t end, uint8_t red, uint8_t green, uint8_t blue);
    void _setColor(uint8_t pixel, uint8_t red, uint8_t green, uint8_t blue);
    uint16_t _rampParameter(uint32_t iniTime, uint16_t rampTime, uint16_t iniPara, uint16_t tgPara, uint16_t updateSpeed);
    void _getCurrentColor(uint8_t pixel, uint8_t &red, uint8_t &green, uint8_t &blue);
    void _clearStrip();
    void _clearPixel(uint8_t pixel);
    void _clearSomePixels(uint8_t start, uint8_t end);

    Adafruit_NeoPixel *_strip; // Pointer to shared NeoPixel instance
    bool _updateRequired;
    uint32_t _now;
    uint32_t _prevUpdate;
    uint8_t _updateSpeed;
    uint32_t _iniTime;
    uint16_t _rampTime;
};

