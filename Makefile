CC = clang++
CCFLAGS = -W -Wall -std=c++14
LD = -lreadline
SRC = src/simple_ra.cpp src/storage.cpp src/parser.cpp src/expression.cpp src/table.cpp src/cell.cpp 
EXE = simple_ra
DEBUG = -D DEBUG -g

.PHONY: clean distclean

all: build

clean: distclean

distclean:
	rm -rf *o ${EXE} .data* *.dSYM

build: 
	${CC} ${CCFLAGS} ${SRC} -o ${EXE} ${LD} 

debug:
	${CC} ${DEBUG} ${CCFLAGS} ${SRC} -o ${EXE} ${LD} 

run:
	./${EXE}
