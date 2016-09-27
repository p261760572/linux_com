#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mis_com.h"

#define STX 	0x02
#define TYPE 	0x01
#define PATH	0x03
#define ETX		0x03

extern field_data fld_data[512];
extern int fld_num, gs_offset;
extern char gs_buffer[64*1024];

extern Packet_Def trans_set[100];

extern int g_seq;
extern char g_tag[2];

int mis_pack(char *packet_type, char *buf)
{
		 char bodybuf[512],tmp[512],*p,*p1,next_set[1024],bd_set[1024];
		 int n,bodylen,i;
		 
		 for(i=0; trans_set[i].type && trans_set[i].type[0]; i++) {
		 		if(strcmp(trans_set[i].type, packet_type) == 0) {
		 				strncpy(bd_set, trans_set[i].Packet_Set[0].set, sizeof(bd_set));
		 				strncpy(next_set, trans_set[i].Packet_Set[1].set, sizeof(next_set));
		 				break;
		 		}	
		 }
		 
		 bodylen=0;
		 p1=bodybuf;
	//	 dcs_log(0,0,"<%s> body_set=[%s]",__FUNCTION__,bd_set);
		 for(p=strtok(bd_set, ",");p;p=strtok( NULL,","))
		 {
			 	 n=get_field(p,packet_type,tmp);
			 	 if( n >=0) 	 tmp[n]=0x00;
			 	 n=sprintf(p1, "%s%03d%s", p, n, tmp);
			 	 if( n > 0 ) 
			 	 {
				 	 	 bodylen +=n;
				 	 	 p1 +=n;
			 	 }
			 	 else if ( n == 0) continue;
			 	 else return -1;
	//		 	 dcs_log(0,0,"<%s>order[%s]=[%s]",__FUNCTION__,p,tmp);
			 	 
		 }
		 
		 memcpy(g_tag, packet_type, sizeof(g_tag));
		 sprintf(buf,"%c%d%c%c%06d%s%c", STX, n, PATH, TYPE, g_seq++, bodybuf,ETX);
		 
		 return bodylen+2+11;
}