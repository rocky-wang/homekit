#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include "debug.h"

int create_tcp_server(int port)
{
	int sockfd;
	struct sockaddr_in self;
	int ret;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	
	memset(&self,0,sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port	= htons(port);
	self.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(sockfd,(struct sockaddr *)&self,sizeof(self));
	if(ret == -1){
		perror("bind");
		return -1;
	}
	
	listen(sockfd,5);
	PRINT("listen...\n");
	
	return sockfd;
}
