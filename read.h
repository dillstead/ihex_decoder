#ifndef READ_H
#define READ_H

#include <stdint.h>
#include <sys/types.h>

ssize_t read_uint8(int fd, uint8_t *value, uint8_t *checksum);
ssize_t read_uint16(int fd, uint16_t *value, uint8_t *checksum);
ssize_t read_uint32(int fd, uint32_t *value, uint8_t *checksum);

#endif    
