#ifndef __MIS_COM__
#define __MIS_COM__

typedef struct{
	char field_name[64]; //字段域在本系统中定义的标识符
	short len;      //数据存放的长度
	char *data_addr; //数据存放的起始地址	
	char type[10];    //数据类型  
}field_data;

typedef struct {
	char trans_type[64];
	char set[512];
} TRANS_SET;

typedef struct {
	char type[10];
	TRANS_SET Packet_Set[2];
} Packet_Def;


#endif