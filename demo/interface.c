#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "debug.h"
#include "interface.h"

int initSerial()
{
    int fd;

    fd = open(TTYS_NAME,O_RDWR | O_NOCTTY | O_NDELAY);
    if(fd == -1){
        ERROR("open ttySAC3 error is %s\n",strerror(errno));
        return -errno;
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

int query_id(int fd)
{
    unsigned char buf[] = {0x80,0x01,0x00,0x99,0x18};
    unsigned char tmp;
    int i;

    for( i = 0; i < sizeof(buf)/sizeof(buf[0]); i++){
        write(fd,buf+i,1);
    }

    sleep(1);

    for( i = 0; i < 5; i++){
        read(fd,&tmp,1);
        PRINT("the tmp is %x\n",tmp);
    }

    return 0;
}

int unlocksys(int fd)
{
    unsigned char buf[] = {0x8A,0x03,0x01,0x11,0x0};
    unsigned char check;
    int i;

    /* Fill up checksum */
    check = buf[0]^ buf[1]; 
    check = check ^ buf[2];
    check = check ^ buf[3];
    buf[4] = check;

    PRINT("the check sum is %x\n",buf[4]);

    for( i = 0; i < sizeof(buf)/sizeof(buf[0]); i++){
           write(fd,buf+i,1);
    }

    PRINT("lock...\n");

    return 0;
}


