#ifndef ArduboyCore_h
#define ArduboyCore_h

#include <Arduino.h>
#include <SPI.h>
#include <limits.h>


// main hardware compile flags

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#if defined(BOARD_RTL8722D)

#define RST 9
#define DC 8
#define CS 10

#define RED_LED 16
#define GREEN_LED 17
#define BLUE_LED 18
#define TX_LED 7    // Pin 7 for NC
#define RX_LED 7    // Pin 7 for NC

// pin values for buttons, probably shouldn't use these
#define PIN_LEFT_BUTTON 3
#define PIN_RIGHT_BUTTON 5
#define PIN_UP_BUTTON 4
#define PIN_DOWN_BUTTON 2
#define PIN_A_BUTTON 1
#define PIN_B_BUTTON 6

// bit values for button states
#define LEFT_BUTTON _BV(5)
#define RIGHT_BUTTON _BV(6)
#define UP_BUTTON _BV(7)
#define DOWN_BUTTON _BV(4)
#define A_BUTTON _BV(3)
#define B_BUTTON _BV(2)

#define PIN_SPEAKER_1 23
#define PIN_SPEAKER_2 24

#define PIN_SPEAKER_1_PORT 0
#define PIN_SPEAKER_2_PORT 0

#define PIN_SPEAKER_1_BITMASK _BV(6)
#define PIN_SPEAKER_2_BITMASK _BV(7)
#endif

#if defined(BOARD_RTL8722DM_MINI)

#define CS 12
#define DC 13
#define RST 15

// map all LEDs to the single TX LED on DEVKIT
#define RED_LED 17
#define GREEN_LED 20
#define BLUE_LED 21
#define TX_LED 8
#define RX_LED 8

// pin values for buttons, probably shouldn't use these
#define PIN_LEFT_BUTTON 6
#define PIN_RIGHT_BUTTON 3
#define PIN_UP_BUTTON 5
#define PIN_DOWN_BUTTON 4
#define PIN_A_BUTTON 2
#define PIN_B_BUTTON 1

// bit values for button states
#define LEFT_BUTTON _BV(5)
#define RIGHT_BUTTON _BV(6)
#define UP_BUTTON _BV(7)
#define DOWN_BUTTON _BV(4)
#define A_BUTTON _BV(3)
#define B_BUTTON _BV(2)

#define PIN_SPEAKER_1 14
#define PIN_SPEAKER_2 7

#define PIN_SPEAKER_1_PORT 0
#define PIN_SPEAKER_2_PORT 0

#define PIN_SPEAKER_1_BITMASK _BV(6)
#define PIN_SPEAKER_2_BITMASK _BV(7)
#endif

// OLED hardware (SSD1306)

#define OLED_PIXELS_INVERTED 0xA7 // All pixels inverted
#define OLED_PIXELS_NORMAL 0xA6 // All pixels normal

#define OLED_ALL_PIXELS_ON 0xA5 // all pixels on
#define OLED_PIXELS_FROM_RAM 0xA4 // pixels mapped to display RAM contents

#define OLED_VERTICAL_FLIPPED 0xC0 // reversed COM scan direction
#define OLED_VERTICAL_NORMAL 0xC8 // normal COM scan direction

// -----

#define COLUMN_ADDRESS_END (WIDTH - 1) & 0x7F   // 128 pixels wide
#define PAGE_ADDRESS_END ((HEIGHT/8)-1) & 0x07  // 8 pages high

#define WIDTH 128
#define HEIGHT 64

#define INVERT 2 //< lit/unlit pixel
#define WHITE 1 //< lit pixel
#define BLACK 0 //< unlit pixel

class ArduboyCore
{
public:
    ArduboyCore();

    /// allows the CPU to idle between frames
    /**
     * This puts the CPU in "Idle" sleep mode.  You should call this as often
     * as you can for the best power savings.  The timer 0 overflow interrupt
     * will wake up the chip every 1ms - so even at 60 FPS a well written
     * app should be able to sleep maybe half the time in between rendering
     * it's own frames.
     * 
     * See the Arduboy class nextFrame() for an example of how to use idle()
     * in a frame loop.
     */
    void static idle();

    void static LCDDataMode(); //< put the display in data mode

    /// put the display in command mode
    /**
     * See SSD1306 documents for available commands and command sequences.
     * 
     * Links:
     * - https://www.adafruit.com/datasheets/SSD1306.pdf
     * - http://www.eimodule.com/download/SSD1306-OLED-Controller.pdf
     */
    void static LCDCommandMode();

    uint8_t static width();    //< return display width
    uint8_t static height();   // < return display height

    /// get current state of all buttons (bitmask)
    /**
     * Bit mask that is returned:
     *
     *           Hi   Low   
     *  DevKit   00000000    - reserved                         
     *           -DLU-RAB    D down
     *                       U up       
     *  1.0      00000000    L left
     *           URLDAB--    R right
     * 
     * Of course you shouldn't worry about bits (they may change with future
     * hardware revisions) and should instead use the button defines:
     * LEFT_BUTTON, A_BUTTON, UP_BUTTON, etc.
     */

    uint8_t static getInput(); __attribute__ ((deprecated("use buttonsState() instead")));
    uint8_t static buttonsState();

    // paints 8 pixels (vertically) from a single byte
    //  - 1 is lit, 0 is unlit
    //
    // NOTE: You probably wouldn't actually use this, you'd build something
    // higher level that does it's own calls to SPI.transfer().  It's
    // included for completeness since it seems there should be some very
    // rudimentary low-level draw function in the core that supports the
    // minimum unit that the hardware allows (which is a strip of 8 pixels)
    //
    // This routine starts in the top left and then across the screen.
    // After each "page" (row) of 8 pixels is drawn it will shift down
    // to start drawing the next page.  To paint the full screen you call
    // this function 1,024 times.
    //
    // Example:
    //
    // X = painted pixels, . = unpainted
    //
    // blank()                      paint8Pixels() 0xFF, 0, 0x0F, 0, 0xF0
    // v TOP LEFT corner (8x9)      v TOP LEFT corner
    // ........ (page 1)            X...X... (page 1)
    // ........                     X...X...
    // ........                     X...X...
    // ........                     X...X...
    // ........                     X.X.....
    // ........                     X.X.....
    // ........                     X.X.....
    // ........ (end of page 1)     X.X..... (end of page 1)
    // ........ (page 2)            ........ (page 2)
    void static paint8Pixels(uint8_t pixels);

    /// paints an entire image directly to hardware (from PROGMEM)
    /*
     * Each byte will be 8 vertical pixels, painted in the same order as
     * explained above in paint8Pixels.
     */
    void static paintScreen(const unsigned char *image);

    /// paints an entire image directly to hardware (from RAM)
    /*
     * Each byte will be 8 vertical pixels, painted in the same order as
     * explained above in paint8Pixels.
     */
    void static paintScreen(unsigned char image[]);

    /// paints a blank (black) screen to hardware
    void static blank();

    /// invert the display or set to normal
    /**
     * when inverted, a pixel set to 0 will be on
     */
    void static invert(boolean inverse);

    /// turn all display pixels on, or display the buffer contents
    /**
     * when set to all pixels on, the display buffer will be
     * ignored but not altered
     */
    void static allPixelsOn(boolean on);

    /// flip the display vertically or set to normal
    void static flipVertical(boolean flipped);

    /// flip the display horizontally or set to normal
    void static flipHorizontal(boolean flipped);

    /// send a single byte command to the OLED
    void static sendLCDCommand(uint8_t command);

    /// set the light output of the RGB LEB
    void setRGBled(uint8_t red, uint8_t green, uint8_t blue);

protected:
    /// boots the hardware
    /**
     * - sets input/output/pullup mode for pins
     * - powers up the OLED screen and initializes it properly
     * - sets up power saving
     * - kicks CPU down to 8Mhz if needed
     * - allows Safe mode to be entered
     */
    void static boot();

    /// Safe mode
    /**
     * Safe Mode is engaged by holding down both the LEFT button and UP button
     * when plugging the device into USB.  It puts your device into a tight
     * loop and allows it to be reprogrammed even if you have uploaded a very
     * broken sketch that interferes with the normal USB triggered auto-reboot
     * functionality of the device.
     * 
     * This is most useful on Devkits because they lack a built-in reset
     * button.
     */
    void static inline safeMode() __attribute__((always_inline));

    // internals
    void static inline bootLCD() __attribute__((always_inline));
    void static inline bootPins() __attribute__((always_inline));
    void static inline slowCPU() __attribute__((always_inline));
    void static inline saveMuchPower(); __attribute__((always_inline));


private:
    volatile static uint8_t *mosiport, *csport, *dcport;
    uint8_t static mosipinmask, cspinmask, dcpinmask;

};

#endif