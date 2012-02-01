CPP=g++
 
CPPFLAGS=-g -std=c++0x
  
LIBCDS=/home/rkonow/Desktop/Doctorado/Dualsorted/libs/libcds
LIB=$(LIBCDS)/lib/libcds.a
INCLUDES=-I$(LIBCDS)/includes/
SRC=/home/rkonow/Desktop/Doctorado/Dualsorted/src/
all: 
	    $(CPP) $(CPPFLAGS) $(INCLUDES) -o test $(SRC)main.cpp $(LIB)		 
clean:
	    rm 
