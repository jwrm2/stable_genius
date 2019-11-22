#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>
#include <stdint.h>

#include <array>
#include <string>

// Forward declarations
class Tty;
class Ps2Controller;

/**
    Command bytes for setting things on the PS/2 keyboard.
 */
enum class KeyboardCommand : uint8_t {
    // Sets the LEDs
    set_leds = 0xED,
    // Asks for an echo
    self_test = 0xEE,
    // Sets the scan code set
    set_sc = 0xF0,
    // Acknowledgement from the keyboard
    ack = 0xFA,
    // Resend (failure) from the keyboard
    resend = 0xFE
};

/**
    Different ways for the keyboard to give its output.
 */
enum class KeyboardMode {
    // Always return a key code
    keycode,
    // Return ASCII if possible, otherwise a keycode
    asciicode,
    // Return ASCII if possible, otherwise nothing
    ascii
};

/**
    Different scan codes that might be in use.
 */
enum class KeyboardScan : uint8_t {
    sc1 = 0x01, sc2 = 0x02, sc3 = 0x03
};

/**
    List of the intermediate key codes.
    There are 6 rows and a maximum of 21 columns in each row. However, the
    the actual bounds used are 8 columns and 32 rows. The main key
    section has a maximum of 14 columns. The left in a row is 0. The arrow
    key and ins/del section runs from indices 14 to 16 and the number pad
    from indices 17 to 20. Multimedia/www keys are put in row 6 in an
    arbitrary order. Obviously there are some gaps. 
 */
enum class KeyCode {
    // Bottom row
    lctrl = 0x00, lgui = 0x01, lalt = 0x02, space = 0x03,
    ralt = 0x04, rgui = 0x05, menu = 0x06, rctrl = 0x07,
    larrow = 0x0E, darrow = 0x0F, rarrow = 0x10, k0 = 0x11,
    kdot = 0x13, kenter = 0x14,
    // First row
    lshift = 0x20, backslash = 0x21, z = 0x22, x = 0x23,
    c = 0x24, v = 0x25, b = 0x26, n = 0x27,
    m = 0x28, comma = 0x29, dot = 0x2A, slash = 0x2B,
    rshift = 0x2C, uarrow = 0x2F, k1 = 0x31, k2 = 0x32,
    k3 = 0x33,
    // Second row
    capslock = 0x40, a = 0x41, s = 0x42, d = 0x43,
    f = 0x44, g = 0x45, h = 0x46, j = 0x47,
    k = 0x48, l = 0x49, semi = 0x4A, quote = 0x4B,
    hash = 0x4C, enter = 0x4D, k4 = 0x51, k5 = 0x52,
    k6 = 0x53, kplus = 0x54,
    // Third row
    tab = 0x60, q = 0x61, w = 0x62, e = 0x63,
    r = 0x64, t = 0x65, y = 0x66, u = 0x67,
    i = 0x68, o = 0x69, p = 0x6A, sqopen = 0x6B,
    sqclose = 0x6C, del = 0x6E, end = 0x6F, pdown = 0x70,
    k7 = 0x71, k8 = 0x72, k9 = 0x73,
    // Fourth row
    backtick = 0x80, num1 = 0x81, num2 = 0x82, num3 = 0x83,
    num4 = 0x84, num5 = 0x85, num6 = 0x86, num7 = 0x87,
    num8 = 0x88, num9 = 0x89, num0 = 0x8A, minus = 0x8B,
    equal = 0x8C, back = 0x8D, ins = 0x8E, home = 0x8F,
    pup = 0x90, numlock = 0x91, kslash = 0x92, kstar = 0x93,
    kminus = 0x94,
    // Fifth row
    esc = 0xA0, f1 = 0xA1, f2 = 0xA2, f3 = 0xA3,
    f4 = 0xA4, f5 = 0xA5, f6 = 0xA6, f7 = 0xA7,
    f8 = 0xA8, f9 = 0xA9, f10 = 0xAA, f11 = 0xAB,
    f12 = 0xAC, prtscrn = 0xAE, scrllock = 0xAF, pause = 0xB0,
    // Multimedia etc.
    mvoldown = 0xC0, mvolup = 0xC1, mmute = 0xC2, mplay = 0xC3,
    mstop = 0xC4, mprev = 0xC5, mnext = 0xC6, mselect = 0xC7,
    calc = 0xC8, mycomp = 0xC9, email = 0xCA, wwwhome = 0xCB,
    wwwforward = 0xCC, wwwback = 0xCD, wwwstop = 0xCE, wwwrefresh = 0xCF,
    wwwsearch = 0xD0, wwwfav = 0xD1, power = 0xD2, sleep = 0xD3, wake = 0xD4,
    // Invalid
    inv = 0xFF
};

/**
    Contains information about the meta characters and locks.
 */
struct KeyState {
    bool capslock;
    bool numlock;
    bool scrllock;
    bool lshift;
    bool rshift;
    bool lcrtl;
    bool rctrl;
    bool lgui;
    bool rgui;
    bool lalt;
    bool ralt;
    bool menu;
};

/**
    Abstract base class for keyboard drivers.
 */
class Keyboard {
public:
    Keyboard(const klib::string& d, KeyboardMode m = KeyboardMode::asciicode);

    /**
        Reads a key. Forwards the
        keycode or ASCII character to the device, dependent on current mode.
     */
    virtual void read_key() = 0;

protected:
    // Tty to send the character to.
    Tty* device;
    // The total number of keycodes
    static constexpr size_t num_keycodes = 256;
    // Keeps track of which keys are currently pressed. Indexed by key code.
    klib::array<bool, num_keycodes> pressed;
    // Output mode
    KeyboardMode mode;
    // State of the locks
    bool capslock;
    bool numlock;
    bool scrllock;

    /**
        Translates the number pad keys into their other functions for when
        number lock is off.
     */
    virtual KeyCode numlock_map(KeyCode current) const;

    /**
        Translates a key code into an ASCII character, if possible. Uses the
        current state of shift/caps etc. Note that a control character other
        than shift being pressed guarantees no ASCII representation.

        @param current A key code.
        @retun An ASCII character, or zero if the key code doesn't correspond
               to an ASCII character.
     */
    virtual char get_ascii(KeyCode current) const;

    /**
        Assembles a KeyState based on the current state of modifier keys and
        locks.

        @return KeyState The current state of the modifiers and locks.
     */
    virtual KeyState get_state() const;

    /**
        Sets the LEDs on the keyboard to match the current locks state.
     */
    virtual void set_leds() = 0;
};

/**
    Driver for a PS/2 keyboard.
 */
class Ps2Keyboard : public Keyboard {
public:
    /**
        Initialises the driver.

        @param d Device to send the character to after translation to ASCII.
     */
    Ps2Keyboard(Ps2Controller& p, const klib::string& d,
        KeyboardMode m = KeyboardMode::asciicode,
        KeyboardScan s = KeyboardScan::sc2) :
        Keyboard{d, m},
        scan{s},
        ps2{p}
    {
        set_sc(scan);
        set_leds();
    }

    /**
        Reads a key. Forwards the
        keycode or ASCII character to the device, dependent on current mode.
     */
    virtual void read_key() override;

    /**
        Asks the keyboard to echo a value, as a self test.

        @return Success or failure of the test.
     */
    bool self_test();

    /**
        Sets the LEDs on the keyboard to match the current locks state.
     */
    virtual void set_leds() override;

    /**
        Sets the scan code set to use.

        @param sc The scan code set to set.
     */
    void set_sc(KeyboardScan sc);

    /**
        Checks that the keyboard reported scan code set matches.

        @return Whether the hardware reported and software scan code set match.
     */
    bool check_sc();

private:
    // Scan code set in use
    KeyboardScan scan;
    // PS/2 controller
    Ps2Controller& ps2;

    /**
        Gets the next byte from the keyboard. Determines whether the code is
        complete or whether to wait for the next byte.

        @param codes Store for the bytes read from the scan code.
        @param reading number of bytes read from the scan code.
        @return Whether the scan code is complete.
     */
    bool read_code_sc2(uint8_t* const codes, size_t& reading);

    /**
        Turns the current stored scan code bytes into an intermediate key code.

        @param codes Store for the bytes read from the scan code.
        @param reading number of bytes read from the scan code.
        @return a KeyCode, or KeyCode::inv to indicate invalid.
     */
    KeyCode decode_sc2(uint8_t* const codes, size_t& reading);

    /**
        Check for the special case of pause pressed.

        @param codes Store for the bytes read from the scan code.
        @return Key code for pause or KeyCode::inv to indicate invalid.
     */
    KeyCode check_pause(uint8_t* const codes);

    /**
        Check for the special case of print screen pressed.

        @param codes Store for the bytes read from the scan code.
        @return Key code for print screen or KeyCode::inv to indicate
                invalid.
     */
    KeyCode check_prtscrn_press(uint8_t* const codes);

    /**
        Check for the special case of print screen released.

        @param codes Store for the bytes read from the scan code.
        @return Key code for print screen or KeyCode::inv to indicate
                invalid.
     */
    KeyCode check_prtscrn_rel(uint8_t* const codes);
};

#endif
