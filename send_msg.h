#ifndef _SEND_MSGH_
#define _SEND_MSGH_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define FIFO "/home/thanos/Desktop/fifo"

void send_msg(FILE **,int ,int , int* , int* , int* , int* );

#endif