.SUFFIXES : .cc .o

CXX = g++

INC_DIR = /home/dnatuna/projects/ubuntuJewely/header
SRC_DIR = /home/dnatuna/projects/ubuntuJewely/src

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = -lglut -lGLU -lGL $(OPENCV)
CXXFLAGS = -g -Wall -std=c++14 -O2 -I$(INC_DIR) -I/usr/local/include/opencv -I/usr/local/include/opencv2

SRCS = $(SRC_DIR)/dwColors.cpp $(SRC_DIR)/dwETF.cpp $(SRC_DIR)/dwLIC.cpp $(SRC_DIR)/ETF.cpp $(SRC_DIR)/fdog.cpp $(SRC_DIR)/modelGen.cpp $(SRC_DIR)/jewelModelGenerator.cpp
OBJS = $(SRC_DIR)/dwColors.o $(SRC_DIR)/dwETF.o $(SRC_DIR)/dwLIC.o $(SRC_DIR)/ETF.o $(SRC_DIR)/fdog.o $(SRC_DIR)/modelGen.o $(SRC_DIR)/jewelModelGenerator.o

PROG = jewel

all : $(PROG)

$(PROG) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)
dep :
	gccmakedep $(INC_DIR) $(SRCS)

