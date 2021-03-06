CC= mpicc
CFLAGS=  -std=c99 -g -O0
OBJ = qmpi.o
ARFLAGS = rsv
ODIR = .


%.o: %.c
	$(CC) $(CFLAGS) -o $@  -c $^


libqmpi.a: $(OBJ)
	ar $(ARFLAGS) $@ $^
	ar -t $@

.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.a *~ core $(INCDIR)/*~
	rm -rf *.dSYM/ main
