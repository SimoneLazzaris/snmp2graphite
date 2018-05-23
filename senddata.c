#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include "configuration.h"
#include "senddata.h"


static void accumulate(char ** bufferptr, size_t * buffersize, char* dat) {
	char * buffer=*bufferptr;
	int ld=(buffer!=NULL)?strlen(buffer):0;
	int ls=strlen(dat);
	if (*buffersize<=ld+ls+1) {
		while (*buffersize<=ld+ls+1)
			*buffersize+=4096;
		buffer=(*bufferptr)=(char*)realloc(buffer,*buffersize);
		if (ld==0) buffer[0]=0;
		}
	strncat(buffer,dat,(*buffersize)-ls);
}

static void fullsend(int sock, char * buffer, int buffersize) {
	int n=0;
	while (n<buffersize) {
		n+=send(sock, buffer+n, buffersize-n,0);
	}
}

int senddata(configuration * pcfg, data2send *dat) {
	struct sockaddr_in address;
	int sock = 0;
	struct sockaddr_in serv_addr={0};
	char *buffer=NULL;
	size_t bufferlen=0;
	char buffer1[1024];
	time_t seconds;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr,"Socket creation error \n");
		return -1;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(pcfg->carbon_port);
     
	if(inet_pton(AF_INET, pcfg->carbon_host, &serv_addr.sin_addr)<=0) {
		fprintf(stderr,"Invalid address/ Address not supported [%s]\n",pcfg->carbon_host);
		return -1;
	}
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr,"Connection Failed \n");
		return -1;
	}
	seconds = time(NULL);

	while (dat!=NULL) {
		snprintf(buffer1,1024,"%s %u %u\n",dat->metric, dat->value, seconds);
		accumulate(&buffer,&bufferlen, buffer1);
		dat=dat->next;
	}
	accumulate(&buffer,&bufferlen, "\n");
	fullsend(sock, buffer, strlen(buffer));
	free(buffer);
	close(sock);
	return 0;
}
