#ifndef DATA_DEF
#define DATA_DEF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>

#define TEACHER_NAME_MAXLEN 50
#define COURSE_NAME_MAXLEN 50

typedef struct{
	char tid[TEACHER_NAME_MAXLEN];
} teacherStruct;

typedef struct{
	char cid[COURSE_NAME_MAXLEN];
} courseStruct;

typedef struct{
	char tid[TEACHER_NAME_MAXLEN];
	char cid[COURSE_NAME_MAXLEN];
} relation;


#define MSG_VAL_LEN  100

// For the client queue message
#define CLIENT_Q_NAME_LEN 100

// For the server queue message
#define MSG_TYPE_LEN 100

typedef struct{
char client_q[CLIENT_Q_NAME_LEN];
char msg_val[MSG_VAL_LEN];
} client_msg_t;

typedef struct{
char msg_type[MSG_TYPE_LEN];
char msg_val[MSG_VAL_LEN];
} server_msg_t;

#define SERVER_QUEUE_NAME   "/server_msgq"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_CLT_MSG_SIZE sizeof(client_msg_t)
#define MAX_SRV_MSG_SIZE sizeof(server_msg_t)
#define CLT_MSG_BUFFER_SIZE (MAX_CLT_MSG_SIZE * MAX_MESSAGES)
#define SRV_MSG_BUFFER_SIZE (MAX_SRV_MSG_SIZE * MAX_MESSAGES)
#define MAX_COURSES_PER_TEACHER 3

#define MIN_COURSES 10
#define MAX_COURSES 15
#define MIN_TEACHERS 5
#define MAX_TEACHERS 10

int min_courses, max_courses, min_teachers, max_teachers;

#endif