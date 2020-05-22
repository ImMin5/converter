
ssu_convert : ssu_convert.o ssu_header.o ssu_file.o ssu_extern.o ssu_makefile.o
	gcc -o ssu_convert ssu_convert.o  ssu_header.o ssu_file.o ssu_extern.o ssu_makefile.o
ssu_convert.o : ssu_convert.c
	gcc -c -Wall -W -o ssu_convert.o ssu_convert.c
ssu_header.o : ssu_header.c
	gcc -c -Wall -W -o ssu_header.o ssu_header.c
ssu_file.o : ssu_file.c
	gcc -c -Wall -W -o ssu_file.o ssu_file.c
ssu_extern.o : ssu_extern.c
	gcc -c -Wall -W -o ssu_extern.o ssu_extern.c
ssu_makefile.o : ssu_makefile.c
	gcc -c -Wall -W -o ssu_makefile.o ssu_makefile.c

gdb : ssu_convert.c 
	gcc -Wall -W -g ssu_convert.c ssu_header.c ssu_file.c ssu_extern.c ssu_makefile.c -o test

run :
	./ssu_convert q2.java
