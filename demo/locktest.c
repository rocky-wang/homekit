#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "interface.h"
#include "homeTcpServer.h"
#include "debug.h"

#define HOMESERVER_PORT		8888

void showMyHomeInfo()
{
	int i;

	for( i = 0; i < CABNUM; i++){
		if(myhome.cabinet[i].state == LOCKOFF){
			PRINT("the %s cabinet state is unlock!\n",myhome.cabinet[i].name);
		}
		else if(myhome.cabinet[i].state == LOCKUND){
			PRINT("the %s cabinet state is undefined!\n",myhome.cabinet[i].name);
		}
		else{
			PRINT("the %s cabinet state is locked!\n",myhome.cabinet[i].name);
		}
	}
}

static int getCabIdFromString(const unsigned char *value)
{
	int i = 0;
	
	while(value[i]){
		if(value[i] == ':'){
			return value[++i];
		}
		i++;
	}

	return -1;
}

int processHomeRequest(const unsigned char *request)
{
	int ret;
	int i;

	for(i = 0; i < 5; i++){
		PRINT("the buf[%d] is %x\n",i,request[i]);
	}
	
	switch(request[0]){
		case 'U':
			ret = getCabIdFromString(request);
			if(ret == -1){
				ERROR("Format error!\n");
				return -1;
			}
			return ret;
			break;
		case 'Q':
		default:
			return -1;
	}
}

int main()
{
    int serialFd ; 
	int ret;
	int listenfd;
	int newfd;
	unsigned char buf[128];
	unsigned char cabId;
	
    myhomeInit();
    serialFd = initSerial();
    if (serialFd < 0){
        printf("init error!\n");
        return -1;
    }
    setSerial(serialFd);

    ret = queryBoardId(serialFd);
	if(ret == -1){
		ERROR("query error...\n");
		return -1;
	}

	ret = queryAllCabInfo(serialFd);
	if(ret == -1){
		ERROR("query cabinfo failed!\n");
	}

	showMyHomeInfo();

	listenfd = create_tcp_server(HOMESERVER_PORT);
	if(listenfd == -1){
		ERROR("create tcp server failed!\n");
		exit(1);
	}
   	while(1){
		newfd = accept(listenfd,NULL,NULL);
		if(newfd < 0){
			perror("accept");
			break;
		}

		PRINT("Have a new connection...\n");
		while(1){
			ret = recv(newfd,buf,sizeof(buf),0);
			if(ret < 0){
				perror("recv failed!");
				break;
			}
			else if(ret == 0){
				PRINT("client close connection!\n");
				close(newfd);
				break;
			}
			else{
				ret = processHomeRequest(buf);
				if(ret == -1){
					ERROR("process failed!\n");
					break;
				}
				else{
					cabId = (unsigned char) (ret & 0xff);
					ret = unlocksys(serialFd,cabId);
					if(ret == 0){
						PRINT("unlock success!\n");
					}
				}
			}
		}
		close(newfd);
	}

    close(serialFd);
	close(listenfd);

    return 0;
}

