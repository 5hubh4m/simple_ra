#ifndef CELL_HPP
#define CELL_HPP

#include <string>

enum Type {
    INTEGER, FLOAT, STRING, INVALID
};

struct Data {
    int i;
    float f;
    std::string s;
};

class Cell {
  private:
    Data value;
    Type type;

  public:
    Cell ();
    Cell (int a);
    Cell (std::string a);
    Cell (float a);

    Type getType ();
    Data getVal ();
    std::string show ();

    bool operator== (Cell d);
    bool operator<= (Cell d);
    bool operator> (Cell d);
    bool operator< (Cell d);
    bool operator!= (Cell d);
    bool operator>= (Cell d);
};

#endif
