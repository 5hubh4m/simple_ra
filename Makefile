CC = g++
CXXFLAGS = -W -Wall -std=c++1y
INCLUDES =
LFLAGS =
LIBS = -lreadline
SRCS = simple_ra.cpp storage.cpp parser.cpp expression.cpp table.cpp cell.cpp
OBJS = $(SRCS:.cpp=.o)
MAIN = simple_ra
DEBUG = -D DEBUG -g
VPATH = src

.PHONY : depend clean

all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

%.o : %.cpp %.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean :
	$(RM) *.o *~ $(MAIN) .data* *.dSYM

run : $(MAIN)
	@./$(MAIN)
