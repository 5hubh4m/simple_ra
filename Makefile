CC = g++
CCFLAGS = -W -Wall -std=c++1y
INCLUDE = -Isrc/
LD = -lreadline
SRC = src/simple_ra.cpp src/storage.cpp src/parser.cpp src/expression.cpp src/table.cpp src/cell.cpp 
EXE = simple_ra
DEBUG = -D DEBUG -g

.PHONY: clean distclean

all:	build

clean:	distclean

distclean:
	rm -rf *o ${EXE} .data* *.dSYM

build: 
	${CC} ${CCFLAGS} ${SRC} ${INCLUDE} -o ${EXE} ${LD} 

debug:
	${CC} ${DEBUG} ${CCFLAGS} ${SRC} ${INCLUDE} -o ${EXE} ${LD} 

run:
	./${EXE}
