#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <map>
#include <string>
#include <fstream>

#include "table.hpp"
#include "expression.hpp"

namespace RelationalAlgebra {
    /* Database class
     *
     * The class that manages retrieval and deposition
     * of data onto the file system.
     */
    class Database {
      private:
        // List of commissioned tables by name
        std::vector< std::string > table_list;

        // List of commissioned views by name
        std::map< std::string, std::string > views;

      public:
        // Construct the database
        Database ();

        // Get a list of table and view names for completion
        const std::vector<std::string>& getTableList(void) {
            return table_list;
        }

        const std::map<std::string, std::string>& getViewList(void) {
            return views;
        }


        // Retrieve table by name
        Table operator [] (const std::string&) const;

        // Add table to database
        void add_table (const std::string&, const Table&);

        void add_view (const std::string&, const std::string&);

        // Remove table from database
        void remove (const std::string&);

        // Print DB information to console
        void info (void) const;
    };
};

#endif
