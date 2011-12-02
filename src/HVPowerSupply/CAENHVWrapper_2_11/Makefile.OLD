########################################################################
#                                                                      #
#              --- CAEN SpA - Computing Division ---                   #
#                                                                      #
#   CAENHVWRAPPER Software Project                                     #
#                                                                      #
#   Makefile: it installs shared library and HVWrapperdemo             #
#                                                                      #
#   Created: January 2000                                              #
#                                                                      #
#   Auth: A. Morachioli                                                #
#                                                                      #
########################################################################

GLOBALDIR=      HVWrapperDemo/

PROGRAM=	$(GLOBALDIR)HVWrappdemo

CC=		gcc

FLAGS=		-DUNIX

LFLAGS=

DIRLIB =        /usr/lib
LIBS=		-L $(DIRLIB) -lcaenhvwrapper -lncurses


INCLUDEDIR=	-I./$(GLOBALDIR) -I./include/

INSTALLDIR=	./

SOURCES=	$(GLOBALDIR)MainWrapp.c $(GLOBALDIR)CmdWrapp.c $(GLOBALDIR)console.c

OBJECTS=	$(GLOBALDIR)MainWrapp.o $(GLOBALDIR)CmdWrapp.o $(GLOBALDIR)console.o

INCLUDES=	MainWrapp.h CAENHVWrapper.h console.h caenhvoslib.h

LIB=		Lib/libcaenhvwrapper.so.2.11 Lib/hscaenetlib.so.1.6

INSTALLDIRLIB=	/usr/lib

########################################################################

ARFLAGS=		r

CFLAGS=			$(FLAGS)

clean:
			rm -f $(OBJECTS) $(PROGRAM)

install:
			install $(LIB) $(INSTALLDIRLIB)
			ln -sf $(INSTALLDIRLIB)/libcaenhvwrapper.so.2.11 $(INSTALLDIRLIB)/libcaenhvwrapper.so
			ln -sf $(INSTALLDIRLIB)/hscaenetlib.so.1.6 $(INSTALLDIRLIB)/libhscaenet.so
			ldconfig
			make all
			rm -f $(OBJECTS)

all:			$(PROGRAM)

$(PROGRAM):		$(OBJECTS)
			$(CC) $(CFLAGS) $(LFLAGS) -o $(PROGRAM) $(OBJECTS)\
			$(LIBS)

$(OBJECTS):		$(SOURCES)

$(GLOBALDIR)%.o:	$(GLOBALDIR)%.c
			$(CC) $(CFLAGS) $(INCLUDEDIR) -o $@ -c $<

