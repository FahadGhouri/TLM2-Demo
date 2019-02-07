# Universitaet Stuttgart, ES - ITI 
# Modeling, Specification and Simulation SS 2014
# Feel free to modify this Makefile 

# These should point to the correct SystemC directories
SYSTEMC_INCDIR = /usr/local/systemc-2.3.1/include
SYSTEMC_LIBDIR = /usr/local/systemc-2.3.1/lib-linux64

CXX     = g++
CXXFLAGS = -O1 -g -Wall -Wno-reorder

INCDIR = -I. -I $(SYSTEMC_INCDIR)

LDFLAGS = -L. -L $(SYSTEMC_LIBDIR) -Wl,-rpath,$(SYSTEMC_LIBDIR)
LDLIBS = -lsystemc

SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

TARGET = $(notdir $(shell pwd))

$(TARGET): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(LDLIBS) -o $@ $(OBJECTS)
	@echo
	@echo "----------------------------------------"
	@echo " Compilation finished successfully!"
	@echo " Run simulation with the following command:"
	@echo " ./$@"

clean:  
	-rm $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCDIR) -c -o $@ $<
