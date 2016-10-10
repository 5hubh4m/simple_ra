#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <array>

enum Type {
    INTEGER, FLOAT, STRING, INVALID
};

union Data {
    int i;
    float f;
    char s[100];
};

class Cell {
  private:
    Data value;
    Type type;

  public:
    Cell ();
    Cell (int);
    Cell (std::string);
    Cell (float);

    Type getType () const;
    Data getVal () const;
    std::string show () const;

    bool operator == (const Cell&) const;
    bool operator <= (const Cell&) const;
    bool operator > (const Cell&) const;
    bool operator < (const Cell&) const;
    bool operator != (const Cell&) const;
    bool operator >= (const Cell&) const;
};

#endif
