CC=gcc
LD=gcc
RMF=rm -f
LIBEXT=.dylib

test: tqueue_test.o libtqueue.dylib
	$(LD) -o $@ $< -L. -ltqueue

libtqueue.dylib: tqueue.o
	$(LD) -shared -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $^


clean:
	$(RMF) *.o *$(LIBEXT) test
