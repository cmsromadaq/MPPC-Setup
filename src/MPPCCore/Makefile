include ../maindir.mk
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64
FLAGSSO=`root-config --libs`-m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC -DLINUX `root-config --libs`
CFLAGS=-g -Wall -O2 -m64 -fPIC -DLINUX `root-config --cflags` -c
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib $(MAINDIR)/lib
FLAGSSO=`/home/utfsm/alan/Downloads/root2/bin/root-config --libs` -m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC -DLINUX -DRUNNINGINM32MODE `/home/utfsm/alan/Downloads/root2/bin/root-config --libs`
CFLAGS=-g -Wall -O2 -m32 -fPIC -DLINUX -DRUNNINGINM32MODE `/home/utfsm/alan/Downloads/root2/bin/root-config --cflags` -c
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller HVPowerSupply
endif
SOURCES=$(addprefix $(MAINDIR)/src/,$(join $(GENERATEDLIBS),$(addprefix /,$(addsuffix .cpp,$(GENERATEDLIBS)))))
INCLUDES=$(addprefix $(MAINDIR)/src/,$(join $(GENERATEDLIBS),$(addprefix /,$(addsuffix .h,$(GENERATEDLIBS)))))
LIB=CAENVME hscaenet caenhvwrapper
LIBS=CAENVME hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=$(addprefix $(MAINDIR)/src/,$(GENERATEDLIBS))

#########################################################

MPPCCore.so: $(MAINDIR)/lib/libMPPCCore.so

$(MAINDIR)/lib/libMPPCCore.so: MPPCCore.o
	mkdir -p ../lib
	g++ $(FLAGSSO),libMPPCCore.so -o $(MAINDIR)/lib/libMPPCCore.so.1.0.1 MPPCCore.o
	ln -fs $(MAINDIR)/lib/libMPPCCore.so.1.0.1 $(MAINDIR)/lib/libMPPCCore.so

MPPCCore.o: $(SOURCES) $(INCLUDES)
	g++ $(CFLAGS) MPPCCore.cpp -o MPPCCore.o $(addprefix -I, $(INCLUDESDIR))

test: test.o MPPCCore.o 
	g++ $(FLAGS) test.o MPPCCore.o -o test

test.o: test.cpp MPPCCore.cpp
	g++ $(CFLAGS) $(addprefix -I, $(INCLUDESDIR)) test.cpp -o test.o

clean:
	rm -fr *.o *~ *.so test
	rm -fr $(MAINDIR)/bin/setaddress
	rm -fr $(MAINDIR)/bin/onvol
	rm -fr $(MAINDIR)/bin/offvol

cleanall: clean
	rm -fr $(MAINDIR)/lib/libMPPCCore.so.1.0.1 $(MAINDIR)/lib/libMPPCCore.so

