proc.o: proc.c proc.h
	gcc -g -Wall -c proc.c 
pile_addr.o: pile_addr.c pile_addr.h
	gcc -g -Wall -c pile_addr.c
interface: interface.c interface.h pile_addr.o proc.o
	gcc `pkg-config --libs --cflags gtk+-2.0` -g -Wall interface.c proc.o pile_addr.o -o interface