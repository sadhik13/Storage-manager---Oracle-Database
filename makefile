CC=gcc
CFLAGS=-I.
REMOVE=rm

dberror.o: dberror.c
	$(CC) -o dberror.o -c dberror.c

storage_mgr.o: storage_mgr.c
	$(CC)  -o storage_mgr.o -c storage_mgr.c

test_assign1_1.o: test_assign1_1.c
	$(CC)  -o test_assign1_1.o -c test_assign1_1.c

sm: test_assign1_1.o storage_mgr.o dberror.o
		$(CC)  -o sm test_assign1_1.o storage_mgr.o dberror.o -I.
clean:
	$(REMOVE) sm *.o
