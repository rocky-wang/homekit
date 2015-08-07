#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "debug.h"
#include "lockcmd.h"
#include "interface.h"

HomeInfo_t myhome; 

int initSerial()
{
    int fd;

    fd = open(TTYS_NAME,O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1){
        ERROR("open ttySAC3 error is %s\n",strerror(errno));
        return errno;
    }

    fcntl(fd, F_SETFL, 0);

    PRINT("init serial success!\n");
    return fd;
}

int setSerial(int fd)
{
    struct termios options;

    tcgetattr(fd, &options);

    /* Set the baud rates to 9600...*/
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    /* Enable the receiver and set local mode... */
    options.c_cflag |= (CLOCAL | CREAD);

    /* No parity (8N1) */
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    /* disable hardware flow control */
    options.c_cflag &= ~CRTSCTS;

    /* Raw Input */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* Raw Output */
    options.c_oflag &= ~OPOST;

   /* Make changes now without waiting for data to complete */
   if (tcsetattr(fd, TCSANOW, &options) != 0){
        ERROR("Serial set attr failed:%s!\n",strerror(errno));
        return errno;
   }

   return 0;
}

/*
 * fill xor value to the last data
*/
int checkSum(unsigned char *data,int num)
{
	unsigned char check;
	int i;
	
	if(data == NULL){
		ERROR("input data is NULL!\n");
		return EFAULT;
	}
	if(num <= 2){
		ERROR("input data need tow space at leaste!\n");
		return EINVAL;
	}

	check = data[0] ^ data[1]; 
	for( i = 2; i < num; i++){
		check = check ^ data[i];
	}

	return check;
}

int queryBoardId(int fd)
{
    unsigned char buf[5];
    unsigned char tmp[5];
    int len = 0;
	int ret;

	buf[0] = MLOCK_CMDHEAD;
	buf[1] = MLCOK_CMDQBOARDID1;
	buf[2] = 0x00;
	buf[3] = MLOCK_CMDQBOARDID2;
	buf[4] = checkSum(buf,4);

	ret = write(fd,buf,5);
	if(ret != 5){
		perror("write query id data");
		return -1;
	}

	while(len < 5){
		ret = read(fd,tmp + len,5);
		if(ret == -1){
			perror("read query id data");
			return -1;
		}
		PRINT("the read query id ret is %d\n",ret);
		len += ret;
	}

	if( tmp[4] != checkSum(tmp,4) ){
		ERROR("Query BoardId checksum error!\n");
		return -1;
	}

	myhome.BoardId = tmp[2];
	PRINT("Query Board Id is %x\n",myhome.BoardId);
	
	return 0;
}

int queryAllCabInfo(int fd)
{
	unsigned char buf[5];
	unsigned char tmp[8];
	int ret;
	int len = 0;
	int i;

	buf[0] = MLOCK_CMDHEAD;
	buf[1] = myhome.BoardId;
	buf[2] = MLOCK_CMDQALLCAB;
	buf[3] = MLOCK_CMDQCABEND;
	buf[4] = checkSum(buf,4);

	ret = write(fd,buf,5);
	if(ret != 5){
		perror("write query all cab data");
		return -1;
	}

	while(len < 8){
		ret = read(fd,tmp + len,8);
		if(ret == -1){
			perror("read query id data");
			return -1;
		}
		PRINT("the read query all cab,ret is %d\n",ret);
		len += ret;
	}

	if(tmp[7] != checkSum(tmp,7)){
		ERROR("query all cab checksum failed!\n");
		return -1;
	}

	for( i = 0; i < CABNUM; i++){
		if( !(tmp[5] & (0x1<<i)) ){
			myhome.cabinet[i].state = LOCKON;
		}
	}

	return 0;
}

int unlocksys(int fd,unsigned char id)
{
    unsigned char buf[5];
    int ret;
	int i;

	buf[0] = MLOCK_LOCKHEAD;
	buf[1] = myhome.BoardId;
	for(i = 0; i < CABNUM; i++){
		if(id == myhome.cabinet[i].id){
			buf[2] = myhome.cabinet[i].cmdData;
			break;
		}
	}
	if( i == CABNUM){
		ERROR("Not Found cabinet id!\n");
		return -1;
	}
	buf[3] = MLOCK_LOCKSTART;
	buf[4] = checkSum(buf,4);

    ret = write(fd,buf,5);
	if(ret != 5){
		perror("write unlock cab data");
		return -1;
	}

    return 0;
}

int myhomeInit()
{
	int i;

	myhome.BoardId = 0;
	
	for(i = 0; i < CABNUM; i++){
		myhome.cabinet[i].id = CABIDHEAD+i;
		myhome.cabinet[i].cmdData = i+0x1;
		myhome.cabinet[i].state = LOCKUND;
		snprintf(myhome.cabinet[i].name,16,"Home-%d",i+1);
	}

	return 0;
}

