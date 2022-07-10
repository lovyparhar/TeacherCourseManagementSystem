all: srv clt close

srv: dataDef.h server.c database.h database.c
	gcc -o srv server.c database.c -lrt -lpthread -D_REENTRANT

clt: dataDef.h client.c
	gcc -o clt client.c -lrt

close: dataDef.h close.c
	gcc -o close close.c -lrt

clean: srv clt
	rm -f srv clt close

dclean: teacher.txt rel.txt course.txt params.txt
	rm teacher.txt rel.txt course.txt params.txt