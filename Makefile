CFLAGS = -Wall
LIBS = -lpcap

all: 
	gcc $(CFLAGS) -o cd00rd ./cd00rd.c $(LIBS)

clean: 	
	rm *.o
