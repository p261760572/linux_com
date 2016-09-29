CC=gcc
CFLAGS = -g -Wall -pthread -I.
SRC=mis_com.c mis_pack.c mis_unpack.c tlv.c com_test.c

OBJECT=$(SRC:*.c=*.o)
all: com_test
com_test: $(OBJECT)
	$(CC) $(OBJECT) -o $@ $(CFLAGS)

%o: %c
	$(CC) $(CFLAGS) -c -o $@ $<
