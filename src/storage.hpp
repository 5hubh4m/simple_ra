#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <map>
#include <string>
#include <fstream>

#include "table.hpp"

/* Database class
 *
 * The class that manages retrieval and deposition
 * of data onto the file system.
 */
class Database {
  private:
    // List of commisioned tables by name
    std::vector< std::string > table_list;

  public:
    // Construct the database
    Database ();

    // Retrieve table by name
    Table operator[] (const std::string&) const;

    // Add table to database
    void add_table (const std::string&, const Table&);

    // Remove table from database
    void remove (const std::string&);

    // Print DB information to console
    void info (void) const;
};


#endif
