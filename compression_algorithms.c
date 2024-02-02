#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// compress.h //


enum CompressType
{
    TYPE_MOCK,
    TYPE_RLE,
    TYPE_RLE_DECODE,
    TYPE_FIB,
    TYPE_FIB_DECODE
};

struct CompressedData
{
    int n;
    void* compressed;
};


// compress.c //


struct CompressedData
mock_compress(int n, void *data) {
    struct CompressedData cd;

    if (n <= 0 || data == NULL) {
        cd.compressed = NULL;
        cd.n = 0;
        
        return cd;
    }


    cd.compressed = calloc(n, 1);
    memcpy(cd.compressed, data, n);

    cd.n = n;


    return cd;
}

struct CompressedData
rle_compress(int n, void *data) {
    struct CompressedData cd;

    if (n <= 0 || data == NULL) {
        cd.compressed = NULL;
        cd.n = 0;

        return cd;
    }

    char* src = (char*)data;


    char *dst;
    dst = calloc(2 * n, sizeof(*dst));

    int cur_i = 0;
    char cur_c = src[cur_i];

    int c = 1, // текущее число символов в серии
        cur_j = 0;
    for (cur_i = 1; cur_i < n; ++cur_i) {
        if (src[cur_i] == cur_c && c < 255) {
            ++c;
        } else {
            dst[cur_j++] = cur_c;
            dst[cur_j++] = c;
            cur_c = src[cur_i];
            c = 1;
        }
    }

    if (c > 0) {
        dst[cur_j++] = cur_c;
        dst[cur_j++] = c;
    }

    cd.compressed = calloc(cur_j, 1);
    cd.n = cur_j;
    memcpy(cd.compressed, dst, cur_j);

    free(dst);


    return cd;
}

struct CompressedData
rle_decode(int n, void *data) {
    struct CompressedData cd;
    cd.n = 0;

    if (n <= 0 || data == NULL) {
        cd.compressed = NULL;
        cd.n = 0;

        return cd;
    }

    unsigned char* src = (unsigned char*)data;

    int i;
    for (i = 0; i < n; i += 2) {
        cd.n += src[i+1];
    }


    char *dst;
    cd.compressed = calloc(cd.n, sizeof(*dst));

    int j, cur_j = 0;
    char *code = (char*)cd.compressed;
    for (i = 0; i < n; i += 2) {
        for (j = 0; j < src[i + 1]; ++j) {
            code[cur_j++] = src[i];
        }
    }

    return cd;
}

// Не работает!
struct CompressedData
fib_compress(int n, void *data) {
    const unsigned char fib_nums[] = {233,144,89,55,34,21,13,8,5,3,2,1};
    struct CompressedData cd;

    char* bytes = (char*)data;

    int fib_sz = sizeof(fib_nums);
    
    char *code;
    code = calloc(fib_sz * n, 1); // на каждый байт по-максимому 12 + 1 байт
    
    int code_i = 0;
    
    for (int i = 0; i < n; i ++)
    {
        unsigned char x = bytes[i];
        
        if (code_i != 0)
        {
            //организация разделителя
            
            //для теста поменять на ' '
            code[code_i ++] = '1';
        }
        
        if (x != 0)
        {
            //пробежаться и найти число фиб. которое меньше или равно нашему
            
            int fib_index = 0;
            
            while (fib_nums[fib_index] > x)
            {
                fib_index ++;
            }
            
            while (x)
            {
                code[code_i++] = '1';
                
                x -= fib_nums[fib_index++];

                while (fib_index < fib_sz && fib_nums[fib_index] > x)
                {
                    fib_index ++;
                    
                    code[code_i++] = '0';
                }
            }
        }
        else
        {
            code[code_i++] = '0';
        }
    }
    
    //TODO: выделить память точного размера и скопировать результат в неё

    cd.compressed = (void*)code;
    cd.n = code_i;

    return cd;
}

// Не работает!
struct CompressedData
fib_decode(int n, void *data) {
    const unsigned char fib_nums[] = {233, 144, 89, 55, 34, 21, 13, 8, 5, 3, 2, 1};


    struct CompressedData cd;
    char* bytes = (char*)data;
    int fib_sz = sizeof(fib_nums);
    
    char *code;
    code = calloc(n, 1);

    int code_i = 0;
    int start_i = 0;
    
    for (int cur_i = 1; cur_i < n; ++cur_i) {
        if (bytes[cur_i] == '1' && bytes[cur_i - 1] == '1' && cur_i > start_i) {
            unsigned char x = 0;
            int l = cur_i - start_i;
            
            for (int i = 0; i < l; ++i) {
                if (bytes[start_i + i] == '1') {
                    x += fib_nums[i];
                }
            }
            
            code[code_i++] = x;
            
            start_i = cur_i + 1;
        }
    }
    
    cd.compressed = (void*)code;
    cd.n = code_i;
    

    return cd;
}

struct CompressedData
compress(enum CompressType mode, int n, void *data) {
    switch (mode) {
        case TYPE_RLE:
            return rle_compress(n, data);
        case TYPE_RLE_DECODE:
            return rle_decode(n, data);
        case TYPE_FIB:
            return fib_compress(n, data);
        case TYPE_FIB_DECODE:
            return fib_decode(n, data);
        case TYPE_MOCK:
        default:
            return mock_compress(n, data);
    }
}


// compressor.c //


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
    
    
    if (cd.compressed != NULL) {
        free(cd.compressed);
    }
    free(data);


    return 0;
}


int
main() {
    file_compress("input.txt", "output.txt", TYPE_MOCK);
}