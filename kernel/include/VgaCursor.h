#ifndef VGACURSOR_H
#define VGACURSOR_H

#include <stddef.h>
#include <stdint.h>

/**
    Class for controlling the displyed cursor in VGA text mode.
 */
class VgaCursor {
public:
    /**
        The possible shapes for the cursor to be displayed as.
     */
    enum class Shape {
        underscore, block
    };

    /**
        Sets the cursor position to the start and the disply to an underscore.
     */
    VgaCursor();

    /**
        Copy constructor and assignment are deleted.
     */
    VgaCursor(const VgaCursor&) = delete;
    VgaCursor& operator=(const VgaCursor&) = delete;

    /**
        Enable or disable the cursor, without changing its position.

        @param state State to set the cursor to.
     */
    void set_enabled(bool state);

    /**
        Set the cursor position. If an invalid position is specified, nothing
        happens.

        @param pos Position (row * width + column)
        @param row The row to set, between 0 and 79 inclusive.
        @param column The column to set, between 0 and 24 inclusive.
        @return Success or failure.
     */
    bool set_pos(size_t pos);
    bool set_pos(size_t row, size_t column);

    /**
        Advance the cursor a set number of spaces. Line wrapping is enabled.
        Negative number are allowed for moving backwards. Moving too far
        backwards will put the cursor in the top left, too far forwards will put
        it in the bottom right. No scrolling will be done.

        @param n Number of positions to advance by.
        @return Whether either end was reached.
     */
    bool advance(int n = 1);

    /**
        Set the cursor mode to either an underscore or a block.

        @param sh Specifies which shape to become.
     */
    void set_shape(Shape sh);

    /**
        Get the current cursor position.

        @return Cursor position.
     */
    size_t pos() const { return cursor_pos; }

    /**
        Maximum value of the cursor position
     */
    static const size_t cursor_max = 80 * 25 - 1;

private:
    // Number of columns
    static constexpr size_t width = 80;
    // Number of rows
    static constexpr size_t height = 25;
    // Cursor position (moves across rows, then wraps to start of row).
    size_t cursor_pos;

    /**
        Gets the maximum possible size of the cursor.

        @return The height of a character cell minus one.
     */
    uint8_t max_scan_line();

    /**
        Sets the top of the cursor rectangle. If the value is not between zero
        and the maximum size, nothing happens. If no value is provided, the
        maximum scan line minus one is used.

        @param value The line for the cursor top.
        @return Whether the value was allowed.
     */
    bool set_top(uint8_t value);
    bool set_top();

    /**
        Sets the bottom of the cursor rectangle. If the value is not between
        zero and the maximum size, nothing happens. If no value is provided, the
        maximum scan line is used.

        @param value The line for the cursor bottom.
        @return Whether the value was allowed.
     */
    bool set_bottom(uint8_t value);
    bool set_bottom();
};

#endif
