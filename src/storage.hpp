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

    Table operator[] (std::string&);
    void add_table (std::string&, Table&);
    void remove (std::string&);
};


#endif
