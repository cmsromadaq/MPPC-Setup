include ../maindir.mk
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64
FLAGSSO=`root-config --libs` -m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC -DUNIX `root-config --libs`
CFLAGS=-g -Wall -O2 -m64 -fPIC -DUNIX `root-config --cflags` -c
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib $(MAINDIR)/lib 
FLAGSSO=`/home/utfsm/alan/Downloads/root/bin/root-config --libs` -m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC -DUNIX -DRUNNINGINM32MODE `/home/utfsm/alan/Downloads/root/bin/root-config --libs`
CFLAGS=-g -Wall -O2 -m32 -fPIC -DUNIX -DRUNNINGINM32MODE `/home/utfsm/alan/Downloads/root/bin/root-config --cflags` -c
endif
LIB=CAENVME hscaenet caenhvwrapper
LIBS=CAENVME hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=.

#################################################################

ProgramParameters.so: ../../lib/libProgramParameters.so

../../lib/libProgramParameters.so: ProgramParameters.o
	mkdir -p $(MAINDIR)/lib
	g++ $(FLAGSSO),libProgramParameters.so -o $(MAINDIR)/lib/libProgramParameters.so.1.0.1 ProgramParameters.o
	ln -fs $(MAINDIR)/lib/libProgramParameters.so.1.0.1 $(MAINDIR)/lib/libProgramParameters.so

ProgramParameters.o: ProgramParameters.cpp ProgramParameters.h
	g++ $(CFLAGS) ProgramParameters.cpp -o ProgramParameters.o  $(addprefix -I,$(INCLUDESDIR)) 

prueba: prueba.o ProgramParameters.o ProgramParameters.cpp ProgramParameters.h
	g++ $(FLAGS) prueba.o ProgramParameters.o -o prueba $(addprefix -L,$(LIBSDIR))

prueba.o: prueba.cpp ProgramParameters.cpp ProgramParameters.h
	g++ $(CFLAGS) prueba.cpp -o prueba.o $(addprefix -I,$(INCLUDESDIR))

clean:
	rm -f *.o *~ *.so prueba

cleanall: clean
	rm -fr ../../lib/libProgramParameters.so.1.0.1 ../../lib/libProgramParameters.so
