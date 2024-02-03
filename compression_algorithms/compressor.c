// compressor.c //


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/headers/compress.h"


int
file_compress(char* input, char* output, enum CompressType mode) {
    FILE *infile;
    infile = fopen(input, "rb");
    
    if (infile == NULL) {
        printf("%s does not exist.\n", input);
        return -1;
    }

    fseek(infile, 0, SEEK_END);
    int n = ftell(infile);
    rewind(infile);


    void *data = calloc(n, 1);
    fread(data, 1, n, infile);
    fclose(infile);


    struct CompressedData cd = compress(mode, n, data);

    FILE *outfile;
    outfile = fopen(output, "wb");

    fwrite(cd.compressed, 1, cd.n, outfile);
    fclose(outfile);


    if (cd.compressed != NULL) {
        free(cd.compressed);
    }
    free(data);


    return 0;
}

int main() {
    return file_compress("input.txt", "output.txt", TYPE_MOCK);
}

