#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define RCOMD "KE"
#define QCOMD "KF"
#define JMJCONF "./jmj.conf"
#define MKFILE "./mek20160920"

int open_socket(char *ip, unsigned int port);

int CommandProc(char *inBuf,int inLen,char *outBuf, int outlen) {
		FILE *fp;
		int rbyte=0, wbyte=0;
		char jmj_ip[20], jmj_port[6], buf[1024];
		
		bzero(jmj_ip, sizeof(jmj_ip));
		bzero(jmj_port, sizeof(jmj_port));
		bzero(outBuf, outlen);
		
		fp = fopen(JMJCONF, "r+")	;
		while(!feof(fp)) {
				bzero(buf, sizeof(buf));
				if(NULL == fgets(buf, sizeof(buf), fp)) continue;
				sscanf(buf, "%[^:] : %[0-9]", jmj_ip, jmj_port);
				break;
		}
		if(!jmj_ip[0] || !jmj_port[0]) return -1;
		
		int sockfd = open_socket(jmj_ip, atol(jmj_port));
    
    wbyte = write(sockfd, inBuf, inLen);
    
    if((rbyte = read(sockfd, outBuf, outlen)) <=0)
    {
        close(sockfd);
        return -1;
    }
    close(sockfd);
    fclose(fp);
    return rbyte;
}
/*  sek加密tmk转换成tek加密tmk
 *  flag ：1 16H， 2 32H， 3 48H
 */
int sek_conv_tek(const char *sek_index, const char *tek_index, const char *tmk_sek, char *tmk_tek, char flag) {
		int len, n;
		char Buffer[2048];
		
		bzero(Buffer, sizeof(Buffer));
		
		len = 2;
		n = strlen(RCOMD);
		memcpy(Buffer+len, RCOMD, n);
		len += n;
		
		n = strlen(sek_index);
		memcpy(Buffer+len, sek_index, n);
		len += n;
		
		n = strlen(tek_index);
		memcpy(Buffer+len, tek_index, n);
		len += n;
		
		Buffer[len++] = '0';    // sek to tek
		switch(flag) {
				case '1':
							Buffer[len++] = 'X'; 
							n = 16;
							break;
				case '2':
							Buffer[len++] = 'Y'; 
							n = 32;
							break;
				case '3':
							Buffer[len++] = 'Z'; 
							n = 48;
		}
		memcpy(Buffer+len, tmk_sek, n);
		len += n;
		Buffer[0] = (len-2)/256;
		Buffer[1] = (len-2)%256;
		
		if(0>CommandProc(Buffer,len,Buffer,len)) {
        return -1;
    }
    if(memcmp(Buffer+2,"00",2)!=0) {
        return -1;
    }
    memcpy(tmk_tek,Buffer+4, n);
    
		return len;
}

/*  tek加密tmk转换成sek加密tmk
 *  flag ：1 16H， 2 32H， 3 48H
 */
int tek_conv_sek(const char *tek_index, const char *sek_index, const char *tmk_tek, char *tmk_sek, char flag) {
		int len, n;
		char Buffer[2048];
		
		bzero(Buffer, sizeof(Buffer));
		
		len = 2;
		n = strlen(RCOMD);
		memcpy(Buffer+len, RCOMD, n);
		len += n;
		
		n = strlen(tek_index);
		memcpy(Buffer+len, tek_index, n);
		len += n;
		
		n = strlen(sek_index);
		memcpy(Buffer+len, sek_index, n);
		len += n;
		
		Buffer[len++] = '0';    // sek to tek
		switch(flag) {
				case '1':
							Buffer[len++] = 'X'; 
							n = 16;
							break;
				case '2':
							Buffer[len++] = 'Y'; 
							n = 32;
							break;
				case '3':
							Buffer[len++] = 'Z'; 
							n = 48;
		}
		memcpy(Buffer+len, tmk_tek, n);
		len += n;
		Buffer[0] = (len-2)/256;
		Buffer[1] = (len-2)%256;
		
		if(0>CommandProc(Buffer,len,Buffer,len)) {
        return -1;
    }
    if(memcmp(Buffer+2,"00",2)!=0) {
        return -1;
    }
    memcpy(tmk_sek,Buffer+4, n);
    
		return len;
}

// 打开TCP Socket连接   
int open_socket(char *ip, unsigned int port) {  
    int sockfd = 0;  
    struct sockaddr_in their_addr = {0};  
    
    memset(&their_addr, 0, sizeof(their_addr));  
    their_addr.sin_family = AF_INET;  
    their_addr.sin_port = htons(port);  
    their_addr.sin_addr.s_addr = inet_addr(ip);
    
    sockfd=socket(PF_INET, SOCK_STREAM, 0);  
    if(sockfd < 0)  
    {  
        return -1;  
    }  
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) < 0)  
    {  
        return -1;  
    }  
    return sockfd;  
}  

int term_key_file_conv(char *in_file) {
		FILE *ifp;
			
}