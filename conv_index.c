#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define CONF_VAR 4
#define RCOMD "KE"
#define QCOMD "KF"
#define JMJCONF "./jmj.conf"

static char g_mkfile[] = "", g_jmj_ip[20] = "", g_jmj_port[6] = "", g_sek_index[6] = "", g_tek_index[6] = "";

int open_socket(char *ip, unsigned int port);

int CommandProc(char *inBuf,int inLen,char *outBuf, int outlen) {
		int rbyte=0, wbyte=0;
		
		int sockfd = open_socket(jmj_ip, atol(jmj_port));
    wbyte = write(sockfd, inBuf, inLen);
    
    if((rbyte = read(sockfd, outBuf, outlen)) <=0)
    {
        close(sockfd);
        return -1;
    }
    close(sockfd);
    return rbyte;
}
/*  sek加密tmk转换成tek加密tmk
 *  flag ：1 16H， 2 32H， 3 48H
 */
int sek_conv_tek(const char *sek_index, const char *tek_index, const char *tmk_sek, char *tmk_tek, int flag) {
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
				case 1:
							Buffer[len++] = 'X'; 
							n = 16;
							break;
				case 2:
							Buffer[len++] = 'Y'; 
							n = 32;
							break;
				case 3:
							Buffer[len++] = 'Z'; 
							n = 48;
							break;
				default:
							Buffer[len++] = 'Y'; 
							n = 32;
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
    
		return n;
}

/*  tek加密tmk转换成sek加密tmk
 *  flag ：1 16H， 2 32H， 3 48H
 */
int tek_conv_sek(const char *tek_index, const char *sek_index, const char *tmk_tek, char *tmk_sek, int flag) {
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
				case 1:
							Buffer[len++] = 'X'; 
							n = 16;
							break;
				case 2:
							Buffer[len++] = 'Y'; 
							n = 32;
							break;
				case 3:
							Buffer[len++] = 'Z'; 
							n = 48;
							break;
				default:
							Buffer[len++] = 'Y'; 
							n = 32;
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
    
		return n;
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

int load_config() {
		int get=0;
		FILE *fp = fopen(JMJCONF, "r+")	;
		while(!feof(fp)) {
				bzero(buf, sizeof(buf));
				if(get == CONF_VAR) break;
				if(NULL == fgets(buf, sizeof(buf), fp)) continue;
				if((sscanf(buf, "server=%[^:] : %[0-9]", g_jmj_ip, g_jmj_port) > 0) || 
						(sscanf(buf, "srcfile=%s", g_mkfile) > 0) ||
						(sscanf(buf, "sek_index=%s", g_sek_index) > 0) ||
						(sscanf(buf, "tek_index=%s", g_tek_index) > 0) ) get++;
		}
		fprintf(stdout, "server=%s:%s\nfile=%s\nsek_index=%s\ntek_index=%s\n", 
							g_jmj_ip, g_jmj_port, g_mkfile, g_sek_index, g_tek_index);
		return ((get == CONF_VAR)? 1: -1);
}

int term_key_file_conv() {
		FILE *ifp, *ofp;
		char line_buf[1024], split[5][60], rewrite[1024], overfile[40];
		int read = 0, writed = 0;
		bzero(&split, sizeof(split));
		
		snprintf(overfile, sizeof(overfile), "%s_d", g_mkfile);
		if(load_config() < 0) return -1;
		
		ifp = fopen(g_mkfile, "r+");
		if(!ifp) return -1;
		ofp = fopen(overfile, "w+");
		if(!ofp) return -1;
		while(!feof(ifp)) {
				bzero(line_buf, sizeof(line_buf));
				bzero(rewrite, sizeof(rewrite));
				if(NULL == fgets(line_buf, sizeof(line_buf), ifp)) continue;
				if(sscanf(line_buf, "%[^,],%[^,],%[^,],%[^,],%[0-9a-zA-Z]", 
									split[0], split[1], split[2], split[3], split[4]) > 1)
					read++;
				*tmk_size = sek_conv_tek(g_sek_index, g_tek_index, split[2], split[2], 2);
				sprintf(rewrite, "%s,%s,%s,%s,%s\n", split[0], split[1], split[2], split[3], split[4]);
				if(fwrite(rewrite, strlen(rewrite), 1, ofp) > 0) writed ++;
		}
		
		fclose(ifp);
		fclose(ofp);
		
		fprintf(stdout, "read from=%d, write to=%d\n", read, writed);
		return 1;
}