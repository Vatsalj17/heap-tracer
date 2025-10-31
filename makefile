CC=gcc
CFLAGS=-Wall -g
BINS=test.out shim.so shim-old.so loader.out

all: $(BINS)

%.so: %.c
	$(CC) $(CFLAGS) -shared -fPIC -o $@ $^ -ldl

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.so *.out
