INCDIR = $(SSMWORKPATH)/inc/
LIBDIR = $(SSMWORKPATH)/lib/

INCLUDE = $(INCDIR)util.h \
	$(INCDIR)errlog.h \
	$(INCDIR)xmlapp.h \
	$(INCDIR)flatfile.h \
	$(INCDIR)conffile.h 

FILELIB = $(LIBDIR)/libfile.a

CPP = g++
CPPFLAG = -c -g -I$(INCDIR) -Igcc_s
XML2FLAGS       = -I/usr/include/libxml2
XML2LIBS        = -L/usr/lib -lxml2 -lz -pthread -lm

OBJ = xmlapp.o \
	flatfile.o \
	conffile.o

project: $(FILELIB) rmobj

$(FILELIB):$(OBJ)
	ar -r $(FILELIB) $(OBJ)

flatfile.o:flatfile.cpp $(INCLUDE)
	$(CPP) -o flatfile.o $(CPPFLAG) flatfile.cpp

xmlapp.o:xmlapp.cpp $(INCLUDE)
	$(CPP) -o xmlapp.o $(CPPFLAG) $(XML2FLAGS) xmlapp.cpp

conffile.o:conffile.cpp $(INCLUDE)
	$(CPP) -o conffile.o $(CPPFLAG) $(XML2FLAGS) conffile.cpp

rmobj:
	rm -f *.o *old*
