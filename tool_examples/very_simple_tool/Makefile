CC= mpicc
CFLAGS= -I../.. -L../.. -std=c99 -fPIC -shared -lqmpi
OBJ = very_simple_tool
ODIR = ./


./very_simple_tool.so: ./very_simple_tool.c
	$(CC) $(CFLAGS) -o $@ $<
#	cd ./mpiP_refactored && make clean && make shared && cd ..

.PHONY: clean
clean:
	rm -f $(ODIR)/*.so
#	cd mpiP_refactored && make clean && cd ..

.PHONY: fmt
fmt:
	clang-format -i *.c *.h
