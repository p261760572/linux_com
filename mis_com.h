#ifndef __MIS_COM__
#define __MIS_COM__

typedef struct{
	char field_name[64]; //�ֶ����ڱ�ϵͳ�ж���ı�ʶ��
	short len;      //���ݴ�ŵĳ���
	char *data_addr; //���ݴ�ŵ���ʼ��ַ	
	char type[10];    //��������  
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