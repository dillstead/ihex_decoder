#ifndef READ_H
#define READ_H

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>

int checked_read(void *ptr, size_t size, size_t nmemb, FILE *fin, bool report_eof);
int read_uint8(FILE *fin, uint8_t *value, uint8_t *checksum);
int read_uint16(FILE *fin,  uint16_t *value, uint8_t *checksum);
int read_uint32(FILE *fin, uint32_t *value, uint8_t *checksum);

#endif    
