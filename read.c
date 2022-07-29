#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "read.h"

int checked_read(void *ptr, size_t size, size_t nmemb, FILE *fin, bool report_eof)
{
    size_t num_read;
    
    num_read = fread(ptr, size, nmemb, fin);
    if (num_read < 1)
    {
        if (feof(fin))
        {
            if (report_eof)
            {
                fprintf(stderr, "Error: premature EOF\n");
            }
            return 0;
        }
        else if (ferror(fin))
        {
            fprintf(stderr, "Error reading from file: %s\n",
                    strerror(errno));
            return -1;
        }
    }
    return num_read;
}

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

int read_uint8(FILE *fin, uint8_t *value, uint8_t *checksum)
{
    int num_read;
    unsigned char high;
    unsigned char low;
    unsigned int bin;

    num_read = checked_read(&high, sizeof high, 1, fin, true);
    if (num_read < 1)
    {
        return num_read;
    }

    num_read = checked_read(&low, sizeof low, 1, fin, true);
    if (num_read < 1)
    {
        return num_read;
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

int read_uint16(FILE *fin, uint16_t *value, uint8_t *checksum)
{
    int num_read;
    uint8_t high;
    uint8_t low;

    num_read = read_uint8(fin, &high, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    num_read = read_uint8(fin, &low, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    *value = high << 8 | low;
    return 1;
}

int read_uint32(FILE *fin, uint32_t *four_bytes, uint8_t *checksum)
{
    ssize_t num_read;
    uint16_t high;
    uint16_t low;

    num_read = read_uint16(fin, &high, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    num_read = read_uint16(fin, &low, checksum);
    if (num_read <= 0)
    {
        return num_read;
    }
    *four_bytes = (uint32_t) high << 16 | low;
    return 1;
}
