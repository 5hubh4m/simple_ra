#ifndef RA_PRET_HPP
#define RA_PRET_HPP

#include <map>
#include <string>

#include "table.hpp"

#define PROMPT "R-ALGEBRA>>> "
#define QUIT "quit"

typedef std::map< std::string, Table > Database;

extern Database database;

#endif
