ifndef PREFIX
PREFIX := /usr/local
endif

ifdef MINGW
bin_suffix := .exe
endif

bin_suffix ?= .out

ifndef CXX
CXX := g++
endif

ifndef LIBPNG_CONFIG
LIBPNG_CONFIG := libpng-config
endif


LDFLAGS= -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window -pthread
CCFLAGS = -Wall -Wextra -Wpedantic -O3
# PATH_TO_ARKANOID=Tutorials/DiveIntoC++11/1_Arkanoid/
# ARKANOID_EXEC := ${PATH_TO_ARKANOID}p1.cpp

sources := $(wildcard : *.cpp)
cpp_headers := $(wildcard : *.hpp *.h)
objects := $(sources:.cpp=.o)
targets := main

make_cflags := $(CCFLAGS) -I$(PREFIX)/include $(shell $(LIBPNG_CONFIG) --cflags)
make_ldflags := -L$(PREFIX)/lib $(shell $(LIBPNG_CONFIG) --ldflags) $(LDFLAGS)

all: $(targets)

$(targets): $(objects)
	$(CXX) -o $@ $< $(make_ldflags)

%.o: %.cpp $(cpp_headers)
	$(CXX) -c -o $@ $< $(make_cflags)

clean:
	rm -f $(targets) $(objects)

.PHONY: all clean
