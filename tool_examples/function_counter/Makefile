CC= mpicc
CFLAGS= -I../.. -I.. -L../.. -std=c99 -fPIC -shared -lqmpi -O3

-include function_counter.d

./function_counter.so: ./function_counter.c
	$(CC) -MMD $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f *.so *.d

.PHONY: fmt
fmt:
	clang-format -i *.c *.h
