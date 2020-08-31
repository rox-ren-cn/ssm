INCDIR = $(SSMWORKPATH)/inc/
LIBDIR = $(SSMWORKPATH)/lib/

INCLUDE = $(INCDIR)util.h 
UTILLIB = $(LIBDIR)/libutil.a

CPP = g++
CPPFLAG = -c -g -I$(INCDIR)

OBJ = util.o 

project: $(UTILLIB) rmobj

$(UTILLIB):util.o
	ar -r $(UTILLIB) util.o

util.o:util.cpp $(INCLUDE)
	$(CPP) -o util.o $(CPPFLAG) util.cpp

rmobj:
	rm -f *.o *old*
