CC=mpicc
CFLAGS=-std=c99 -W -Wall  -Werror
ODIR=./

LDFLAGS= -L../ -lqmpi -ldl -rdynamic

DEPS =libqmpi

_OBJ = main.o  
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	cd .. && make 
	$(CC) -g -o $@ $^ $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f ./*.o  
	rm -rf *.dSYM/ main

