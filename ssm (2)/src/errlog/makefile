INCDIR = $(SSMWORKPATH)/inc/
LIBDIR = $(SSMWORKPATH)/lib/

INCLUDE = $(INCDIR)util.h \
	$(INCDIR)errlog.h
 
ERRLOGLIB = $(LIBDIR)/liberrlog.a

CPP = g++
CPPFLAG = -c -g -I$(INCDIR)

OBJ = errlog.o 

project: $(ERRLOGLIB) rmobj

$(ERRLOGLIB):errlog.o
	ar -r $(ERRLOGLIB) errlog.o

errlog.o:errlog.cpp $(INCLUDE)
	$(CPP) -o errlog.o $(CPPFLAG) errlog.cpp

rmobj:
	rm -f *.o *old*
