#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>

#ifdef __DEBUG__

#define PRINT(arg...)	do{ fprintf(stdout,"\033[0;34m[Rocky prj]:");\
								fprintf(stdout,arg);\
								fprintf(stdout,"\033[0m");\
								fflush(stdout);} while(0)

#define ERROR(arg...)	do { fprintf(stderr,"\033[0;31m[Rocky prj]:");\
								fprintf(stderr,arg);\
								fprintf(stderr,"\033[0m");} while(0)

#else

#define PRINT(arg...)	do { }while(0)

#define ERROR(arg...)	do { fprintf(stderr,arg); }while(0)

#endif

#endif

