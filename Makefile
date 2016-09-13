CC = clang++
CCFLAGS = -g -W -Wall -std=c++14
LD = -lreadline

.PHONY: clean distclean

all: main

clean:
	rm -f *.o

distclean: clean
	rm -f *o simple_ra

main: 
	${CC} ${CCFLAGS} src/main.cpp src/simple_parser.cpp src/expression.cpp src/table.cpp src/cell.cpp -o simple_ra ${LD} 

run:
	./simple_ra
