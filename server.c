// Lab11 MsgQ Server process
// Compilation of this file
// gcc -o srv server.c -lrt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "dataDef.h"
#include "database.h"

sem_t bin_sem;

void *thread_fn(void *arg);
void set_server_parameters();

static client_msg_t client_msg;

void save_and_exit() {
    save();
    exit(0);
}

int main (int argc, char **argv)
{
    signal(SIGINT, save_and_exit);

    load();
    mqd_t qd_srv, qd_client;   // Server and Client Msg queue descriptors
    int num = 1;

    printf("Setting up the server...\n");
    set_server_parameters();

    printf ("Server MsgQ: Welcome!!!\n");

    // Creating the parallel thread ------------
    int stat;
    pthread_t thread_id;
    stat = pthread_create(&thread_id, NULL, thread_fn, NULL);

    if(stat != 0){ 
        perror("Main: Error: Thread 1 creation failed\n");
        exit(EXIT_FAILURE);
    }
    // ------------------------------------------


    // Set up the semaphore ----------  
  
    if(sem_init(&bin_sem, 0, 1) != 0) {
        printf("Semaphore1 creation failure");
        exit(1);
    }  

    // -------------------------------


    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_SRV_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_srv = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS,
                           &attr)) == -1) {
        perror ("Server MsgQ: mq_open (qd_srv)");
        exit (1);
    }

    char cmd[MSG_VAL_LEN];
    client_msg_t in_msg;
	int val_client;
    while (1) {
        // ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);
        if (mq_receive(qd_srv,(char *) &in_msg, MAX_SRV_MSG_SIZE, NULL) == -1) {
            perror ("Server msgq: mq_receive");
            exit (1);
        }

        strcpy(cmd, in_msg.msg_val);
		val_client = atoi(in_msg.msg_val);
        printf ("%d: Server MsgQ: message received.\n", num);
        printf("Command received is : %s\n", cmd);
        num++;

        server_msg_t out_msg; 
        strcpy(out_msg.msg_type, "Server msg"); 

        // Process the command here ------------------
        int l = 0;
        char pcmd[15][MSG_VAL_LEN];
        char * token;
        token = strtok(cmd, " ");
        while(token != NULL) {
            strcpy(pcmd[l++], token);
            token = strtok(NULL, ",");
        }


        // Open the client queue using the client queue name received
        if ((qd_client = mq_open(in_msg.client_q, O_WRONLY)) == 1) {
            perror ("Server MsgQ: Not able to open the client queue");
            continue;
        }


        sem_wait(&bin_sem);
        if(l == 1) {
            int count = 1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }
            strcpy(out_msg.msg_val, "Invalid Command");
            if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }
        }

        else if(strcmp("ADD_COURSE", pcmd[0]) == 0) {
            int count = l-1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }

            for(int i = 1; i < l; i++) {
                add_course(pcmd[i], out_msg.msg_val);
                if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                    perror ("Server MsgQ: Not able to send message to the client queue");
                    continue;
                }  
            }
        }

        else if(strcmp("DEL_COURSE", pcmd[0]) == 0) {
            int count = l-1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }

            for(int i = 1; i < l; i++) {
                del_course(pcmd[i], out_msg.msg_val);
                if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                    perror ("Server MsgQ: Not able to send message to the client queue");
                    continue;
                }
            }
        }

        else if(strcmp("ADD_TEACHER", pcmd[0]) == 0) {
            int count = l-1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }

            for(int i = 1; i < l; i++) {
                add_teacher(pcmd[i], out_msg.msg_val);
                if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                    perror ("Server MsgQ: Not able to send message to the client queue");
                    continue;
                }
            }
        }

        else if(strcmp("DEL_TEACHER", pcmd[0]) == 0) {
            int count = l-1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }

            for(int i = 1; i < l; i++) {
                del_teacher(pcmd[i], out_msg.msg_val);
                if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                    perror ("Server MsgQ: Not able to send message to the client queue");
                    continue;
                }
            }
        }

        else {
            int count = 1;
            if (mq_send(qd_client, (char *) &count, sizeof(count), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }

            strcpy(out_msg.msg_val, "Invalid Command"); 
            if (mq_send(qd_client, (char *) &out_msg, sizeof(out_msg), 0) == -1) {
                perror ("Server MsgQ: Not able to send message to the client queue");
                continue;
            }
        }
        sem_post(&bin_sem);

        // -------------------------------------------
            
    } // end of while(1) 
    
}  // end of main()


void * thread_fn(void *arg){

    while(1) {
        sleep(10);
        sem_wait(&bin_sem);
        print_info();
        // save();
        sem_post(&bin_sem);
    }
} 

void set_server_parameters() {
    FILE * pf = fopen(PARAM_FILE, "r");
    if(pf == NULL) {
        printf("Set the minimum number of courses (between %d and %d): ", MIN_COURSES, MAX_COURSES);
        scanf("%d", &min_courses);

        if(min_courses < MIN_COURSES || min_courses > MAX_COURSES) {
            printf("Invalid value for minimum courses\n");
            min_courses = MIN_COURSES;
            printf("Value set to %d\n", MIN_COURSES);
        }

        printf("Set the maximum number of courses (between %d and %d): ", MIN_COURSES, MAX_COURSES);
        scanf("%d", &max_courses);

        if(max_courses < MIN_COURSES || max_courses > MAX_COURSES || max_courses < min_courses) {
            printf("Invalid value for maximum courses\n");
            max_courses = MAX_COURSES;
            printf("Value set to %d\n", MAX_COURSES);
        }

        printf("Set the minimum number of teachers (between %d and %d): ", MIN_TEACHERS, MAX_TEACHERS);
        scanf("%d", &min_teachers);

        if(min_teachers < MIN_TEACHERS || min_teachers > MAX_TEACHERS) {
            printf("Invalid value for minimum teachers\n");
            min_teachers = MIN_TEACHERS;
            printf("Value set to %d\n", MIN_TEACHERS);
        }

        printf("Set the maximum number of teachers (between %d and %d): ", MIN_TEACHERS, MAX_TEACHERS);
        scanf("%d", &max_teachers);

        if(max_teachers < MIN_TEACHERS || max_teachers > MAX_TEACHERS || max_teachers < min_teachers) {
            printf("Invalid value for maximum teachers\n");
            max_teachers = MAX_TEACHERS;
            printf("Value set to %d\n", MAX_TEACHERS);
        }

        FILE * pf = fopen(PARAM_FILE, "w");
        if(pf) {
            fprintf(pf, "%d,%d,%d,%d\n", min_courses, max_courses, min_teachers, max_teachers);
            fclose(pf);
        }
    }
    else {
        fscanf(pf, "%d,%d,%d,%d\n", &min_courses, &max_courses, &min_teachers, &max_teachers);
        fclose(pf);
    }


    printf("\nServer parameters set successfully\n");
}