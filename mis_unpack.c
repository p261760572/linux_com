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
		const tag_def_t *tag_39 = tag_array_get(&arr, "39");
    if(tag_39 && tag_39->length > 0) {
        
    }
    
    tag_array_destroy(&arr);
		return 1;
}
