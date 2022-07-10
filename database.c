#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "dataDef.h"
#include "database.h"

int is_non_neg_integer(char * s) {
	for(int i = 0; i < strlen(s); i++) {
		if('0' <= s[i] && s[i] <= '9') {
			continue;
		}
		else {
			return 0;
		}
	}
	return 1;
}

void trimmedString(char * dest, char * src) {
	int n = strlen(src);
	int l = 0;
	while(src[l] == ' ') l++;
	for(int i = 0; i <= n-l; i++) {
		dest[i] = src[i+l];
	}
	l = n-l-1;
	while(dest[l] == ' ') dest[l--] = '\0';
}

void assignCourse(int slot) {
	int minTeach = -1;
	// Try to assign the course to the teacher with minimum courses to offer
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(t_av[i] == 0) continue;
		if(minTeach == -1) {
			minTeach = i;
		}
		else if(num_courses[minTeach] > num_courses[i]) {
			minTeach = i;
		}
	}

	if(minTeach == -1) {
		return;
	}

	if(num_courses[minTeach] >= MAX_COURSES_PER_TEACHER) {
		return;
	}

	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
		if(r_av[i] == 1) continue;
		strcpy(rel[i].tid, teacher[minTeach].tid);
		strcpy(rel[i].cid, course[slot].cid);
		r_av[i] = 1;
		assigned[slot] = 1;
		num_courses[minTeach] += 1;
		break;
	}
}

void assignTeacher(int slot) {
	// Try to assign a teacher MAX_COURSES_PER_TEACHER courses
	int ass = 0;
	int courseInd[MAX_COURSES_PER_TEACHER];

	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(c_av[i] == 0) {
			continue;
		}
		
		if(assigned[i]) {
			continue;
		}

		courseInd[ass] = i;
		ass++;

		if(ass == MAX_COURSES_PER_TEACHER) break;
	}

	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {

		if(ass == 0) break;
		if(r_av[i] == 1) continue;

		ass--;
		strcpy(rel[i].tid, teacher[slot].tid);
		strcpy(rel[i].cid, course[courseInd[ass]].cid);
		r_av[i] = 1;
		assigned[courseInd[ass]] = 1;
		num_courses[slot] += 1;
	}
}

void add_course(char * name, char * res) {

	if(c_count >= max_courses) {
		sprintf(res, "Can't add more courses, limit reached");
		return;
	}

	// Check if the id is already present or not
	char idToAdd[MSG_VAL_LEN];
	trimmedString(idToAdd, name);
	int found = 0;
	int slot = -1;

	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(c_av[i] == 0) {
			slot = i;
			continue;
		}
		if(strcmp(course[i].cid, idToAdd) == 0) {
			found = 1;
			break;
		} 
	}

	if(found) {
		sprintf(res, "Course already present with cid = %s", idToAdd);
		return;
	}
	else if(slot == -1) {
		sprintf(res, "Slots are filled for courses");
		return;
	}
	else {
		// If not already present, add the course to the course list
		strcpy(course[slot].cid, idToAdd);
		c_av[slot] = 1;
		assigned[slot] = 0;
	}

	c_count += 1;
	sprintf(res, "Added successfully the course with cid = %s", idToAdd);


	// Check if you can assign a teacher to this course

	assignCourse(slot);
	
	return;
}

void del_course(char * name, char * res) {

	if(c_count <= min_courses) {
		sprintf(res, "Can't delete more courses, limit reached");
		return;
	}

	char idToDel[MSG_VAL_LEN];
	trimmedString(idToDel, name);
	int deleted = 0;

	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(c_av[i] == 0) continue;
		if(strcmp(course[i].cid, idToDel) == 0) {
			c_av[i] = 0;
			deleted = 1;
			break;
		} 
	}

	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
		if(r_av[i] == 0) continue;
		if(strcmp(rel[i].cid, idToDel) == 0) {
			r_av[i] = 0;
			for(int j = 0; j < MAX_ELEMENTS; j++) {
				if(t_av[j] == 0) continue;
				if(strcmp(teacher[j].tid, rel[i].tid) == 0) {
					num_courses[j] -= 1;
				}
			}
		}
	}

	if(deleted) {
		c_count -= 1;
		sprintf(res, "Delete executed successfully for cid = %s", idToDel);
	}
	else {
		sprintf(res, "No course found with cid = %s", idToDel);
	}
}






void add_teacher(char * name, char * res) {

	if(t_count >= max_teachers) {
		sprintf(res, "Can't add more teachers, limit reached");
		return;
	}

	// Check if the id is already present or not
	char idToAdd[MSG_VAL_LEN];
	trimmedString(idToAdd, name);
	int found = 0;
	int slot = -1;

	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(t_av[i] == 0) {
			slot = i;
			continue;
		}
		if(strcmp(teacher[i].tid, idToAdd) == 0) {
			found = 1;
			break;
		} 
	}

	if(found) {
		sprintf(res, "Teacher already present with tid = %s", idToAdd);
		return;
	}
	else if(slot == -1) {
		sprintf(res, "Slots are filled for teachers");
		return;
	}
	else {
		// If not already present, add the teacher to the teacher list
		strcpy(teacher[slot].tid, idToAdd);
		t_av[slot] = 1;
		num_courses[slot] = 0;
	}

	t_count += 1;
	sprintf(res, "Added successfully the teacher with tid = %s", idToAdd);


	// Check if you can assign courses to this teacher

	assignTeacher(slot);
	
	return;
}





void del_teacher(char * name, char * res) {

	if(t_count <= min_teachers) {
		sprintf(res, "Can't delete more teachers, limit reached");
		return;
	}

	char idToDel[MSG_VAL_LEN];
	trimmedString(idToDel, name);
	int deleted = 0;

	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(t_av[i] == 0) continue;
		if(strcmp(teacher[i].tid, idToDel) == 0) {
			t_av[i] = 0;
			deleted = 1;
			break;
		} 
	}

	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
		if(r_av[i] == 0) continue;
		if(strcmp(rel[i].tid, idToDel) == 0) {
			r_av[i] = 0;
			for(int j = 0; j < MAX_ELEMENTS; j++) {
				if(c_av[j] == 0) continue;
				if(strcmp(course[j].cid, rel[i].cid) == 0) {
					assigned[j] = 0;
				}
			}
		}
	}

	for(int j = 0; j < MAX_ELEMENTS; j++) {
		if(c_av[j] == 0) continue;
		if(assigned[j] == 0) {
			assignCourse(j);
		}
	}

	if(deleted) {
		t_count -= 1;
		sprintf(res, "Delete executed successfully for tid = %s", idToDel);
	}
	else {
		sprintf(res, "No teacher found with tid = %s", idToDel);
	}
	
	return;
}



void print_info() {
	printf("\n------- SUMMARY -------\n\n");

	printf("THE TEACHERS\n");
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(t_av[i] == 0) continue;
		printf("tid = %s, num_courses = %d\n", teacher[i].tid, num_courses[i]);
	}
	printf("\n");

	printf("THE COURSES\n");
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(c_av[i] == 0) continue;
		printf("cid = %s, assigned = %d\n", course[i].cid, assigned[i]);
	}
	printf("\n");

	printf("TEACHER AND COURSE ASSIGNMENTS\n");
	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
		if(r_av[i] == 0) continue;
		printf("tid = %s, cid = %s\n", rel[i].tid, rel[i].cid);
	}

	printf("\n-----------------------\n");
}


void load() {

	FILE * tf = fopen(TEACHER_FILE, "r"), * cf = fopen(COURSE_FILE, "r"), * rf = fopen(REL_FILE, "r");

	int tcount = 0;
	if(tf) {
		char info[MSG_VAL_LEN];
		fgets(info, MSG_VAL_LEN, tf);
        info[strlen(info)-1] = '\0';
        
        tcount = atoi(info);

		for(int i = 0; i < tcount; i++) {
			fgets(info, MSG_VAL_LEN, tf);
        	info[strlen(info)-1] = '\0';

	        char * token;
	        token = strtok(info, ",");
	        strcpy(teacher[i].tid, token);
	        token = strtok(NULL, ",");
	        num_courses[i] = atoi(token);
	        t_av[i] = 1;
		}

		fclose(tf);
	}


	int ccount = 0;
	if(cf) {
		char info[MSG_VAL_LEN];
		fgets(info, MSG_VAL_LEN, cf);
        info[strlen(info)-1] = '\0';
        
        ccount = atoi(info);

		for(int i = 0; i < ccount; i++) {
			fgets(info, MSG_VAL_LEN, cf);
        	info[strlen(info)-1] = '\0';

	        char * token;
	        token = strtok(info, ",");
	        strcpy(course[i].cid, token);
	        token = strtok(NULL, ",");
	        assigned[i] = atoi(token);
	        c_av[i] = 1;
		}

		fclose(cf);
	}


	int rcount = 0;
	if(rf) {
		char info[MSG_VAL_LEN];
		fgets(info, MSG_VAL_LEN, rf);
        info[strlen(info)-1] = '\0';
        
        rcount = atoi(info);

		for(int i = 0; i < rcount; i++) {
			fgets(info, MSG_VAL_LEN, rf);
        	info[strlen(info)-1] = '\0';

	        char * token;
	        token = strtok(info, ",");
	        strcpy(rel[i].tid, token);
	        token = strtok(NULL, ",");
	        strcpy(rel[i].cid, token);
	        r_av[i] = 1;
		}

		fclose(rf);
	}

	t_count = tcount;
	c_count = ccount;
	
}

void save() {

	FILE * tf = fopen(TEACHER_FILE, "w"), * cf = fopen(COURSE_FILE, "w"), * rf = fopen(REL_FILE, "w");

	int tcount = 0;
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(t_av[i] == 1) tcount++;
	}

	if(tf) {
		fprintf(tf, "%d\n", tcount);
		for(int i = 0; i < MAX_ELEMENTS; i++) {
			if(t_av[i] == 0) continue;
			fprintf(tf, "%s,%d\n", teacher[i].tid, num_courses[i]);
		}
	}


	int ccount = 0;
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(c_av[i] == 1) ccount++;
	}
	
	if(cf) {
		fprintf(cf, "%d\n", ccount);
		for(int i = 0; i < MAX_ELEMENTS; i++) {
			if(c_av[i] == 0) continue;
			fprintf(cf, "%s,%d\n", course[i].cid, assigned[i]);
		}
	}


	int rcount = 0;
	for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
		if(r_av[i] == 1) rcount++;
	}

	if(rf) {
		fprintf(rf, "%d\n", rcount);
		for(int i = 0; i < 3*MAX_ELEMENTS; i++) {
			if(r_av[i] == 0) continue;
			fprintf(rf, "%s,%s\n", rel[i].tid, rel[i].cid);
		}
	}

	fclose(tf);
	fclose(cf);
	fclose(rf);
}