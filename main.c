#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "decode.h"

int main(int argc, char **argv)
{
    int hex_in = -1;
    int res = EXIT_FAILURE;
                   
    if (argc != 2)
    {
        fprintf(stdout, "Usage: ihex_decoder <input file>\n");
        return EXIT_FAILURE;
    }
    
    hex_in = open(argv[1], O_RDONLY);
    if (hex_in < 0)
    {
        fprintf(stderr, "Failed to open %s: %s\n", argv[1],
                strerror(errno));
        goto cleanup;
    }
    res = decode(hex_in, 1) ? EXIT_SUCCESS : EXIT_FAILURE;

cleanup:
    close(hex_in);
    return res;
}
