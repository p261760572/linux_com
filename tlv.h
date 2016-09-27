#ifndef __TLV__
#define __TLV__

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define TIMEOUT 20000
#define TAG_LEN 2
#define LEN_LEN 3

struct tag_def_s {
    unsigned char *tag;
    unsigned int length;
    unsigned char *value;
    unsigned int pid;
};

typedef struct tag_def_s tag_def_t;

struct tag_array_s {
    tag_def_t def[1024];
    unsigned int length;
};

typedef struct tag_array_s tag_array_t;

void tag_array_destroy(tag_array_t *arr);

int tag_array_init(tag_array_t *arr);

void tag_array_dump(tag_array_t *arr);

int tag_array_append(tag_array_t *arr, unsigned char *tag, unsigned int length, unsigned char *value, unsigned int pid);

const tag_def_t *tag_array_get(const tag_array_t *arr, unsigned char *tag);

int tlv_parse(unsigned char *data, unsigned int data_len, tag_array_t *arr);

int tlv_tag_to_value(unsigned char *data, unsigned int data_len, const tag_array_t *arr, unsigned char buf[MAX_DATA_ARRAY_SIZE]);

#endif