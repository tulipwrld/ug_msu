// compress.c //


#include "../headers/compress.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



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

struct CompressedData
fib_compress(int n, void *data) {
    const int fib_nums[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377};


    struct CompressedData cd;

    char* bytes = (char*)data;
    int fib_sz = sizeof(fib_nums) / sizeof(int);

    char *code;
    code = calloc(fib_sz * n, 1);

    int code_i = 0;
    for (int i = 0; i < n; ++i) {
        unsigned char x = bytes[i];
        
        if (x != 0) {
            int max_fib_index = 0;
            
            for (; max_fib_index < fib_sz; ++max_fib_index) {
                if (fib_nums[max_fib_index] > x) {
                    --max_fib_index;
                    break;
                }
            }

            int fib_index = max_fib_index,
                code_j = code_i + max_fib_index;
            
            while (x) {
                code[code_j--] = '1';
                x = x - fib_nums[fib_index--];
                while (code_j >= code_i && fib_nums[fib_index] > x) {
                    code[code_j--] = '0';
    
                    fib_index --;
                }
            }
            
            code_i += max_fib_index + 1;
        }
        else {
            code[code_i++] = '0';
        }
        
        code[code_i ++] = '1';
    }

    cd.compressed = (void*)code;
    cd.n = code_i;


    return cd;
}

struct CompressedData
fib_decode(int n, void *data) {
    const int fib_nums[] = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377};


    struct CompressedData cd;

    char* bytes = (char*)data;
    int fib_sz = sizeof(fib_nums) / sizeof(int);


    char *code;
    code = calloc(n, 1);
    
    int code_i = 0, start_i = 0;
    
    for (int cur_i = 1; cur_i < n; ++cur_i) {
        if (bytes[cur_i] == '1' && bytes[cur_i - 1] == '1' && cur_i > start_i) {
            unsigned char x = 0;
            int l = cur_i - start_i;

            for (int i = 0; i < l; ++i) {
                if (bytes[start_i+i] == '1') {
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
