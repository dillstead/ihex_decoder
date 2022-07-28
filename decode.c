#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "decode.h"
#include "read.h"

#define DATA   0x00
#define END    0x01
#define XSEG   0x02
#define SSEG   0x03
#define XADDR  0x04
#define SADDR  0x05
#define START  0x06
#define FINISH 0x07
#define ERROR  0x08
#define CHKSUM 0x09

struct parser
{
    uint8_t byte_count;
    uint8_t state;
    uint32_t base_addr;
    uint32_t start_addr;
    uint16_t offset;
    uint8_t checksum;
};

static bool validate_checksum(int hex_in, struct parser *parser)
{
    ssize_t num_read;
    uint8_t checksum;
            
    num_read = read_uint8(hex_in, &checksum, &parser->checksum);
    if (num_read == 1)
    {
        if (parser->checksum == 0)
        {
            return true;
        }
        else
        {
#ifdef DISABLE_CHECKSUM
            return true;
#else
            fprintf(stderr, "Error: checksum doesn't match\n");
            return false;
#endif            
        }
    }
    return false;
}
    
bool decode(int hex_in, int bin_out)
{
    FILE *fout;
    int dup_out;
    ssize_t num_read;
    char c;
    struct parser parser = { 0 };

    dup_out = dup(bin_out);
    if (dup_out < 0)
    {
        fprintf(stderr, "Failed dup: %s\n",
                strerror(errno));
        return false;
    }
    bin_out = dup_out;
    
    fout = fdopen(bin_out, "w");
    if (fout == NULL)
    {
        fprintf(stderr, "Failed stream output: %s\n",
                strerror(errno));
        return false;
    }
    
    parser.state = START;
    while (parser.state != FINISH && parser.state != ERROR)
    {        
        switch (parser.state)
        {
        case DATA:
        {
            uint8_t byte;

            printf("%08" PRIX32 "     ", parser.base_addr + parser.offset);
            for (uint8_t i = 0; i < parser.byte_count; i++)
            {
                num_read = read_uint8(hex_in, &byte, &parser.checksum);
                if (num_read < 1)
                {
                    break;
                }
                printf("%02" PRIX8, byte);
            }                     
            if (num_read == 1)
            {
                printf("\n");
                parser.state = CHKSUM;
            }
            else
            {
                parser.state = ERROR;
            }
            break;
        }
        case END:
        {
            if (parser.byte_count == 0)
            {
                if (validate_checksum(hex_in, &parser))
                {
                    printf("Starting address %08" PRIX32 "\n", parser.start_addr);
                    parser.state = FINISH;
                }
                else
                {
                    parser.state = ERROR;                
                }
            }
            else
            {
                fprintf(stderr, "Error: incorrect byte count for end\n");
                parser.state = ERROR;            
            }
            break;
        }
        case XSEG:
        case XADDR:
        {
            uint16_t base_addr;
            
            if (parser.byte_count == 2)
            {
                num_read = read_uint16(hex_in, &base_addr, &parser.checksum);
                if (num_read == 1)
                {
                    parser.base_addr = (uint32_t) base_addr << (parser.state == XSEG ? 4 : 16);
                    parser.state = CHKSUM;
                }
                else
                {
                    parser.state = ERROR;
                }
            }
            else
            {
                fprintf(stderr, "Error: incorrect byte count for %s base\n",
                        parser.state == XSEG ? "segment" : "address");
                parser.state = ERROR;            
            }
            break;
        }
        case SSEG:
        case SADDR:
        {
            if (parser.byte_count == 4)
            {
                num_read = read_uint32(hex_in, &parser.start_addr, &parser.checksum);
                if (num_read == 1)
                {
                    parser.state = CHKSUM;
                }
                else
                {
                    parser.state = ERROR;
                }
            }
            else
            {
                fprintf(stderr, "Error: incorrect byte count for %s start\n",
                        parser.state == SSEG ? "segment" : "address");
                parser.state = ERROR;            
            }
            break;
        }
        case START:
        {
            do
            {
                num_read = read(hex_in, &c, sizeof c);
            }
            while (num_read == 1 && c != ':');
            
            if (num_read < 0)
            {
                fprintf(stderr, "Error reading from file: %s\n",
                        strerror(errno));
                parser.state = ERROR;
            }
            else if (read_uint8(hex_in, &parser.byte_count, &parser.checksum) != 1
                     || read_uint16(hex_in, &parser.offset, &parser.checksum) != 1
                     || read_uint8(hex_in, &parser.state, &parser.checksum) != 1)
            {
                parser.state = ERROR;
            }
            break;
        }
        case CHKSUM:
        {
            if (validate_checksum(hex_in, &parser))
            {
                parser.state = START;
            }
            else
            {
                parser.state = ERROR;                
            }
            break;            
        }
        default:
        {
            fprintf(stderr, "Error: unknown state %d\n",
                    parser.state);
            parser.state = ERROR;
            break;
        }
        }   
    }

    fclose(fout);
    return parser.state == FINISH ? true : false;
}
