CFLAGS= -O1 -fPIC
CFLAGS= -g -fPIC -Wall -Wextra -Werror
CFLAGS= -g -std=c99 -O3 -fPIC -msse -Wall -Wextra -pedantic -Werror
CC=gcc
LIBS=-lm

VPATH = ../common

all: lib

lib: block.so libsixtrack.so

block.o: block.c block.h tracking_beam.h track.h value.h particle.h
	$(CC) -I../common $(CFLAGS) -c -o $@ $<

block.so: block.o
	$(CC) -shared $(LIBS) -o $@ $^

libsixtrack.so : tracking_cl.o tracking_elements.o tracking_beam.o
	$(CC) -shared $(LIBS) -o $@ $^

tracking_cl.o: tracking_cl.c tracking_cl.h tracking_beam.h tracking_elements.h tracking_err.h
	$(CC) -I../common $(CFLAGS) -c -o $@ $<

tracking_elements.o: tracking_elements.c tracking_elements.h value.h block.h ../common/constants.h tracking_err.h
	$(CC) -I../common $(CFLAGS) -c -o $@ $<

tracking_beam.o: tracking_beam.c tracking_beam.h ../common/particle.h tracking_err.h
	$(CC) -I../common $(CFLAGS) -c -o $@ $<

testlib: test_sixtrack

test_sixtrack : test_sixtrack.c libsixtrack.so
	$(CC) -I../common $(CFLAGS) -lOpenCL -lsixtrack -L./ -Wl,-rpath=. -o $@ $<

clean:
	-rm -f block.so *.o
