#ifndef RA_PRET_HPP
#define RA_PRET_HPP

#include <map>
#include <iostream>
#include <string>

#include "storage.hpp"
#include "parser.hpp"

#define DATA   ".data"
#define SCHEMA "schema"
#define VIEW  "views"
#define SYNTAX "syntax.ebnf"
#define PROMPT "R-ALGEBRA>>> "

#define QUIT "quit"
#define HELP "help"
#define DROP "drop"
#define SHOWALL "showall"

namespace RelationalAlgebra {
    extern Database database;

    std::string rl_gets ();
    void print_help (void);
};

#endif
