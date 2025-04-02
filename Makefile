ODIR = obj
IDIR = inc
OBJECTOS = main.o process.o memory.o wallet.o server.o
CC = gcc

SOchain: main.o process.o memory.o wallet.o server.o
	gcc obj/main.o obj/process.o obj/memory.o obj/wallet.o obj/server.o -o bin/SOchain

main.o: src/main.c inc/main.h
	gcc -c src/main.c -o obj/main.o

process.o: src/process.c inc/process.h
	gcc -c src/process.c -o obj/process.o

memory.o: src/memory.c inc/memory.h
	gcc -c src/memory.c -o obj/memory.o

wallet.o: src/wallet.c inc/wallet.h
	gcc -c src/wallet.c -o obj/wallet.o

server.o: src/server.c inc/server.h
	gcc -c src/server.c -o obj/server.o



clean:
	rm -f $(ODIR)/*.o SOchain



