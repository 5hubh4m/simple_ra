#ifndef TABLE_HPP
#define TABLE_HPP

#include <map>
#include <vector>
#include <string>

#include "cell.hpp"

typedef std::map< std::string, std::pair<size_t, Type > > Schema;
typedef std::vector< Cell > Tuple;
typedef std::vector< Tuple > TableArray;

class Table {
  private:
    size_t row_size;
    Schema schema;
    TableArray table;

  public:
    Schema getSchema ();
    TableArray getTable ();
    size_t size ();
    Tuple operator[] (size_t);
    void operator+= (Tuple);
    void rename (std::vector< std::string >);
    void print (void);
};

#endif
