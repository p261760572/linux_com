#include "tlv.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static unsigned char bhex[] = "0123456789ABCDEF";

static unsigned int hexb[] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 10, 11, 12, 13, 14, 15, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void bin_to_hex(const unsigned char *bin, unsigned char *hex, int size) {
    int i;
    for(i = 0; i < size; i++) {
        *hex++ = bhex[(bin[i] >> 4) & 0xf];
        *hex++ = bhex[bin[i] & 0xf];
    }
}

void hex_to_bin(const unsigned char *hex, unsigned char *bin, int size) {
    int i;
    for(i = 0; i < size; i+=2) {
        *bin++ = (hexb[(int)hex[i]]<<4) | hexb[(int)hex[i+1]];
    }
}

void bcd_to_asc(const unsigned char *bcd, unsigned char *asc, int size) {
	int i;
	for(i = 0; i < size; i++) {
		*asc++ = ((bcd[i] >> 4) & 0xf) + 0x30;
		*asc++ = (bcd[i] & 0xf) + 0x30;
	}
}

void dump(const char *format, ...) {
    char buf[MAX_DATA_ARRAY_SIZE*2];
    va_list args;

    va_start(args, format);
    memset(buf, 0, sizeof(buf));
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    fprintf(stdout, "%s\n", buf);
}

void dump_hex(unsigned char *data, int data_len, const char *format, ...) {
    unsigned char buf[MAX_DATA_ARRAY_SIZE*2];

    va_list args;

    va_start(args, format);
    memset(buf, 0, sizeof(buf));
    vsnprintf((char *)buf, sizeof(buf), format, args);
    va_end(args);
    fprintf(stderr, "%s", buf);

    memset(buf, 0, sizeof(buf));
    bin_to_hex(data, buf, data_len);
    fprintf(stderr, "%s\n", buf);
}



void tag_array_destroy(tag_array_t *arr) {
    unsigned int i;
    if(arr != NULL) {
        for(i = 0; i < arr->length; i++) {
            free(arr->def[i].value);
            free(arr->def[i].tag);
        }
    }
}

int tag_array_init(tag_array_t *arr) {
    if(arr != NULL) {
        memset(arr, 0, sizeof(*arr));
    }

    return 0;
}

void tag_array_dump(tag_array_t *arr) {
    unsigned int i;
    if(arr != NULL) {
        dump("\ntag_array_dump");
        for(i = 0; i < arr->length; i++) {
            dump_hex(arr->def[i].value, arr->def[i].length, "[%x][%x]=", arr->def[i].tag, arr->def[i].length);
        }
    }
}

int tag_array_append(tag_array_t *arr, unsigned char *tag, unsigned int length, unsigned char *value, unsigned int pid) {

    if(arr->length >= ARRAY_SIZE(arr->def)) {
        return -1;
    }
		
		arr->def[arr->length].tag = (unsigned char *)calloc(1, 2);
		memcpy(arr->def[arr->length].tag, tag, 2);
    arr->def[arr->length].length = length;
    arr->def[arr->length].value = (unsigned char *)calloc(1, length);
    memcpy(arr->def[arr->length].value, value, length);
    arr->def[arr->length].pid = pid;

    arr->length++;

    return 0;
}


const tag_def_t *tag_array_get(const tag_array_t *arr, unsigned char *tag) {
    unsigned int i;
    if(arr != NULL) {
#if 0
        for(i = 0; i < arr->length; i++) {
            if(arr->def[i].tag == tag) {
                return &arr->def[i];
            }
        }
#endif

        for(i = arr->length; i > 0; i--) {
            if(arr->def[i-1].tag == tag) {
                return &arr->def[i-1];
            }
        }
    }

    return NULL;
}


int tlv_parse(unsigned char *data, unsigned int data_len, tag_array_t *arr) {
    unsigned int offset = 0,  l, tag_construct;
    unsigned char *v, t[TAG_LEN], tmp_len[3];

    while(offset < data_len) {
    		tag_construct = 0;
    		memcpy(t, data+offset, TAG_LEN);
        offset += TAG_LEN;

				memcpy(tmp_len, data+offset, LEN_LEN);
        l = atoi((const char *)tmp_len);
        
        offset += LEN_LEN;
        v = data + offset;
        tag_array_append(arr, t, l, v, -1);
        if(tag_construct) {
            tlv_parse(v, l, arr);
        }

        offset += l;
    }

    return 0;
}


/*
int tlv_tag_to_value(unsigned char *data, unsigned int data_len, const tag_array_t *arr, unsigned char buf[MAX_DATA_ARRAY_SIZE]) {
    unsigned int offset = 0, t, l;
    //unsigned char *v;
    const tag_def_t *def;

    unsigned int buf_offset = 0;

    while(offset < data_len) {
        int tag_construct = 0;
        t = data[offset];

        if(tag_is_constructed(t)) {
            tag_construct = 1;
        }

        if(tag_is_expand(t)) {
            offset ++;
            t = (t << 8 | data[offset]);
        }
        offset ++;

        l = data[offset];
        if(len_is_expand(l)) {
            unsigned temp_l = (l & LEN_BYTES_MASK);
            l = 0;
            while(temp_l -- > 0) {
                offset ++;
                l = (l << 8 | data[offset]);
            }
        }
        offset ++;

        def = tag_array_get(arr, t);
        if(def == NULL || def->length != l) {
            dump("tlv_tag_to_value %x %d %d", t, def == NULL ? 0 : def->length, l);
            return -1;
        }

        if(buf_offset + def->length > MAX_DATA_ARRAY_SIZE) {
            dump("tlv_tag_to_value data too long");
            return -1;
        }

        memcpy(buf+buf_offset, def->value, def->length);
        buf_offset += def->length;
    }

    return buf_offset;
} */
