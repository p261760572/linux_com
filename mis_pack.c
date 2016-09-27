#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mis_com.h"

#define STX 	0x02
#define TYPE 	0x01
#define PATH	0x03
#define ETX		0x03

int mis_pack(char *body_set, char *next, char *next_node, char *packet_type, char *buf)
{
		 char headbuf[512],bodybuf[512],tmp[512],*p,*p1,next_set[1024],bd_set[1024];
		 int n,bodylen;
		// ≥‰ µheadbuf
		 p1=headbuf;
		 headlen=0;
		 strcpy(bd_set,body_set);
		 strcpy(next_set,next);
		 
		 bodylen=0;
		 p1=bodybuf;
		 dcs_log(0,0,"<%s> body_set=[%s]",__FUNCTION__,bd_set);
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
			 	 dcs_log(0,0,"<%s>order[%s]=[%s]",__FUNCTION__,p,tmp);
			 	 
		 }
		 
		 memcpy(g_tag, "%s", packet_type);
		 sprintf(buf,"%c%d%c%c%06d%s%c", STX, n, PATH, TYPE, seq++, bodybuf,ETX);
		 
		 return bodylen+2+11;
}