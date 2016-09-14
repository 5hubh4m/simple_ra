CC = clang++
CCFLAGS = -W -Wall -std=c++14
LD = -lreadline
SRC = src/main.cpp src/simple_parser.cpp src/expression.cpp src/table.cpp src/cell.cpp 
EXE = simple_ra

.PHONY: clean distclean

all: main

clean:
	rm -f *.o

distclean: clean
	rm -f *o ${EXE}

main: 
	${CC} ${CCFLAGS} ${SRC} -o ${EXE} ${LD} 

debug:
	${CC} -g ${CCFLAGS} ${SRC} -o ${EXE} ${LD} 

run:
	./${EXE}
