#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>

int create_tcp_client(const char *ip,unsigned short port)
{
	int sockfd;
	struct sockaddr_in server_addr;
	struct hostent *tmp;
	int ret;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1){
		perror("socket");
		return -1;
	}
	
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family 	= AF_INET;
	server_addr.sin_port	= htons(port);
	tmp = gethostbyname(ip);
	if(tmp == NULL){
		perror("gethostbyname");
		return -1;
	}
	memcpy(&server_addr.sin_addr,(struct in_addr *)tmp->h_addr,4);
	
	ret = connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr));
	if(ret == -1){
		perror("connect");
		return -1;
	}
	
	printf("connect success!\n");
	
	return sockfd;
}

int main(int argc,char *argv[])
{
	int sockfd;
	int port;
	int ret;
	unsigned char buf[128];
	
	if(argc != 3){
		fprintf(stderr,"Usage Url[IP] port...\n");
		exit(1);
	}
	
	port = atoi(argv[2]);
	sockfd = create_tcp_client(argv[1],port);
	if(sockfd < 0){
		fprintf(stderr,"create tcp client failed!\n");
		exit(-1);
	}
	
	/* Data handle */
	printf("choose function:\n");
	getchar();
	
	buf[0] = 'U';
	buf[1] = ':';
	buf[2] = 0xe0;
	buf[3] = 0;
	
	ret = send(sockfd,buf,4,0);
	if(ret < 0){
		perror("send buf");
	}
	
	close(sockfd);
}
