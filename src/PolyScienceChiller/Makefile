include ../maindir.mk
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64
FLAGSSO=-m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC
CFLAGS=-m64 -g -Wall -O2 -fPIC -c
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib
FLAGSSO=-m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC 
CFLAGS=-m32 -g -Wall -O2 -fPIC -c
endif
LIB=CAENVME hscaenet caenhvwrapper
LIBS=CAENVME hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=.
DEPENDENCES=PolyScienceChiller.cpp PolyScienceChiller.h

Polyscience.so: ../../lib/libPolyScienceChiller.so $(DEPENDENCES)

../../lib/libPolyScienceChiller.so: PolyScienceChiller.o
	mkdir -p ../lib
	g++ $(FLAGSSO),libPolyScienceChiller.so PolyScienceChiller.o -o $(MAINDIR)/lib/libPolyScienceChiller.so.1.0.1
	ln -fs $(MAINDIR)/lib/libPolyScienceChiller.so.1.0.1 $(MAINDIR)/lib/libPolyScienceChiller.so

PolyScienceChiller.o: $(DEPENDENCES)
	g++ $(CFLAGS) PolyScienceChiller.cpp -o PolyScienceChiller.o $(addprefix -I, $(INCLUDESDIR))

chiller: chiller.o PolyScienceChiller.o $(DEPENDENCES)
	g++ $(GFLAGS) chiller.o PolyScienceChiller.o -o chiller

chiller.o: chiller.cpp $(DEPENDENCES)
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) chiller.cpp -o chiller.o

clean:
	rm -f *.o *~ *.so chiller

cleanall: clean
	rm -fr ../../lib/libPolyScienceChiller.so.1.0.1 ../../lib/libPolyScienceChiller.so

