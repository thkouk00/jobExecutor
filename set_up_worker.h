#ifndef _SET_UP_WORKER_H_
#define _SET_UP_WORKER_H_
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
#include "send_msg.h"
#include "list.h"
#include "map_file.h"
#include "trie.h"

void set_up_worker(listNode ** ,char *);


#endif