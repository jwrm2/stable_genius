#ifndef IO_H
#define IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
    Outputs a byte of data to an I/O port.

    @param value The value to send.
    @param port Address of the port.
 */
void outb(uint8_t value, uint16_t port);

/**
    Reads a byte of data from an I/O port.

    @param port Address of the port.
    @return Data read from the port.
 */
uint8_t inb(uint16_t port);

/**
    Outputs 2 bytes of data to an I/O port.

    @param value The value to send.
    @param port Address of the port.
 */
void outw(uint16_t value, uint16_t port);

/**
    Reads 2 bytes of data from an I/O port.

    @param port Address of the port.
    @return Data read from the port.
 */
uint16_t inw(uint16_t port);

/**
    Outputs a 4 bytes of data to an I/O port.

    @param value The value to send.
    @param port Address of the port.
 */
void outl(uint32_t value, uint16_t port);

/**
    Reads a 4 bytes of data from an I/O port.

    @param port Address of the port.
    @return Data read from the port.
 */
uint32_t inl(uint16_t port);

/**
    Writes a sequence of 2 byte words to an I/O port.

    @param port Address of the port.
    @param src Address to get the data from.
    @param sz Number of words to write.
 */
void outsw(uint16_t port, const void* src, uint32_t sz);

/**
    Reads a sequence of 2 byte words from an I/O port.

    @param port Address of the port.
    @param buffer Address to write the data to.
    @param sz Number of words to read.
 */
void insw(uint16_t port, void* buffer, uint32_t sz);

/**
    Reads a sequence of 4 byte double words from an I/O port.

    @param port Address of the port.
    @param buffer Address to write the data to.
    @param sz Number of double words to read.
 */
void insl(uint16_t port, void* buffer, uint32_t sz);

/**
    Wait for a bit by sending data to a hopefully unused port (0x80).
 */
void io_wait();

#ifdef __cplusplus
}
#endif

#endif
