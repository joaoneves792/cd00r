CFLAGS = -Wall
LIBS = -lpcap

all: 
	gcc $(CFLAGS) -o cd00rd ./cd00rd.c $(LIBS)
	gcc $(CFLAGS) -o lock ./lock.c

clean: 	
	rm *.o
