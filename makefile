objects = main.o cfg.o err.o fhdrv3.o lfrec.o mjd.o obs.o

CC = g++
LDFLAGS = -lgzstream -lz
CFLAGS = -std=c++11

lofasmrec : $(objects)
		$(CC) $(CFLAGS) -o lofasmrec $(objects) $(LDFLAGS)
cfg.o : cfg.cpp cfg.h
		$(CC) $(CFLAGS) -c cfg.cpp
err.o : err.cpp err.h
		$(CC) $(CFLAGS) -c err.cpp
fhdrv3.o : fhdrv3.cpp fhdrv3.h
		$(CC) $(CFLAGS) -c fhdrv3.cpp
lfrec.o : lfrec.cpp lfrec.h fhdrv3.h obs.h
		$(CC) $(CFLAGS) -c lfrec.cpp
main.o : main.cpp fhdrv3.h lfrec.h cfg.h err.h
		$(CC) $(CFLAGS) -c main.cpp
mjd.o : mjd.cpp mjd.h
		$(CC) $(CFLAGS) -c mjd.cpp
obs.o: obs.cpp obs.h cfg.h
		$(CC) $(CFLAGS) -c obs.cpp
clean :
		rm lofasmrec $(objects)
install : lofasmrec
		cp lofasmrec /home/controller/bin/.
