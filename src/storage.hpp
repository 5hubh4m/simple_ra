#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <map>
#include <string>
#include <fstream>

#include "table.hpp"

class Database {
  private:
    std::vector< std::string > table_list;

  public:
    Database ();

    Table operator[] (const std::string&) const;
    void add_table (const std::string&, const Table&);
    void remove (const std::string&);
};


#endif
