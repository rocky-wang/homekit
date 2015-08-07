#ifndef INTERFACE_H
#define INTERFACE_H

#define CABNUM	8

enum lockstate{LOCKOFF = 0x10,LOCKON = 0x20,LOCKUND = 0x30};

typedef struct {
	char name[16];
	unsigned char id;		// for show client info
	unsigned char cmdData;	// send board command
	unsigned char state;
}RoomInfo_t;

typedef struct{
	unsigned char BoardId;
	RoomInfo_t cabinet[8];
}HomeInfo_t;

extern HomeInfo_t myhome; 


#define TTYS_NAME	"/dev/ttySAC3"

extern int initSerial(void);

extern int setSerial(int fd);

extern int queryBoardId(int fd);

extern int queryAllCabInfo(int fd);

extern int myhomeInit();

extern int unlocksys(int fd,unsigned char id);

#endif

