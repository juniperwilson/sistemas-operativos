ODIR = obj
IDIR = inc
OBJECTOS = main.o process.o memory.o wallet.o server.o
CC = gcc

SOchain: $(OBJECTOS)
	$(CC) $(addprefix $(ODIR)/,$(OBJECTOS)) -o bin/SOchain

main.o: main.c main.h
	$(CC) -c main.c

process.o: process.c process.h
	$(CC) -c process.c

memory.o: memory.c memory.h
	$(CC) -c memory.c

wallet.o: wallet.c wallet.h
	$(CC) -c wallet.c

server.o: server.c server.h
	$(CC) -c server.c


