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
    printf("Commands\n\n");
    printf("1. ADD_COURSE C1, C2, C3 ...\n");
    printf("2. DEL_COURSE C1, C2, C3 ...\n");
    printf("3. ADD_TEACHER T1, T2, T3 ...\n");
    printf("4. DEL_TEACHER T1, T2, T3 ...\n");
    printf("\n");
    printf("To exit, press Ctrl + C\n");
    printf("\n");

    char client_queue_name [64];
    mqd_t qd_srv, qd_client;   // Server and Client Msg queue descriptors
    int num = 1;

    if ((qd_srv = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror ("Client MsgQ: mq_open (qd_srv)");
        exit (1);
    }

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_CLT_MSG_SIZE;
    attr.mq_curmsgs = 0;

    client_msg_t out_msg;
    // create the client queue for receiving messages from the server
    sprintf (out_msg.client_q, "/clientQ-%d", getpid());

    if ((qd_client = mq_open(out_msg.client_q, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS,
                           &attr)) == -1) {
        perror ("Client msgq: mq_open (qd_client)");
        exit (1);
    }

    char cmd[MSG_VAL_LEN];

    while (1) {

        fgets(cmd, MSG_VAL_LEN, stdin);
        cmd[strlen(cmd)-1] = '\0';
        strcpy(out_msg.msg_val, cmd);   

        // send message to my_msgq_rx queue
        // int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
        if (mq_send (qd_srv, (char *) &out_msg, MAX_SRV_MSG_SIZE, 0) == -1) {
            perror ("Client MsgQ: Not able to send message to the queue /server_msgq");
            continue;
        }

        printf ("%d: Client MsgQ: Message sent successfully\n", num);

        sleep(0.5);  // sleep for 0.5 seconds
        num++;

        int count;
        if (mq_receive(qd_client,(char *) &count, MAX_CLT_MSG_SIZE, NULL) == -1) {
            perror ("Client MsgQ: mq_receive from server");
            exit (1);
        }

        printf("Number of messages to be collected from server %d\n", count);

		server_msg_t in_msg;

        for(int i = 0; i < count; i++)
        {
            // ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
            if (mq_receive(qd_client,(char *) &in_msg, MAX_CLT_MSG_SIZE, NULL) == -1) {
                perror ("Client MsgQ: mq_receive from server");
                exit (1);
            }
        
            printf("Client MsgQ: Msg received from the server = %s\n", in_msg.msg_val);
        }

        printf("\n");
    }

    printf ("Client MsgQ: bye\n");

    exit (0);
}
