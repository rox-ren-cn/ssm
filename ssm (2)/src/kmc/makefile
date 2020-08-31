INCDIR = $(SSMWORKPATH)/inc/
LIBDIR = $(SSMWORKPATH)/lib/
EXEDIR = $(SSMWORKPATH)/bin/

KMC = $(EXEDIR)KMC
LIB = -L$(LIBDIR) -lerrlog -lutil -lhsmcmd -lsec -lfile -lcommTCPIP -lcommSERIALPORT
XML2LIBS        = -L/usr/lib -lxml2 -lz -pthread -lm

INCLUDE = ${INCDIR}kmcUI.h \
		${INCDIR}kmcKeyInfo.h \
		${INCDIR}keyTokenTLV.h \
		${INCDIR}kmcFileOp.h \
		${INCDIR}conffile.h \
		${INCDIR}commTCPIP.h \
		${INCDIR}util.h \
		${INCDIR}bntcmd.h
		
CPP = g++32
CPPFLAG = -c -g -I$(INCDIR)

OBJ = keyTokenTLV.o \
		kmcFileOp.o\
		kmcKeyInfo.o\
		kmcUI.o\
		main.o
			

project: $(KMC) rmobj 

$(KMC):$(OBJ)
	${CPP} -o $(KMC) $(OBJ) $(LIB) $(XML2LIBS)

keyTokenTLV.o:keyTokenTLV.cpp $(INCLUDE)
	$(CPP) -o keyTokenTLV.o $(CPPFLAG) keyTokenTLV.cpp

kmcFileOp.o:kmcFileOp.cpp $(INCLUDE)
	$(CPP) -o kmcFileOp.o $(CPPFLAG) kmcFileOp.cpp

kmcKeyInfo.o:kmcKeyInfo.cpp $(INCLUDE)
	$(CPP) -o kmcKeyInfo.o $(CPPFLAG) kmcKeyInfo.cpp

kmcUI.o:kmcUI.cpp $(INCLUDE)
	$(CPP) -o kmcUI.o $(CPPFLAG) kmcUI.cpp
	
main.o:main.cpp $(INCLUDE)
	$(CPP) -o main.o $(CPPFLAG) main.cpp
	
rmobj:
	rm -f *.o *old*
