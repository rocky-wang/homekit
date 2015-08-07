#Design by Rocky For HomeKit Demo Project
#Date:	2015-5-25

TARGET		:=	egLock

#Define OBJS for TARGET
OBJS	:= locktest.o
OBJS	+= interface.o
OBJS	+= homeTcpServer.o

#Define COMPILER Flags
CFLAGS	+= -Wall -O2
CFLAGS	+= -D__DEBUG__
CFLAGS	+= -I../inc

LDFLAGS += 
#Define CROSS_COMPILER etc. info
CROSS_COMPILE	?= /opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-
CC				:= $(CROSS_COMPILE)gcc
LD				:= $(CROSS_COMPILE)ld
OBJCOPY			:= $(CROSS_COMPILE)objcopy

#Define WorkingSpace
WORKDIR	:= /home/rocky/work/rootfs/eg

#Rules
all:$(TARGET)
$(TARGET):$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
	cp -f $@ $(WORKDIR)/$@

%.o:%.c
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY : clean

clean:
	rm -f *.o $(TAGET)

