// compress.h //


enum CompressType
{
    TYPE_MOCK,
    TYPE_RLE,
    TYPE_RLE_DECODE,
    TYPE_FIB,
    TYPE_FIB_DECODE,
};

struct CompressedData
{
    int n;
    void* compressed;
};

struct CompressedData compress(enum CompressType mode, int n, void *data);
