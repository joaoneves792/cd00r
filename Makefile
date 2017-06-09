CFLAGS = -Wall
LIBS = -lpcap
CC= gcc

all: cd00rd lock 
#	$(CC) $(CFLAGS) -o cd00rd ./cd00rd.c $(LIBS)
#	$(CC) $(CFLAGS) -o lock ./lock.c

cd00rd: cd00rd.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

lock: lock.o
	$(CC) -o $@ $^ $(CFLAGS)

clean: 	
	-rm *.o
	-rm cd00rd
	-rm lock
