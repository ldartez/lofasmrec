objects = main.o cfg.o err.o fhdr.o lfrec.o mjd.o obs.o

CC = g++
LDFLAGS = -lgzstream -lz
CFLAGS = -std=c++11

all : $(objects)
		$(CC) $(CFLAGS) -o lofasmrec $(objects) $(LDFLAGS)
cfg.o : cfg.cpp cfg.h
		$(CC) $(CFLAGS) -c cfg.cpp
err.o : err.cpp err.h
		$(CC) $(CFLAGS) -c err.cpp
fhdr.o : fhdr.cpp fhdr.h err.h mjd.h
		$(CC) $(CFLAGS) -c fhdr.cpp
lfrec.o : lfrec.cpp lfrec.h fhdr.h obs.h
		$(CC) $(CFLAGS) -c lfrec.cpp
main.o : main.cpp fhdr.h lfrec.h cfg.h err.h
		$(CC) $(CFLAGS) -c main.cpp
mjd.o : mjd.cpp mjd.h
		$(CC) $(CFLAGS) -c mjd.cpp
obs.o: obs.cpp obs.h cfg.h
		$(CC) $(CFLAGS) -c obs.cpp
clean :
		rm lofasmrec $(objects)
