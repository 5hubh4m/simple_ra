#ifndef TABLE_HPP
#define TABLE_HPP

#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>

#include "cell.hpp"

// Typedefs common types
typedef std::vector< std::pair< std::string, Type > > Schema;
typedef std::vector< Cell > Tuple;
typedef std::set< Tuple > TableArray;

// Forward declaration of predicate and aggregate struct
struct Predicate;
struct Aggregate;

/* Table class
 *
 * The primary abstraction and data structure to
 * contain the information regarding a single relation.
 * Implements methods to perform relational operations on the table.
 */
class Table {
  private:
    Schema schema;     // Schema for table
    TableArray table;  // The set of tuples

  public:
    // Constructors
    Table () {}
    Table (const Schema&);

    const Schema& getSchema () const;               // Return schema
    size_t size () const;                           // Return size of table

    bool is_union_compatible (const Tuple&) const;  // Check schema for union
    bool is_union_compatible (const Table&) const;  // compatibility
    bool is_union_compatible (const Schema&) const;

    int is_in (const Tuple&) const;                 // Check whether tuple is in table.

    Tuple operator [] (size_t) const;               // Get tuple at index
    Table operator + (const Table&);                // Union
    Table operator - (const Table&);                // Set difference
    Table operator * (const Table&);                // Cartesian product
    void operator += (const Tuple&);                // Add tuple to table

    Table select (const Predicate& p);
    Table project (const std::vector< std::string >&);
    Table rename (const std::vector< std::string >&);

    // Implemented aggregate functions
    Table max (const std::string&) const;
    Table min (const std::string&) const;
    Table sum (const std::string&) const;
    Table avg (const std::string&) const;
    Table count (const std::string&, const Cell&) const;

    Table aggregate (const Aggregate&) const;

    // Pretty print table
    void print (void) const;

    // Return iterators to sets
    TableArray::iterator begin () const {
        return table.begin ();
    }
    TableArray::iterator end () const {
        return table.end ();
    }
};

#endif
