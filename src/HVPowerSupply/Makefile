ARQ=$(shell getconf LONG_BIT)
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 lib
FLAGSSO=-m64 -shared  -Wl,--no-as-needed -pthread -Wl,-soname
FLAGS=-m64 -g -Wall  -Wl,--no-as-needed -pthread -O2 -fPIC -DUNIX 
CFLAGS=-g -Wall -O2 -m64 -fPIC -pthread -DUNIX -c
else
LIBSDIR=/usr/lib lib
FLAGSSO=-m32 -share -Wl,--no-as-needed d -Wl,-soname
FLAGS=-m32 -g -Wall -Wl,--no-as-needed -O2 -fPIC -DUNIX  
CFLAGS=-g -Wall -O2 -m32 -fPIC -pthread -DUNIX -c
endif

LIBS=caenhvwrapper dl pthread 
PROG=vi_test I_test setBias
PROGS=$(addprefix bin,$(PROG))


########################################################################
all: lib/libHVPowerSupply.so $(addprefix bin/,$(PROG))

lib/libHVPowerSupply.so: HVPowerSupply.o HVPowerSupply.h HVPowerSupply.cpp
	mkdir -p lib
	g++ $(FLAGSSO),libHVPowerSupply.so -o lib/libHVPowerSupply.so HVPowerSupply.o $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))
#	ln -fs lib/libHVPowerSupply.so.1.0.1 lib/libHVPowerSupply.so

HVPowerSupply.o: HVPowerSupply.h HVPowerSupply.cpp 
	g++ $(CFLAGS) HVPowerSupply.cpp -o HVPowerSupply.o $(addprefix -I,$(INCLUDESDIR))

bin/vi_test: vi_test.o lib/libHVPowerSupply.so 
	mkdir -p bin
	g++ $(FLAGS) vi_test.o -o bin/vi_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

vi_test.o: VI_test.cpp HVPowerSupply.h 
	g++ $(CFLAGS) VI_test.cpp -o vi_test.o $(addprefix -I,$(INCLUDEDIR))

bin/I_test: I_test.o lib/libHVPowerSupply.so 
	mkdir -p bin
	g++ $(FLAGS) I_test.o -o bin/I_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

I_test.o: I_test.cpp HVPowerSupply.h 
	g++ $(CFLAGS) I_test.cpp -o I_test.o $(addprefix -I,$(INCLUDEDIR))

bin/setBias: setBias.o lib/libHVPowerSupply.so
	mkdir -p bin
	g++ $(FLAGS) setBias.o -o bin/setBias $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

setBias.o: setBias.cpp HVPowerSupply.h 
	g++ $(CFLAGS) setBias.cpp -o setBias.o $(addprefix -I,$(INCLUDEDIR))

clean:
	rm -fr *.o *~ *.so *.so.1.0.1 bin/*

cleanall: clean
	rm -fr lib/libHVPowerSupply.so

