ARQ=$(shell getconf LONG_BIT)
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 lib
FLAGSSO=-m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m64 -fPIC -DUNIX -c
else
LIBSDIR=/usr/lib lib
FLAGSSO=-m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m32 -fPIC -DUNIX -c
endif
#LIB=CAENVME pthread hscaenet caenhvwrapper
LIB=hscaenet caenhvwrapper
LIBS=hscaenet caenhvwrapper
#LIBS=CAENVME pthread hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=. ./CAENHVWrapper_2_11/include

########################################################################

lib/libHVPowerSupply.so: HVPowerSupply.o wrapper HVPowerSupply.h HVPowerSupply.cpp
	g++ $(FLAGSSO),libHVPowerSupply.so -o lib/libHVPowerSupply.so.1.0.1 HVPowerSupply.o $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIB))
	ln -fs lib/libHVPowerSupply.so.1.0.1 lib/libHVPowerSupply.so

HVPowerSupply.o: HVPowerSupply.h HVPowerSupply.cpp ./CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) HVPowerSupply.cpp -o HVPowerSupply.o $(addprefix -I,$(INCLUDESDIR))

wrapper: lib/libhscaenet.so lib/libcaenhvwrapper.so
	@echo "Wrapper Done"

lib/libhscaenet.so: ./CAENHVWrapper_2_11/Lib/hscaenetlib.so.1.6
	cp ./CAENHVWrapper_2_11/Lib/hscaenetlib.so.1.6 lib/.
	ln -fs lib/hscaenetlib.so.1.6 lib/libhscaenet.so

lib/libcaenhvwrapper.so: ./CAENHVWrapper_2_11/Lib/libcaenhvwrapper.so.2.11
	cp ./CAENHVWrapper_2_11/Lib/libcaenhvwrapper.so.2.11 lib/.
	ln -fs lib/libcaenhvwrapper.so.2.11 lib/libcaenhvwrapper.so

test: test.o lib/libhscaenet.so lib/libcaenhvwrapper.so HVPowerSupply.o 
	g++ $(FLAGS) test.o HVPowerSupply.o -o test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))

test.o: test.cpp HVPowerSupply.h ./CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) test.cpp -o test.o $(addprefix -I,$(INCLUDEDIR))

vi_test: vi_test.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ $(FLAGS) vi_test.o -o vi_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))

vi_test.o: VI_test.cpp HVPowerSupply.h ./CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) VI_test.cpp -o vi_test.o $(addprefix -I,$(INCLUDEDIR))

I_test: I_test.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ $(FLAGS) I_test.o -o bin/I_test $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

I_test.o: I_test.cpp HVPowerSupply.h $(MAINDIR)/src/HVPowerSupply/CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) I_test.cpp -o I_test.o $(addprefix -I,$(INCLUDEDIR))

setBias: setBias.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ $(FLAGS) setBias.o -o setBias $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS)) -lHVPowerSupply

setBias.o: setBias.cpp HVPowerSupply.h $(MAINDIR)/src/HVPowerSupply/CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ $(CFLAGS) setBias.cpp -o setBias.o $(addprefix -I,$(INCLUDEDIR))

clean:
	rm -fr *.o *~ *.so *.so.1.0.1 test vi_test setBias

cleanall: clean
	rm -fr lib/libHVPowerSupply.so
	rm -fr lib/libHVPowerSupply.so.1.0.1
	rm -fr lib/libcaenhvwrapper.so.2.11
	rm -fr lib/libcaenhvwrapper.so
	rm -fr lib/hscaenetlib.so.1.6
	rm -fr lib/libhscaenet.so

