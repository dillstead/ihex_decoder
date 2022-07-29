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
    FILE *fin = NULL;
    int res = EXIT_FAILURE;

    if (argc != 2)
    {
        fprintf(stdout, "Usage: ihex_decoder <input file>\n");
        return EXIT_FAILURE;
    }
    
    fin = fopen(argv[1], "r");
    if (fin == NULL)
    {
        fprintf(stderr, "Failed to open %s: %s\n", argv[1],
                strerror(errno));
        goto cleanup;
    }

    if (!decode(fin) || fflush(NULL) < 0)
    {
        goto cleanup;
    }
    res = EXIT_SUCCESS;

cleanup:
    if (fin != NULL)
    {
        fclose(fin);
    }
    return res;
}
