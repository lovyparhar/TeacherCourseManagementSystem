// Lab11 MsgQ Client process
// Compilation of this file
// gcc -o clt client.c -lrt


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>

#include "dataDef.h"

int main (int argc, char **argv)
{
    mq_unlink(SERVER_QUEUE_NAME);
}
