#include "Keyboard.h"

#include <cctype>

#include "DevFileSystem.h"
#include "FileSystem.h"
#include "io.h"
#include "Kernel.h"
#include "Logger.h"
#include "Ps2Controller.h"
#include "Tty.h"

/******************************************************************************
 ******************************************************************************/

// Mapping from key code to ASCII when not shifted. 0 is used for invalid.
const char ascii_unshift[256] = {
    0, 0, 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '\b', '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '#', '\n', 0, 0,
    0, 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, '\b', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 0, 0,
    0, 0, '/', '*', '-', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    '\e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Mapping from key code to ASCII when shifted. 0 is used for invalid.
const char ascii_shift[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, '0', 0, '.', '\n', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, 0,
    0, '1', '2', '3', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '@', '~', '\n', 0, 0,
    0, '4', '5', '6', '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, '\b', 0,
    0, '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, '!', '"', 0, '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 0, 0,
    0, 0, '/', '*', '-', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    '\e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/******************************************************************************/

// Mapping from scan code to key code from scan code set 2. A scan code can have
// a maximum of eight bytes, although all 8 are used for only one key (pause).
// If the value is sensible, it is the key code.
// The value KeyCode::inv indicates invalid.
const KeyCode sc2_basic[132] = {
    KeyCode::inv, KeyCode::f9, KeyCode::inv, KeyCode::f5,
    KeyCode::f3, KeyCode::f1, KeyCode::f2, KeyCode::f12,
    KeyCode::inv, KeyCode::f10, KeyCode::f8, KeyCode::f6,
    KeyCode::f4, KeyCode::tab, KeyCode::backtick, KeyCode::inv,
    KeyCode::inv, KeyCode::lalt, KeyCode::lshift, KeyCode::inv,
    KeyCode::lctrl, KeyCode::q, KeyCode::num1, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::z, KeyCode::s,
    KeyCode::a, KeyCode::w, KeyCode::num2, KeyCode::inv,
    KeyCode::inv, KeyCode::c, KeyCode::x, KeyCode::d,
    KeyCode::e, KeyCode::num4, KeyCode::num3, KeyCode::inv,
    KeyCode::inv, KeyCode::space, KeyCode::v, KeyCode::f,
    KeyCode::t, KeyCode::r, KeyCode::num5, KeyCode::inv,
    KeyCode::inv, KeyCode::n, KeyCode::b, KeyCode::h,
    KeyCode::g, KeyCode::y, KeyCode::num6, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::m, KeyCode::j,
    KeyCode::u, KeyCode::num7, KeyCode::num8, KeyCode::inv,
    KeyCode::inv, KeyCode::comma, KeyCode::k, KeyCode::i,
    KeyCode::o, KeyCode::num0, KeyCode::num9, KeyCode::inv,
    KeyCode::inv, KeyCode::dot, KeyCode::slash, KeyCode::l,
    KeyCode::semi, KeyCode::p, KeyCode::minus, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::quote, KeyCode::inv,
    KeyCode::sqopen, KeyCode::equal, KeyCode::inv, KeyCode::inv,
    KeyCode::capslock, KeyCode::rshift, KeyCode::enter, KeyCode::sqclose,
    KeyCode::inv, KeyCode::hash, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::backslash, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::back, KeyCode::inv,
    KeyCode::inv, KeyCode::k1, KeyCode::inv, KeyCode::k4,
    KeyCode::k7, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::k0, KeyCode::kdot, KeyCode::k2, KeyCode::k5,
    KeyCode::k6, KeyCode::k8, KeyCode::esc, KeyCode::numlock,
    KeyCode::f11, KeyCode::kplus, KeyCode::k3, KeyCode::kminus,
    KeyCode::kstar, KeyCode::k9, KeyCode::scrllock, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::f7
};
const KeyCode sc2_extended[126] = {
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::wwwsearch, KeyCode::ralt, KeyCode::inv, KeyCode::inv,
    KeyCode::rctrl, KeyCode::mprev, KeyCode::inv, KeyCode::inv,
    KeyCode::wwwfav, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::lgui,
    KeyCode::wwwrefresh, KeyCode::mvoldown, KeyCode::inv, KeyCode::mmute,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::rgui,
    KeyCode::wwwstop, KeyCode::inv, KeyCode::inv, KeyCode::calc,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::menu,
    KeyCode::wwwforward, KeyCode::inv, KeyCode::mvolup, KeyCode::inv,
    KeyCode::mplay, KeyCode::inv, KeyCode::inv, KeyCode::power,
    KeyCode::wwwback, KeyCode::inv, KeyCode::wwwhome, KeyCode::mstop,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::sleep,
    KeyCode::mycomp, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::email, KeyCode::inv, KeyCode::kslash, KeyCode::inv,
    KeyCode::mnext, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::mselect, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::kenter, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::wake, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::end, KeyCode::inv, KeyCode::larrow,
    KeyCode::home, KeyCode::inv, KeyCode::inv, KeyCode::inv,
    KeyCode::ins, KeyCode::del, KeyCode::darrow, KeyCode::inv,
    KeyCode::rarrow, KeyCode::uarrow, KeyCode::inv, KeyCode::inv,
    KeyCode::inv, KeyCode::inv, KeyCode::pdown, KeyCode::inv,
    KeyCode::inv, KeyCode::pup
};

/******************************************************************************
 ******************************************************************************/

Keyboard::Keyboard(const klib::string& d, KeyboardMode m) :
    device{nullptr},
    mode{m},
    capslock{false},
    numlock{false},
    scrllock{false}
{
    device = dynamic_cast<Tty*>(
        global_kernel->get_vfs()->get_dev()->get_device_driver(d));

    if (!device)
        global_kernel->syslog()->warn(
            "Keyboard output device does not exist\n");

    for (size_t i = 0; i < num_keycodes; ++i)
        pressed[i] = false;
}

/******************************************************************************/

KeyCode Keyboard::numlock_map(KeyCode current) const
{
    switch (current)
    {
    case KeyCode::k0:
        return KeyCode::ins;
    case KeyCode::kdot:
        return KeyCode::del;
    case KeyCode::k1:
        return KeyCode::end;
    case KeyCode::k2:
        return KeyCode::darrow;
    case KeyCode::k3:
        return KeyCode::pdown;
    case KeyCode::k4:
        return KeyCode::larrow;
    case KeyCode::k6:
        return KeyCode::rarrow;
    case KeyCode::k7:
        return KeyCode::home;
    case KeyCode::k8:
        return KeyCode::uarrow;
    case KeyCode::k9:
        return KeyCode::pup;
    default:
        return current;
    }
}

/******************************************************************************/

char Keyboard::get_ascii(KeyCode current) const
{
    // Return a zero (invalid) if any modifier keys other than shift are down
    if (pressed[static_cast<uint8_t>(KeyCode::lctrl)] ||
        pressed[static_cast<uint8_t>(KeyCode::rctrl)] ||
        pressed[static_cast<uint8_t>(KeyCode::lalt)] ||
        pressed[static_cast<uint8_t>(KeyCode::ralt)] ||
        pressed[static_cast<uint8_t>(KeyCode::lgui)] ||
        pressed[static_cast<uint8_t>(KeyCode::rgui)] ||
        pressed[static_cast<uint8_t>(KeyCode::menu)])
        return 0;

    // Whether or not we want the shifted output depends on lshift and rshift,
    // as well as numlock and capslock in a not entirely straightforward way.
    bool shift = pressed[static_cast<uint8_t>(KeyCode::lshift)] || 
                 pressed[static_cast<uint8_t>(KeyCode::rshift)];
    char test;

    // Reverse shift if num lock is on and it's a keypad key code
    if (numlock && (current == KeyCode::k0 || current == KeyCode::kdot ||
                    current == KeyCode::k1 || current == KeyCode::k2 ||
                    current == KeyCode::k3 || current == KeyCode::k4 ||
                    current == KeyCode::k5 || current == KeyCode::k6 ||
                    current == KeyCode::k7 || current == KeyCode::k8 ||
                    current == KeyCode::k9))
    {
        if (shift)
            test = ascii_unshift[static_cast<uint8_t>(current)];
        else
            test = ascii_shift[static_cast<uint8_t>(current)];
    }
    else
    {
        if (shift)
            test = ascii_shift[static_cast<uint8_t>(current)];
        else
            test = ascii_unshift[static_cast<uint8_t>(current)];
    }

    // Reverse alphabetic characters (but not number or punctuation) if caps
    // lock is on.
    if (klib::isalpha(test) && capslock)
    {
        if (shift)
            test = ascii_unshift[static_cast<uint8_t>(current)];
        else
            test = ascii_shift[static_cast<uint8_t>(current)];
    }

    return test;
}

/******************************************************************************/

KeyState Keyboard::get_state() const
{
    return KeyState {
        capslock,
        numlock,
        scrllock,
        pressed[static_cast<uint8_t>(KeyCode::lshift)],
        pressed[static_cast<uint8_t>(KeyCode::rshift)],
        pressed[static_cast<uint8_t>(KeyCode::lctrl)],
        pressed[static_cast<uint8_t>(KeyCode::rctrl)],
        pressed[static_cast<uint8_t>(KeyCode::lgui)],
        pressed[static_cast<uint8_t>(KeyCode::rgui)],
        pressed[static_cast<uint8_t>(KeyCode::lalt)],
        pressed[static_cast<uint8_t>(KeyCode::ralt)],
        pressed[static_cast<uint8_t>(KeyCode::menu)]
    };
}

/******************************************************************************
 ******************************************************************************/

void Ps2Keyboard::read_key()
{
    // For the key code generated
    KeyCode current;
    // Store the bytes of the scan codes
    uint8_t codes[8];
    // For the number of bytes of the scan code read
    size_t reading = 0;

    // Disable interrupts
    ps2.disable_interrupts();

    // Get the key code, dependent on current scan code set
    switch (scan)
    {
    case KeyboardScan::sc2:
        while (!read_code_sc2(codes, reading)) ;
        current = decode_sc2(codes, reading);
        break;
    default:
        global_kernel->syslog()->error("Unsupported keyboard scan set\n");
        return;
    }

    bool output = true;
    // Check for an invalid scan code
    if (current == KeyCode::inv)
    {
        global_kernel->syslog()->error("Invalid key code generated\n");
        output = false;
    }

    // Do nothing if this is just a release.
    if (!pressed[static_cast<uint8_t>(current)])
        output = false;

    if (output && device)
    {
        // Adjust locks
        switch (current)
        {
        case KeyCode::capslock:
            capslock = !capslock;
            set_leds();
            break;
        case KeyCode::numlock:
            numlock = !numlock;
            set_leds();
            break;
        case KeyCode::scrllock:
            scrllock = !scrllock;
            set_leds();
            break;
        default:
            break;
        }

        // Output, dependent on mode
        char c;
        switch (mode)
        {
        case KeyboardMode::keycode:
            device->key(current, get_state());
            break;
        case KeyboardMode::asciicode:
            c = get_ascii(current);
            if(c)
            {
                device->add_char(c);
            }
            else
                device->key(current, get_state());
            break;
        case KeyboardMode::ascii:
            c = get_ascii(current);
            if(c)
                device->add_char(c);
            break;
        }
    }

    // Enable interrupts
    ps2.flush();
    ps2.enable_interrupts();
}

/******************************************************************************/

bool Ps2Keyboard::self_test()
{
    ps2.send_data(static_cast<uint8_t>(KeyboardCommand::self_test));

    uint8_t resp = ps2.get_response();

//    global_kernel->syslog()->info("Performing echo test, received %X from the keyboard\n",
//                 static_cast<unsigned int>(resp));

    bool ret_val = false;
    switch(resp)
    {
    case static_cast<uint8_t>(KeyboardCommand::self_test):
        ret_val = true;
        break;
    case static_cast<uint8_t>(KeyboardCommand::resend):
        break;
    default:
        global_kernel->syslog()->error("Unexpected response to keyboard self-test: %X\n",
                       static_cast<unsigned int>(resp));
    }

    ps2.flush();

    return ret_val;
}

/******************************************************************************/

void Ps2Keyboard::set_leds()
{
    // Prepare to receive LED settings
    ps2.send_data(static_cast<uint8_t>(KeyboardCommand::set_leds));

    // Check result
    uint8_t resp = ps2.get_response();

//    global_kernel->syslog()->info("Preparing to set leds, received %X from the keyboard\n",
//                 static_cast<unsigned int>(resp));

    bool succ = true;
    switch(resp)
    {
    case static_cast<uint8_t>(KeyboardCommand::ack):
        break;
    case static_cast<uint8_t>(KeyboardCommand::resend):
        global_kernel->syslog()->error("Failed to set keyboard LEDs\n");
        succ = false;
        break;
    default:
        global_kernel->syslog()->error("Unexpected response to setting keyboard LEDs: %X\n",
                       static_cast<unsigned int>(resp));
        succ = false;
    }

    if (succ)
    {
        // Assemble the data
        uint8_t data = 0x00;
        if (scrllock)
            data |= 0x01;
        if (numlock)
            data |= 0x02;
        if (capslock)
            data |= 0x04;

        // Send data
        ps2.send_data(data);

        // Check result
        resp = ps2.get_response();

//        global_kernel->syslog()->info("Setting leds, received %X from the keyboard\n",
//                     static_cast<unsigned int>(resp));

        switch(resp)
        {
        case static_cast<uint8_t>(KeyboardCommand::ack):
            break;
        case static_cast<uint8_t>(KeyboardCommand::resend):
            global_kernel->syslog()->error("Failed to set keyboard LEDs\n");
            break;
        default:
            global_kernel->syslog()->error("Unexpected response to setting keyboard LEDs: %X\n",
                          static_cast<unsigned int>(resp));
        }
    }

    ps2.flush();
}

/******************************************************************************/

void Ps2Keyboard::set_sc(KeyboardScan sc)
{
    // Prepare to set scan code set
    ps2.send_data(static_cast<uint8_t>(KeyboardCommand::set_sc));

    // Check result
    uint8_t resp = ps2.get_response();

//    global_kernel->syslog()->info("Preparing to set scan code set, received %X from the keyboard\n",
//                 static_cast<unsigned int>(resp));

    bool succ = true;
    switch(resp)
    {
    case static_cast<uint8_t>(KeyboardCommand::ack):
        break;
    case static_cast<uint8_t>(KeyboardCommand::resend):
        global_kernel->syslog()->error("Failed to set keyboard scan code set\n");
        succ = false;
        break;
    default:
        global_kernel->syslog()->error("Unexpected response to setting keyboard scan code set: %X\n",
                       static_cast<unsigned int>(resp));
        succ = false;;
    }

    if (succ)
    {
        // Send scan code set
        ps2.send_data(static_cast<uint8_t>(sc));

        // Check result
        resp = ps2.get_response();

//        global_kernel->syslog()->info("Setting scan code set, received %X from the keyboard\n",
//                     static_cast<unsigned int>(resp));

        switch(resp)
        {
        case static_cast<uint8_t>(KeyboardCommand::ack):
            break;
        case static_cast<uint8_t>(KeyboardCommand::resend):
            global_kernel->syslog()->error("Failed to set keyboard scan code set\n");
            break;
        default:
            global_kernel->syslog()->error("Unexpected response to setting keyboard scan code set: %X\n",
                          static_cast<unsigned int>(resp));
        }
    }

    ps2.flush();
}

/******************************************************************************/

bool Ps2Keyboard::check_sc()
{
    // Prepare to receive scan code set
    ps2.send_data(static_cast<uint8_t>(KeyboardCommand::set_sc));

    // Check result
    uint8_t resp = ps2.get_response();

    //global_kernel->syslog()->info("Preparing to check scan code set, received %X from the keyboard\n",
    //             static_cast<unsigned int>(resp));

    switch(resp)
    {
    case static_cast<uint8_t>(KeyboardCommand::ack):
        break;
    case static_cast<uint8_t>(KeyboardCommand::resend):
        global_kernel->syslog()->error("Failed to check keyboard scan code set\n");
        return false;
    default:
        global_kernel->syslog()->error("Unexpected response to checking keyboard scan code set: %X\n",
                       static_cast<unsigned int>(resp));
        return false;
    }

    // Send request for current scan code set
    ps2.send_data(0x00);

    // Check result
    resp = ps2.get_response();

    //global_kernel->syslog()->info("Preparing to check scan code set, received %X from the keyboard\n",
    //             static_cast<unsigned int>(resp));

    // Some versions of QEMU send an ACK here, some just go straight to the
    // result. If we get an ACK, we'll just ask for another value.
    if (resp == static_cast<uint8_t>(KeyboardCommand::ack))
        resp = ps2.get_response();

    //global_kernel->syslog()->info("Checking scan code set, received %X from the keyboard\n",
    //             static_cast<unsigned int>(resp));

    bool ret_val = false;
    if (resp == static_cast<uint8_t>(scan))
        ret_val = true;
    else
    {
        switch(resp)
        {
        case static_cast<uint8_t>(KeyboardCommand::resend):
            global_kernel->syslog()->error("Failed to check keyboard scan code set\n");
            break;
        default:
            global_kernel->syslog()->error("Unexpected response to checking keyboard scan code set: %X\n",
                          static_cast<unsigned int>(resp));
        }
    }

    ps2.flush();
    return ret_val;
}

/******************************************************************************/

bool Ps2Keyboard::read_code_sc2(uint8_t* const codes, size_t& reading)
{
    // Read the byte
    codes[reading] = ps2.get_response();
//    global_kernel->syslog()->info("Byte %u of scan code is %X\n",
//        reading, static_cast<unsigned int>(codes[reading]));
    ++reading;

    // We don't worry here about whether the scan code is valid, so we return
    // false (to indicate keep reading) only for valid continuations. For a 
    // complete or an obviously invalid code, we return true;
    switch (reading)
    {
    case 1:
        return !(codes[0] == 0xE0 || codes[0] == 0xF0 || codes[0] == 0xE1);
    case 2:
        return !((codes[0] == 0xE0 && codes[1] == 0xF0) ||
                 (codes[0] == 0xE0 && codes[1] == 0x12) ||
                 (codes[0] == 0xE1 && codes[1] == 0x14));
    case 3:
        return !((codes[0] == 0xE0 && codes[1] == 0x12 && codes[2] == 0xE0) ||
                 (codes[0] == 0xE0 && codes[1] == 0xF0 && codes[2] == 0x7C) ||
                 (codes[0] == 0xE1 && codes[1] == 0x14 && codes[2] == 0x77));
    case 4:
        return !(codes[3] == 0xE0 || codes[3] == 0xE1);
    case 5:
        return codes[4] != 0xF0;
    case 6:
        return codes[5] != 0x14;
    case 7:
        return codes[6] != 0xF0;
    default:
        return true;
    }
}

/******************************************************************************/

KeyCode Ps2Keyboard::decode_sc2(uint8_t* const codes, size_t& reading)
{
    KeyCode ret_val = KeyCode::inv;

    // reading gives the number of bytes in the scan code
//    global_kernel->syslog()->info("%u bytes in scan code\n", reading);
    switch(reading)
    {
    case 1:
        if (codes[0] <= 0x83)
        {
            pressed[static_cast<uint8_t>(sc2_basic[codes[0]])] = true;
            ret_val = sc2_basic[codes[0]];
        }
        break;
    case 2:
        if (codes[0] == 0xE0 && codes[1] <= 0x7D)
        {
            pressed[static_cast<uint8_t>(sc2_extended[codes[1]])] = true;
            ret_val = sc2_extended[codes[1]];
        }
        else if (codes[0] == 0xF0 && codes[1] <= 0x83)
        {
            pressed[static_cast<uint8_t>(sc2_basic[codes[1]])] = false;
            ret_val = sc2_basic[codes[1]];
        }
        break;
    case 3:
        if (codes[0] == 0xE0 && codes[1] == 0xF0 && codes[2] <= 0x7D)
        {
            pressed[static_cast<uint8_t>(sc2_extended[codes[2]])] = false;
            ret_val = sc2_extended[codes[2]];
        }
        break;
    case 4:
        ret_val = check_prtscrn_press(codes);
        break;
    case 6:
        ret_val = check_prtscrn_rel(codes);
        break;
    case 8:
        ret_val = check_pause(codes);
        break;
    default:
        break;
    }

    reading = 0;
    ps2.flush();
    return ret_val;
}

/******************************************************************************/

KeyCode Ps2Keyboard::check_pause(uint8_t* const codes)
{
    uint8_t pause_seq[8] = {0xE1, 0x14, 0x77, 0xE1, 0xF0, 0x14, 0xF0, 0x77};

    for (size_t i = 0; i < 8; ++i)
        if (codes[i] != pause_seq[i])
            return KeyCode::inv;

    pressed[static_cast<uint8_t>(KeyCode::pause)] = true;
    return KeyCode::pause;
}

/******************************************************************************/

KeyCode Ps2Keyboard::check_prtscrn_press(uint8_t* const codes)
{
    uint8_t prtscrn_seq[4] = {0xE0, 0x12, 0xE0, 0x7C};

    for (size_t i = 0; i < 4; ++i)
        if (codes[i] != prtscrn_seq[i])
            return KeyCode::inv;

    pressed[static_cast<uint8_t>(KeyCode::prtscrn)] = true;
    return KeyCode::prtscrn;
}

/******************************************************************************/

KeyCode Ps2Keyboard::check_prtscrn_rel(uint8_t* const codes)
{
    uint8_t prtscrn_seq[6] = {0xE0, 0xF0, 0x7C, 0xE0, 0xF0, 0x12};

    for (size_t i = 0; i < 6; ++i)
        if (codes[i] != prtscrn_seq[i])
            return KeyCode::inv;

    pressed[static_cast<uint8_t>(KeyCode::prtscrn)] = false;
    return KeyCode::prtscrn;
}

/******************************************************************************
 ******************************************************************************/
