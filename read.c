#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "read.h"

static unsigned int hex_to_bin(unsigned char hex)
{
    if (hex >= '0' && hex <= '9')
    {
        return hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        return 0xA + hex - 'A';
    }
    else if (hex >= 'a' && hex <= 'f')
    {
        return 0xA + hex - 'a';
    }
    return 256;
}

ssize_t read_uint8(int fd, uint8_t *value, uint8_t *checksum)
{
    ssize_t num_read;
    unsigned char high;
    unsigned char low;
    unsigned int bin;

    num_read = read(fd, &high, sizeof high);
    if (num_read < 0)
    {
        fprintf(stderr, "Error reading from file: %s\n",
                strerror(errno));
        return num_read;
    }
    else if (num_read == 0)
    {
        fprintf(stderr, "Error: premature EOF\n");
        return 0;
    }
    num_read = read(fd, &low, sizeof low);
    if (num_read < 0)
    {
        fprintf(stderr, "Error reading from file: %s\n",
                strerror(errno));
        return num_read;
    }
    else if (num_read == 0)
    {
        fprintf(stderr, "Error: truncated value\n");
        return -1;
    }
    bin = (hex_to_bin(high) << 4) | hex_to_bin(low);
    if (bin > 255)
    {
        fprintf(stderr, "Error: invalid character\n");
        return -1;
    }
    *value = (uint8_t) (bin & 0xFF);
    *checksum += *value;
    return 1;
}

ssize_t read_uint16(int fd, uint16_t *value, uint8_t *checksum)
{
    ssize_t num_read;
    uint8_t high;
    uint8_t low;

    num_read = read_uint8(fd, &high, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    num_read = read_uint8(fd, &low, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    *value = high << 8 | low;
    return 1;
}

ssize_t read_uint32(int fd, uint32_t *four_bytes, uint8_t *checksum)
{
    ssize_t num_read;
    uint16_t high;
    uint16_t low;

    num_read = read_uint16(fd, &high, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    num_read = read_uint16(fd, &low, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    *four_bytes = high << 16 | low;
    return 1;
}
