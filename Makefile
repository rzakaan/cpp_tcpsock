#
# @author Riza Kaan Ucak
# @date 27.01.2022
#

.PHONY: clean

# Project
BIN=socket
OUT_LIB=tcpsocket.so

# Compiler -fPIC
CXX=c++
CXX_FLAGS=-Wall -std=c++17
LIB_FLAGS='-shared'
LIBS=-l pthread

# Directories
SRC_DIR=src
BUILD_DIR=bin

# Files
HEADERS=$(wildcard src/*.h)
SOURCES=$(wildcard src/*.cpp)
OBJECTS=${SOURCES:.cpp=.o}

# Commands
MKDIR=mkdir -p
REMOVE=rm -f

all: init clean socket

socket: ${OBJECTS}
	${CXX} ${CXX_FLAGS} ${LIBS} $^ -o ${BUILD_DIR}/$@

lib: ${OBJECTS}
	${CXX} ${LIB_FLAGS} ${LIBS} $^ -o ${BUILD_DIR}/${OUT_LIB}

%.o: %.cpp
	${CXX} ${CXX_FLAGS} -c $^ -o $@

init:
	${MKDIR} ${BUILD_DIR}

clean:
	${REMOVE} ${BUILD_DIR}/*
	${REMOVE} ${SRC_DIR}/*.o

run:
	./${BUILD_DIR}/${BIN}