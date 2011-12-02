include ../maindir.mk
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64
FLAGSSO=-m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m64 -fPIC -DUNIX -c
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib $(MAINDIR)/lib
FLAGSSO=-m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m32 -fPIC -DUNIX -c
endif
LIB=CAENVME hscaenet caenhvwrapper
LIBS=CAENVME hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=.

#########################################################

TestAssembly.so: ../../lib/libTestAssembly.so

../../lib/libTestAssembly.so: TestAssembly.o
	mkdir -p ../lib
	g++ $(FLAGSSO),libTestAssembly.so -o $(MAINDIR)/lib/libTestAssembly.so.1.0.1 TestAssembly.o
	ln -fs $(MAINDIR)/lib/libTestAssembly.so.1.0.1 $(MAINDIR)/lib/libTestAssembly.so

TestAssembly.o: TestAssembly.cpp TestAssembly.h
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) TestAssembly.cpp -o TestAssembly.o

test: test.o TestAssembly.o 
	g++ $(FLAGS) test.o TestAssembly.o -o test

test.o: test.cpp TestAssembly.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) test.cpp -o test.o

setaddress: setaddress.o TestAssembly.o
	g++ $(FLAGS) setaddress.o TestAssembly.o -o  $(MAINDIR)/bin/setaddress

setaddress.o: setaddress.cpp TestAssembly.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) setaddress.cpp -o setaddress.o

onvol: onvol.o TestAssembly.o
	g++ $(FLAGS) onvol.o TestAssembly.o -o  $(MAINDIR)/bin/onvol

onvol.o: onvol.cpp TestAssembly.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) onvol.cpp -o onvol.o

offvol: offvol.o TestAssembly.o
	g++ $(FLAGS) offvol.o TestAssembly.o -o  $(MAINDIR)/bin/offvol

offvol.o: offvol.cpp TestAssembly.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) offvol.cpp -o offvol.o

flushSerial: flushSerial.cpp flushSerial.o
	g++ $(FLAGS) flushSerial.o -o  $(MAINDIR)/bin/flushSerial

flushSerial.o: flushSerial.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) flushSerial.cpp -o flushSerial.o

clean:
	rm -fr *.o *~ *.so test
	rm -fr ../../bin/setaddress
	rm -fr ../../bin/onvol
	rm -fr ../../bin/offvol

cleanall: clean
	rm -fr ../../lib/libTestAssembly.so.1.0.1 ../../lib/libTestAssembly.so

