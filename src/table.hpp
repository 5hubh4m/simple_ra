#ifndef TABLE_HPP
#define TABLE_HPP

#include <map>
#include <vector>
#include <string>

#include "cell.hpp"

typedef std::vector< std::pair< std::string, Type > > Schema;
typedef std::vector< Cell > Tuple;
typedef std::vector< Tuple > TableArray;

struct Predicate;

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
    bool is_valid (Tuple);      // Check schema for match.
    int is_in (Tuple);          // Check whether tuple is in table.
    Tuple operator[] (size_t);  // Get tuple at index
    Table operator^ (Table&);   // Intersection of two tables.
    Table operator+ (Table&);   // Union
    Table operator- (Table&);   // Set difference
    Table operator* (Table&);   // Cartesian product
    Table operator| (Table&);   // Natural join
    void operator += (Tuple&);  // Add tuple to table
    Table select (Predicate& p);
    Table project (std::vector< std::string >&);
    Table rename (std::vector< std::string >&);
    void print (void);
};

#endif
