CC= mpicc
CFLAGS= -I../.. -L../.. -std=c99 -fPIC -shared -lqmpi -O3

-include $(wildcard *.d)

./function_counter.so: ./function_counter.c
	$(CC) -MMD $(CFLAGS) -o $@ $<

clean:
	rm -f *.so *.d
