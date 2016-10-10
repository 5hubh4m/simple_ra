#ifndef TABLE_HPP
#define TABLE_HPP

#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>

#include "cell.hpp"

typedef std::vector< std::pair< std::string, Type > > Schema;
typedef std::vector< Cell > Tuple;
typedef std::set< Tuple > TableArray;

struct Predicate;

class Table {
  private:
    Schema schema;
    TableArray table;

  public:
    Table () {}
    Table (const Schema&);

    Schema getSchema () const;
    size_t size () const;
    bool is_valid (const Tuple&) const;  // Check schema for match.
    int is_in (const Tuple&) const;      // Check whether tuple is in table.
    Tuple operator [] (size_t) const;    // Get tuple at index
    Table operator + (const Table&);     // Union
    Table operator - (const Table&);     // Set difference
    Table operator * (const Table&);     // Cartesian product
    void operator += (const Tuple&);     // Add tuple to table
    Table select (const Predicate& p);
    Table project (const std::vector< std::string >&);
    Table rename (const std::vector< std::string >&);
    Cell max (const std::string&) const;
    Cell min (const std::string&) const;
    Cell sum (const std::string&) const;
    Cell avg (const std::string&) const;
    Cell count (const std::string&, const Cell&) const;
    void print (void) const;

    TableArray::iterator begin () const {
        return table.begin ();
    }
    TableArray::iterator end () const {
        return table.end ();
    }
};

#endif
