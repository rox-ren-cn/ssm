INCDIR = $(SSMWORKPATH)/inc/
LIBDIR = $(SSMWORKPATH)/lib/
EXEDIR = $(SSMWORKPATH)/bin/

SSM = $(EXEDIR)SSM
LIB = -L$(LIBDIR) -lerrlog -lutil -lhsmcmd -lsec -lfile -lcommTCPIP -lcommSERIALPORT  -L/usr/lib/  -lssl -lcrypto
XML2LIBS        = -L/usr/lib -lxml2 -lz -pthread -lm

CPP = g++
CPPFLAG = -c -g -I$(INCDIR) -D_DATE_VALIDATION

OBJ = ssm.o license.o rsakey.o

project: $(SSM) rmobj

$(SSM):$(OBJ)
	g++ -o $(SSM) -Wl -Bdynamic -lgcc_s $(OBJ) $(LIB) $(XML2LIBS)

ssm.o:ssm.cpp $(INCLUDE)
	$(CPP) -o ssm.o $(CPPFLAG) ssm.cpp

license.o:license.cpp $(INCLUDE)
	$(CPP) -o license.o $(CPPFLAG) license.cpp

rsakey.o:rsakey.cpp $(INCLUDE)
	$(CPP) -o rsakey.o $(CPPFLAG) rsakey.cpp
	
rmobj:
	rm -f *.o *old*


