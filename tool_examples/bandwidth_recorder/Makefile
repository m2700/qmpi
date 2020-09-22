CC= mpicc
CFLAGS= -I../.. -I.. -L../.. -std=c99 -fPIC -shared -lqmpi -O3

-include bandwidth_recorder.d

./bandwidth_recorder.so: ./bandwidth_recorder.c
	$(CC) -MMD $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f *.so *.d

.PHONY: fmt
fmt:
	clang-format -i *.c *.h
