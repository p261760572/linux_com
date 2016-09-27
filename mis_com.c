#include <stdio.h>
#include <string.h>
#include "mis_com.h"


field_data fld_data[512];
int fld_num=0, gs_offset=0;
char gs_buffer[64*1024];

Packet_Def trans_set[100];

int g_seq = 1;
char g_tag[2] = "";

void rtrim( char * str)
{
	int i=0;
	if ( str == NULL ) return ;
	i=strlen(str);
	for(;i>0;i--)
	   if( str[i-1]!=0x20) break;
	if(i >=0)	str[i]=0x00;
	return ;
}

int get_field( char * field_name , char *packet_type, char * data)
{
	int i;
	for(i=0; i< fld_num ;i++)
	{
		if( strcmp(field_name,fld_data[i].field_name)==0 && strcmp(packet_type,fld_data[i].type)==0)
		{
			memcpy(data,fld_data[i].data_addr,fld_data[i].len);
			break;
	  }
	}
	if( i == fld_num ) return -1;
	return fld_data[i].len;
}


int get_command_next( char *packet_type, char * next)
{
		int i;
		for(i=0;trans_set[i].type && trans_set[i].type[0];i++)
		{
				if( strcmp(packet_type,trans_set[i].type)==0)
				{
					strcpy(next,trans_set[i].Packet_Set[1].set);
					break;
			  }
		}
		if( !trans_set[i].type[0] ) {
			fprintf(stderr, "(at %s:%d)非法指令[%s]", __FILE__, __LINE__, packet_type);
			return -1;
		}
		return 1;
}

int add_field(int data_len,const char *data,char *field_name,char *type)
{
	 if ( fld_num >=1000 )
	 {	 
	 	  return -1;
	 }
   if(gs_offset + data_len  <64*1024 )
   {	
   	 strcpy(fld_data[fld_num].field_name,field_name);
   	 strcpy(fld_data[fld_num].type,type);
     fld_data[fld_num].len = data_len;     
     fld_data[fld_num].data_addr = gs_buffer+gs_offset;
   	 memcpy(gs_buffer+gs_offset,data,data_len);
   	 gs_offset += data_len; 
   	 fld_num++;
   }
   else return -1;
   return 1;
}

int load_trans_field( char * filename )
{
	//打开配置文件
	FILE *fp;
	char buf[1024],*p,field_name[64],data[512],type[10], head[10];
	fp=fopen(filename,"r");
	if( fp == NULL ) return 0;
	while(!feof(fp))
	{
			if(NULL==fgets(buf,sizeof(buf),fp)) break;
			if( buf[strlen(buf)-1]==0x0D ||buf[strlen(buf)-1]==0x0A)
				buf[strlen(buf)-1]=0x00;
			sscanf(buf, "%[a-z]%[0-9].%[0-9]%*[ ]=%*[ ]%s", head, type, field_name, data);
			if( strncmp(head,"field",5)!=0) continue;
			rtrim(data);
			add_field(strlen(data),data,field_name,type);
		//	dcs_log(0,0,"<%s> %s=[%s], type[%s]",__FUNCTION__,field_name,data,type);
	}
	fclose(fp);
	return 1;
}

int load_trans_set(char * filename )
{
	FILE *fp;
	char buf[2048],*p, trans_type[10], type[3], head[20], set[512];
	int i=0;
	fp=fopen(filename,"r");
	if( fp == NULL ) return 0;
		
	bzero(trans_type, sizeof(trans_type));
	bzero(type, sizeof(type));
	bzero(head, sizeof(head));
	bzero(set, sizeof(set));
	
	memset(&trans_set,0,sizeof(trans_set));
	while(!feof(fp))
	{
			if(NULL==fgets(buf,sizeof(buf),fp)) break;
			if( buf[strlen(buf)-1]==0x0D ||buf[strlen(buf)-1]==0x0A)
				buf[strlen(buf)-1]=0x00;
			sscanf(buf, "%[^.].%[a-z]%[0-9]%*[ ]=%*[ ]%s", head, trans_set[i].Packet_Set[0].trans_type, 
							trans_set[i].type, trans_set[i].Packet_Set[0].set);
			
			if( !head[0] || strcmp(head,"trans")!=0) continue;
			rtrim(trans_set[i].Packet_Set[0].set);
		//	dcs_log(0,0,"<%s> [%d]trans_type=[%s],set=[%s]",__FUNCTION__,i,
					//		trans_set[i].Packet_Set[0].trans_type,trans_set[i].Packet_Set[0].set);
			
			if(NULL==fgets(buf,sizeof(buf),fp)) continue;
			if( buf[strlen(buf)-1]==0x0D ||buf[strlen(buf)-1]==0x0A)
				buf[strlen(buf)-1]=0x00;
				
			sscanf(buf, "%[^.].%[a-z]%[0-9]%*s=%*s%s", head, trans_set[i].Packet_Set[1].trans_type, 
							trans_set[i].type, trans_set[i].Packet_Set[1].set);
			if( !head[0] || strcmp(head,"trans")!=0)
			{
				//	dcs_log(0, 0, "配置错误");
					return -1;
			}
			rtrim(trans_set[i].Packet_Set[1].set);
		//	dcs_log(0,0,"<%s> [%d]trans_type=[%s],set=[%s]",__FUNCTION__,i,
		//					trans_set[i].Packet_Set[1].trans_type,trans_set[i].Packet_Set[1].set);
			
			i++;
	}	
	fclose(fp);
	return 1;
}
