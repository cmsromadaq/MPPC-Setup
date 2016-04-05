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


########################################################################

lib/libHVPowerSupply.so: HVPowerSupply.o HVPowerSupply.h HVPowerSupply.cpp
	g++ $(FLAGSSO),libHVPowerSupply.so -o lib/libHVPowerSupply.so.1.0.1 HVPowerSupply.o $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))
#	ln -fs lib/libHVPowerSupply.so.1.0.1 lib/libHVPowerSupply.so

HVPowerSupply.o: HVPowerSupply.h HVPowerSupply.cpp ./CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) HVPowerSupply.cpp -o HVPowerSupply.o $(addprefix -I,$(INCLUDESDIR))


test: test.o  HVPowerSupply.o 
	g++ $(FLAGS) test.o HVPowerSupply.o -o test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))

test.o: test.cpp HVPowerSupply.h ./CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) test.cpp -o test.o $(addprefix -I,$(INCLUDEDIR))

vi_test: vi_test.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ $(FLAGS) vi_test.o -o vi_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))

vi_test.o: VI_test.cpp HVPowerSupply.h 
	g++ $(CFLAGS) VI_test.cpp -o vi_test.o $(addprefix -I,$(INCLUDEDIR))

I_test: I_test.o lib/libHVPowerSupply.so 
	g++ $(FLAGS) I_test.o -o bin/I_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

I_test.o: I_test.cpp HVPowerSupply.h 
	g++ $(CFLAGS) I_test.cpp -o I_test.o $(addprefix -I,$(INCLUDEDIR))

setBias: setBias.o lib/libHVPowerSupply.so
	g++ $(FLAGS) setBias.o -o setBias $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

setBias.o: setBias.cpp HVPowerSupply.h 
	g++ $(CFLAGS) setBias.cpp -o setBias.o $(addprefix -I,$(INCLUDEDIR))

clean:
	rm -fr *.o *~ *.so *.so.1.0.1 test vi_test setBias

cleanall: clean
	rm -fr lib/libHVPowerSupply.so
	rm -fr lib/libHVPowerSupply.so.1.0.1

