#ifndef RA_PRET_HPP
#define RA_PRET_HPP

#include <map>
#include <iostream>
#include <string>

#include "table.hpp"
#include "storage.hpp"

#define DATA   ".data"
#define SCHEMA "schema"
#define SYNTAX "syntax.ebnf"
#define PROMPT "R-ALGEBRA>>> "

#define QUIT "quit"
#define HELP "help"
#define DROP "drop"

#define SELECT    "SELECT"
#define PROJECT   "PROJECT"
#define RENAME    "RENAME"
#define INTERSEC  "^"
#define UNION     "U"
#define SETDIFF   "-"
#define NATJOIN   "@"
#define CARTESIAN "X"
#define ASSIGN    "ASSIGN"

extern Database database;

std::string rl_gets ();
void print_help (void);
#endif
