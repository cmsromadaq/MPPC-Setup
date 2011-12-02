include ../maindir.mk
ifeq ($(ARQ),64)
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller MPPCCore
CFLAGS=-g -Wall -O2 -m64 -fPIC -DLINUX -c `root-config --cflags`
else
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller HVPowerSupply MPPCCore
CFLAGS=-g -Wall -O2 -m32 -fPIC -DLINUX -DRUNNINGINM32MODE -c `/home/utfsm/alan/Downloads/root/bin/root-config --cflags`
endif
INCLUDES=$(addprefix $(MAINDIR)/src/,$(join $(GENERATEDLIBS),$(addprefix /,$(addsuffix .h,$(GENERATEDLIBS)))))
INCLUDES_DIR=$(addprefix $(MAINDIR)/src/,$(GENERATEDLIBS))

################################################################


system.o: system.cpp $(INCLUDES)
	g++ $(CFLAGS) system.cpp -o system.o $(addprefix -I, $(INCLUDES_DIR))

system-v2.0.o: system-v2.0.cpp $(INCLUDES)
	g++ $(CFLAGS) system-v2.0.cpp -o system-v2.0.o $(addprefix -I, $(INCLUDES_DIR)) 

system-v3.0.o: system-v3.0.cpp $(INCLUDES)
	g++ $(CFLAGS) system-v3.0.cpp -o system-v3.0.o $(addprefix -I, $(INCLUDES_DIR))

system-v4.0.o: system-v4.0.cpp $(INCLUDES)
	g++ $(CFLAGS) system-v4.0.cpp -o system-v4.0.o $(addprefix -I, $(INCLUDES_DIR))

getData.o: getData.cpp $(INCLUDES)
	g++ $(CFLAGS) getData.cpp -o getData.o $(addprefix -I, $(INCLUDES_DIR))


clean:
	rm -fr *.o ../../bin/system-v2.0 ../../bin/system *~

cleanall: clean
