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

static bool validate_checksum(FILE *fin, struct parser *parser)
{
    int num_read;
    uint8_t checksum;
            
    num_read = read_uint8(fin, &checksum, &parser->checksum);
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
    
bool decode(FILE *fin)
{
    int num_read;
    char c;
    struct parser parser = { 0 };

    parser.state = START;
    while (parser.state != FINISH && parser.state != ERROR)
    {        
        switch (parser.state)
        {
        case DATA:
        {
            uint8_t byte;

            if (fprintf(stdout, "%08" PRIX32 "     ", parser.base_addr + parser.offset) <= 0)
            {
                parser.state = ERROR;
                break;
            }
            for (uint8_t i = 0; i < parser.byte_count; i++)
            {
                num_read = read_uint8(fin, &byte, &parser.checksum);
                if (num_read < 1 || fprintf(stdout, "%02" PRIX8, byte) <= 0)
                {
                    num_read = 0;
                    break;
                }
            }                     
            if (num_read == 1 && fprintf(stdout, "\n") > 0)
            {
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
                if (validate_checksum(fin, &parser)
                    && fprintf(stdout, "Starting address %08" PRIX32 "\n", parser.start_addr) > 0)
                {
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
                num_read = read_uint16(fin, &base_addr, &parser.checksum);
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
                num_read = read_uint32(fin, &parser.start_addr, &parser.checksum);
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
                num_read = checked_read(&c, sizeof c, 1, fin, false);
            }
            while (num_read == 1 && c != ':');
            
            if (num_read < 0)
            {
                parser.state = ERROR;
            }
            else if (read_uint8(fin, &parser.byte_count, &parser.checksum) != 1
                     || read_uint16(fin, &parser.offset, &parser.checksum) != 1
                     || read_uint8(fin, &parser.state, &parser.checksum) != 1)
            {
                parser.state = ERROR;
            }
            break;
        }
        case CHKSUM:
        {
            if (validate_checksum(fin, &parser))
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

    return parser.state == FINISH;
}
