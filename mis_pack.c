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
extern int get_field( char * field_name , char *packet_type, char * data);
int mis_pack(char *packet_type, char *buf)
{
		 char bodybuf[512],tmp[512],*p,*p1,next_set[1024],bd_set[1024], cnt;
		 int n,bodylen,i, len[2];
		 
		 for(i=0; trans_set[i].type && trans_set[i].type[0]; i++) {
		 		if(strcmp(trans_set[i].type, packet_type) == 0) {
		 				strncpy(bd_set, trans_set[i].Packet_Set[0].set, sizeof(bd_set));
		 				strncpy(next_set, trans_set[i].Packet_Set[1].set, sizeof(next_set));
		 				break;
		 		}	
		 }
		 
		 printf("type=%s, set=%s\n", packet_type, bd_set);
		 bodylen=0;
		 p1=bodybuf;
	//	 dcs_log(0,0,"<%s> body_set=[%s]",__FUNCTION__,bd_set);
		 for(p=strtok(bd_set, ",");p;p=strtok( NULL,","))
		 {
		 		 memset(tmp, 0, sizeof(tmp));
			 	 n=get_field(p,packet_type,tmp);
			 	 printf("tag=%s,len=%d,data=%s\n", p, n, tmp);
			 	 if( n >=0) 	 tmp[n]=0x00;
			 	 n=sprintf(p1, "%s%03d%s", p, n, tmp);
			 	 if( n > 0 ) 
			 	 {
				 	 	 bodylen +=n;
				 	 	 p1 +=n;
			 	 }
			 	 else if ( n == 0) continue;
			 	 else return -1;
			 	 
		 }
		 
		 len[0] = bodylen/256;
		 len[1] = bodylen%256;
		 memcpy(g_tag, packet_type, sizeof(g_tag));
		 sprintf(buf,"%c%c%c%c%c%06d%s%c", STX, len[0], len[1], PATH, TYPE, g_seq++, bodybuf,ETX);
		 cnt = 0x00;
		 for(i=1; i<bodylen+2+10; i++)
        cnt=cnt ^(unsigned char)buf[i];
     buf[bodylen+2+10]=cnt;
		 return bodylen+2+11;
}