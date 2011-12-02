include ../maindir.mk
ifeq ($(ARQ),64)
LIBSDIR=/usr/lib64 /opt/caen/v1718/lib64
FLAGS=-m64 -shared -Wl,-soname
FLAG=-m64 -g -Wall -O2 -fPIC -DLINUX
CFLAGS=-g -Wall -O2 -m64 -fPIC -DLINUX -c
else
LIBSDIR=/usr/lib /opt/caen/v1718/lib
FLAGS=-m32 -shared -Wl,-soname
FLAG=-m32 -g -Wall -O2 -fPIC -DLINUX
CFLAGS=-g -Wall -O2 -m32 -fPIC -DLINUX -c
endif
#LIBS=CAENVME pthread
LIBS=CAENVME
INCLUDESDIR=.


CAENVMEenvironment.so: $(MAINDIR)/lib/libCAENVMEenvironment.so

$(MAINDIR)/lib/libCAENVMEenvironment.so: CAENVMEenvironment.o
	mkdir -p $(MAINDIR)/lib
	g++ $(FLAGS),$(MAINDIR)/lib/libCAENVMEenvironment.so -o $(MAINDIR)/lib/libCAENVMEenvironment.so.0.0.1 CAENVMEenvironment.o  $(addprefix -L,$(LIBSDIR)) $(addprefix -l,$(LIBS))
	ln -fs $(MAINDIR)/lib/libCAENVMEenvironment.so.0.0.1 $(MAINDIR)/lib/libCAENVMEenvironment.so

CAENVMEenvironment.o: CAENVMElib.h CAENVMEtypes.h CAENVMEoslib.h CAENVMEenvironment.h CAENVMEenvironment.cpp
	g++ $(CFLAGS) CAENVMEenvironment.cpp -o CAENVMEenvironment.o $(addprefix -I,$(INCLUDESDIR))

#test: $(MAINDIR)/lib/libCAENVMEenvironment.so CAENVMEenvironment.h CAENVMEenvironment.cpp test.cpp test.o
test: CAENVMEenvironment.o CAENVMEenvironment.h CAENVMEenvironment.cpp test.cpp test.o
	g++ $(FLAG) test.o CAENVMEenvironment.o -o test -lCAENVME $(addprefix -L,$(LIBSDIR)) -L$(MAINDIR)/lib
	#g++ $(FLAG) test.o -o test -lCAENVMEenvironment -L$(MAINDIR)/lib

test.o: CAENVMEenvironment.h CAENVMEenvironment.cpp test.cpp
	g++ $(CFLAGS) test.cpp -o test.o 

clean:
	rm -fr *.o *~ *.so test

cleanall: clean
	rm -fr $(MAINDIR)/lib/libCAENVMEenvironment.so.0.0.1 $(MAINDIR)/lib/libCAENVMEenvironment.so libCAENVMEenvironment.so.0.0.1
