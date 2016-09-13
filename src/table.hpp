#ifndef TABLE_HPP
#define TABLE_HPP

#include <map>
#include <vector>
#include <string>

#include "cell.hpp"

typedef std::vector< std::pair< std::string, Type > > Schema;
typedef std::vector< Cell > Tuple;
typedef std::vector< Tuple > TableArray;

class Table {
  private:
    Schema schema;
    TableArray table;

  public:
    Table () {}
    Table (Schema s);

    Schema getSchema ();
    TableArray getTable ();
    size_t size ();
    Tuple operator[] (size_t);
    void operator+= (Tuple);
    void operator-= (Tuple);
    void rename (std::vector< std::string >);
    void print (void);
};

#endif
