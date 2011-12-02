ARQ=64
SOURCE_DIR=$(PWD)/src
ifeq ($(ARQ),64)
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller MPPCCore
LIBS_DIR=/usr/lib64 $(PWD)/lib /opt/caen/v1718/lib64
FLAGS=-g -Wall -O2 -m64
CFLAGS=-g -Wall -O2 -m64 -fPIC -DLINUX -c
LIBS=$(GENERATEDLIBS) CAENVME
else
GENERATEDLIBS=TestAssembly CAENVMEenvironment ProgramParameters PolyScienceChiller HVPowerSupply MPPCCore
LIBS_DIR=/usr/lib $(PWD)/lib /opt/caen/v1718/lib
FLAGS=-g -Wall -O2 -m32
CFLAGS=-g -Wall -O2 -m32 -fPIC -DLINUX -c
LIBS=$(GENERATEDLIBS) CAENVME hscaenet caenhvwrapper
endif
LDFLAGS		:= -shared -Wl,-soname,libStage2
LIBNAME		:= libStage2.so
#LIBS=$(GENERATEDLIBS) CAENVME pthread hscaenet caenhvwrapper
SOURCES=$(addprefix $(SOURCE_DIR)/,$(join $(GENERATEDLIBS),$(addprefix /,$(addsuffix .cpp,$(GENERATEDLIBS)))))
INCLUDES=$(addprefix $(SOURCE_DIR)/,$(join $(GENERATEDLIBS),$(addprefix /,$(addsuffix .h,$(GENERATEDLIBS)))))
INCLUDES_DIR=$(addprefix $(SOURCE_DIR)/,$(GENERATEDLIBS))
LIBREPO=$(addprefix $(PWD)/lib/lib,$(addsuffix .so,$(GENERATEDLIBS)))

#############################################################################################################

default: src/maindir.mk $(SOURCES) $(INCLUDES) $(addprefix lib/lib,$(addsuffix .so,$(GENERATEDLIBS))) src/system/system.o
	@echo -e "\e[0;32mGenerating binary: system\e[0m"
	g++ $(LDFLAGS) -o lib/$(LIBNAME) src/system/system.o 
	g++ $(FLAGS) src/system/system.o -o bin/system  $(addprefix -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))

src/maindir.mk:
	@echo -e "MAINDIR="$(PWD)"\nARQ="$(ARQ) > src/maindir.mk

show:
	@echo -e "\e[0;32m" $(INCLUDES) "\e[0m"
	@echo $(SOURCES)
	@echo $(SOURCES)
	@echo $(LIBREPO)

lib/libMPPCCore.so: src/MPPCCore/MPPCCore.h src/MPPCCore/MPPCCore.cpp
	@echo -e "\e[0;32mGenerating Core Class\e[0m"
	make -C src/MPPCCore/

lib/libCAENVMEenvironment.so: src/CAENVMEenvironment/CAENVMEenvironment.h src/CAENVMEenvironment/CAENVMEenvironment.cpp
	@echo -e "\e[0;32mGenerating CAEN QDC Library\e[0m"
	make -C src/CAENVMEenvironment

lib/libTestAssembly.so: src/TestAssembly/TestAssembly.h src/TestAssembly/TestAssembly.cpp
	@echo -e "\e[0;32mGenerating Controller Library\e[0m"
	make -C src/TestAssembly

lib/libProgramParameters.so: src/ProgramParameters/ProgramParameters.h src/ProgramParameters/ProgramParameters.cpp
	@echo -e "\e[0;32mGenerating Parameter Parser Library\e[0m"
	make -C src/ProgramParameters

lib/libPolyScienceChiller.so: src/PolyScienceChiller/PolyScienceChiller.h src/PolyScienceChiller/PolyScienceChiller.cpp
	@echo -e "\e[0;32mGenerating PolyScience Chiller's Lybrary\e[0m"
	make -C src/PolyScienceChiller

lib/libHVPowerSupply.so: src/HVPowerSupply/HVPowerSupply.h src/HVPowerSupply/HVPowerSupply.cpp
	@echo -e "\e[0;32mGenerating BiasVoltage Power Supply Library\e[0m"
	make -C src/HVPowerSupply

src/system/system.o: src/system/system.cpp $(SOURCES) $(INCLUDES)
	@echo -e "\e[0;32mCreating Object from system\e[0m"
	make -C src/system

src/system/system-v2.0: $(SOURCES)/system/system-v2.0.o 
	@echo -e "\e[0;32mGenerating binary: system-v2.0\e[0m"
	g++ $(CFLAGS) system-v2.0.o -o bin/system-v2.0 $(addprefix -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))

src/system/system-v2.0.o: system-v2.0.cpp 
	@echo -e "\e[0;32mCreating object from system\e[0m"
	make -C src/system system-v2.0.o

ufit: src/scripts/ufitMPPC.C src/scripts/ufitMPPC.o
	@echo -e "\e[0;32mGenerating Compiled version of script ufit\e[0m"
	g++ `root-config --libs` -DCHECKFOLDEREXISTANCE src/scripts/ufitMPPC.o -o $(PWD)/bin/ufit

src/scripts/ufitMPPC.o: src/scripts/ufitMPPC.C
	g++ `root-config --cflags` -DCHECKFOLDEREXISTANCE -c src/scripts/ufitMPPC.C -o src/scripts/ufitMPPC.o

chiller: src/PolyScienceChiller/chiller.o lib/libPolyScienceChiller.so
	@echo -e "\e[0;32mLinking Chiller Controller program\e[0m"
	g++ -g src/PolyScienceChiller/chiller.o -L/data/test/lib -lPolyScienceChiller -o bin/chiller

src/PolyScienceChiller/chiller.o: src/PolyScienceChiller/chiller.cpp src/PolyScienceChiller/PolyScienceChiller.h src/PolyScienceChiller/PolyScienceChiller.cpp
	make -C src/PolyScienceChiller chiller.o

VI_measure: src/HVPowerSupply/I_test.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ -g -m32 src/HVPowerSupply/I_test.o -o /data/test/bin/VI_measure $(addprefix -L,$(LIBSDIR)) -Llib -lHVPowerSupply 
	#$(addprefix -l,$(LIBS)) -lHVPowerSupply

src/HVPowerSupply/I_test.o: src/HVPowerSupply/I_test.cpp src/HVPowerSupply/HVPowerSupply.cpp src/HVPowerSupply/HVPowerSupply.h $(PWD)/src/HVPowerSupply/CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ -m32 -c  src/HVPowerSupply/I_test.cpp -o src/HVPowerSupply/I_test.o $(addprefix -I,$(INCLUDEDIR))

setBias: src/HVPowerSupply/setBias.o lib/libHVPowerSupply.so lib/libhscaenet.so lib/libcaenhvwrapper.so
	g++ -m32 -g src/HVPowerSupply/setBias.o -o bin/setBias $(addprefix -L,$(LIBSDIR)) -Llib -lHVPowerSupply

src/HVPowerSupply/setBias.o: src/HVPowerSupply/setBias.cpp src/HVPowerSupply/HVPowerSupply.h src/HVPowerSupply/CAENHVWrapper_2_11/include/CAENHVWrapper.h
	g++ -m32 -c src/HVPowerSupply/setBias.cpp -o src/HVPowerSupply/setBias.o $(addprefix -I,$(INCLUDEDIR))



getData: $(SOURCES) $(INCLUDES) $(addprefix lib/lib,$(addsuffix .so,$(GENERATEDLIBS))) src/system/getData.o
	@echo -e "\e[0;32mGenerating binary: system\e[0m"
	g++ $(FLAGS) src/system/getData.o -o bin/getData  $(addprefix -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))


src/system/getData.o: src/system/getData.cpp $(SOURCES) $(INCLUDES)
	@echo -e "\e[0;32mCreating Object from getData\e[0m"
	make -C src/system getData.o

qdc: src/scripts/qdc.C src/scripts/qdc.o
	@echo -e "\e[0;32mGenerating Compiled version of script qdc\e[0m"
	g++ `root-config --libs` -DCHECKFOLDEREXISTANCE src/scripts/qdc.o -o $(PWD)/bin/qdc

src/scripts/qdc.o: src/scripts/qdc.C
	g++ `root-config --cflags` -DCHECKFOLDEREXISTANCE -c src/scripts/qdc.C -o src/scripts/qdc.o

clean:
	make clean -C src/system
	make clean -C src/MPPCCore
	make clean -C src/TestAssembly
	make clean -C src/CAENVMEenvironment
	make clean -C src/ProgramParameters
	make clean -C src/HVPowerSupply
	make clean -C src/PolyScienceChiller
	rm -fr *.o bin/system src/scripts/ufitMPPC.o

cleanall: clean
	make cleanall -C src/MPPCCore
	make cleanall -C src/TestAssembly
	make cleanall -C src/CAENVMEenvironment
	make cleanall -C src/ProgramParameters
	make cleanall -C src/HVPowerSupply
	make cleanall -C src/PolyScienceChiller



