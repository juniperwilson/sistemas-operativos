ODIR = obj
IDIR = inc
OBJECTOS = main.o process.o memory.o wallet.o server.o
CC = gcc

SOchain: $(OBJECTOS)
	$(CC) $(addprefix $(ODIR)/,$(OBJECTOS)) -o bin/SOchain

%.o: src/%.c $($@)
	$(CC) -I $(IDIR) -o $(ODIR)/$@ -c $<

clean:
	rm -f $(ODIR)/*.o SOchain



