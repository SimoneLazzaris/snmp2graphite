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

int senddata(configuration * pcfg, data2send *dat) {
	struct sockaddr_in address;
	int sock = 0;
	struct sockaddr_in serv_addr={0};

	char buffer[1024] = {0};
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
		snprintf(buffer,1024,"%s %u %u\n",dat->metric, dat->value, seconds);
		send(sock , buffer , strlen(buffer) , MSG_MORE );
		dat=dat->next;
	}	
	send(sock , buffer , 0,0);
	close(sock);
	return 0;
}
