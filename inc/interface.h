#ifndef INTERFACE_H
#define INTERFACE_H

#define TTYS_NAME	"/dev/ttySAC3"

extern int initSerial(void);

extern int setSerial(int fd);

extern int query_id(int fd);

extern int unlocksys(int fd);

#endif

