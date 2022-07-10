#ifndef DATABASE_DEF
#define DATABASE_DEF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "dataDef.h"

#define MAX_ELEMENTS 2000
#define TEACHER_FILE "teacher.txt"
#define COURSE_FILE "course.txt"
#define REL_FILE "rel.txt"
#define PARAM_FILE "params.txt"

teacherStruct teacher[MAX_ELEMENTS];
short t_av[MAX_ELEMENTS];
short num_courses[MAX_ELEMENTS];
int t_count;

courseStruct course[MAX_ELEMENTS];
short c_av[MAX_ELEMENTS];
short assigned[MAX_ELEMENTS];
int c_count;

relation rel[3*MAX_ELEMENTS];
short r_av[3*MAX_ELEMENTS];

void load();

void add_course(char * name, char * res);

void del_course(char * name, char * res);

void add_teacher(char * name, char * res);

void del_teacher(char * name, char * res);

void print_info();

void save();

#endif