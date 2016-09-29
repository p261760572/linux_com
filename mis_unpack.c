#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mis_com.h"
#include "tlv.h"

int mis_unpack(unsigned char *data)
{
		if(data[0] != 0x02) return -1;
		unsigned int data_len = data[0]*256+data[1];
		
		tag_array_t arr;
		tag_array_init(&arr);
		
		tlv_parse(data+11, data_len, &arr);
		const tag_def_t *tag_39 = tag_array_get(&arr, (unsigned char *)"39");
    if(tag_39 && tag_39->length > 0) {
        if(memcmp(tag_39->value, "00", 2) == 0) {
        		return 1;
        }
    }
    
    tag_array_destroy(&arr);
		return 0;
}
