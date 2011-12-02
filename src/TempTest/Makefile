include ../maindir.mk
ifeq ($(ARQ),x86_64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64 $(MAINDIR)/lib/ 
FLAGSSO=-m64 -shared -Wl,-soname
FLAGS=-m64 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m64 -fPIC -DUNIX -c
ROOTSYS=/opt/root/5.28.00
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib $(MAINDIR)/lib
FLAGSSO=-m32 -shared -Wl,-soname
FLAGS=-m32 -g -Wall -O2 -fPIC -DUNIX
CFLAGS=-g -Wall -O2 -m32 -fPIC -DUNIX -c
ROOTSYS=/home/utfsm/royarzun/root32/root
endif
BINARY=getTemp
OBJECT= getTemp.o SocketConnection.o
SOURCE= getTemp.cpp SocketConnection.cpp
#LIB=CAENVME pthread hscaenet caenhvwrapper
LIB=CAENVME hscaenet caenhvwrapper
LIBS=CAENVME hscaenet caenhvwrapper HVPowerSupply
#LIBS=CAENVME pthread hscaenet caenhvwrapper HVPowerSupply
INCLUDESDIR=. $(MAINDIR)/src/HVPowerSupply/CAENHVWrapper_2_11/include

########################################################################
all: $(BINARY) 
	
$(BINARY): $(SOURCE)
	g++  $(SOURCE) -o /data/test/bin/$(BINARY)

clean:
	rm -fr *.o *~ *.so *.so.1.0.1 /data/test/bin/getTemp

