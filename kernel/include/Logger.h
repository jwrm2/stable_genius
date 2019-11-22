#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

#include <ostream>
#include <string>

// Forward declarations
class CharacterDevice;

class Logger {
public:
    /**
        Sets up the logger without a destination. Calls will do nothing.
     */
    Logger() : dest {nullptr}, char_dest {nullptr}, owned {false} {}

    /**
        Sets the stream up with a character device. Can be used to write before
        the heap exists.

        @param d Device to write to.
     */
    explicit Logger(CharacterDevice* d) :
        dest {nullptr}, char_dest {d}, owned {true}
    {}

    explicit Logger(CharacterDevice& d) :
        dest {nullptr}, char_dest {&d}, owned {false}
    {}

    /**
        Sets up the logger to write to a stream.

        @param s The stream to write to.
     */
    explicit Logger(klib::ostream* s) :
        dest{s}, char_dest{nullptr}, owned{true}
    {}

    explicit Logger(klib::ostream& s) :
        dest{&s}, char_dest{nullptr}, owned{false}
    {}

    /**
        Copy constructor is deleted.

        @param other Other logger to (not) copy.
     */
    Logger(const Logger& other) = delete;

    /**
        Copy assignment is deleted.

        @param other Other logger to (not) copy.
        @return This logger.
     */
    Logger& operator=(const Logger& other) = delete;

    /**
        Move constructor. Pirate the other destination.

        @param other Other logger to pirate from.
     */
    Logger(Logger&& other);

    /**
        Move assignment. Clean up own destination. Pirate the other
        destination.

        @param other Other logger to pirate from.
        @return This logger.
     */
    Logger& operator=(Logger&& other);

    /**
        Destructor. Clean up destination.
     */
    ~Logger();

    /**
        Write an error message.

        @param fmt printf style format string.
        @param ... Substitutions for the format string.
     */
    void error(const char* fmt, ...);

    /**
        Write a warning message.

        @param fmt printf style format string.
        @param ... Substitutions for the format string.
     */
    void warn(const char* fmt, ...);

    /**
        Write an information message.

        @param fmt printf style format string.
        @param ... Substitutions for the format string.
     */
    void info(const char* fmt, ...);

    /**
        Write a message.

        @param fmt printf style format string.
        @param ... Substitutions for the format string.
        @param msg Message to write.
     */
    void write(const char* fmt, ...);
    void write(const klib::string& msg);

    /**
        va_list version of write.

        @param fmt printf style format string.
        @param ... Substitutions for the format string.
     */
    void vwrite(const char* fmt, va_list arg);

    /**
        Get the time since the PIT was activated.

        @return Formatted time since the PIT was activated, or 0 time if it
                hasn't been activated.
     */
    klib::string time();

    /**
        Sets the connected device. Only one output is active at a time, so
        previous streams or character devices are forgotten.

        @param s Character device to write to.
     */
    void device(CharacterDevice* s);

    void device(CharacterDevice& s);

    /**
        Get the connected device. May be nullptr if there isn't one.

        @return Pointer to the current device.
     */
    CharacterDevice* device() { return char_dest; }

    /**
        Get the connected device. May be nullptr if there isn't one.

        @return Const pointer to the current stream.
     */
    const CharacterDevice* device() const { return char_dest; }

    /**
        Sets the connected stream. Only one output is active at a time, so
        previous streams or character deivces are forgotten.

        @param s Stream to write to.
     */
    void stream(klib::ostream* s);

    void stream(klib::ostream& s);

    /**
        Get the connected stream.

        @return Pointer to the current stream.
     */
    klib::ostream* stream() { return dest; }

    /**
        Get the connected stream.

        @return Const pointer to the current stream.
     */
    const klib::ostream* stream() const { return dest; }

    
protected:
    // Stream to print to. Requires the heap to exist.
    klib::ostream* dest;
    // Character device to write to. Does not require the heap to exist.
    CharacterDevice* char_dest;
    // Whether the logger owns the stream or device and needs to free it.
    bool owned;
};

#endif
