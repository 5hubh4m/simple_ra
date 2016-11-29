#ifndef RA_PRET_HPP
#define RA_PRET_HPP

#include <map>
#include <iostream>
#include <string>

#include "storage.hpp"
#include "parser.hpp"

#define DATA       ".data"
#define SCHEMA     "schema"
#define VIEW       "views"
#define SYNTAX     "syntax.ebnf"
#define PROMPT     "R-ALGEBRA>>> "
#define SEC_PROMPT "           > "

namespace RelationalAlgebra {
    extern Database database;
    
    void print_help (void);

    std::string rl_gets(void);

    void showall(void);
};

#endif
