#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "interface.h"

int main()
{
    int fd ; 
    
    fd = initSerial();
    if (fd < 0){
        printf("init error!\n");
        return -1;
    }

    setSerial(fd);

    query_id(fd);

    getchar();

    unlocksys(fd);

    close(fd);

    return 0;
}
