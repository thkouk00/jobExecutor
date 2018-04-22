#ifndef _USER_QUERYH_
#define _USER_QUERYH_

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
#include <time.h>
#include "worker.h"
#include "send_msg.h"
#include "list.h"
#include "map_file.h"
#include "trie.h"
#include "set_up_worker.h"
#include "maxcount.h"
#include "wc.h"
#include "search.h"

void user_query(int ,int *,int *, int *);

#endif