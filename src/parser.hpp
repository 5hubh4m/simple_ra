#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"

const std::vector< std::string > operations = {
    "<=",
    ">=",
    "/=",
    "=",
    "<",
    ">"
};

const std::vector< std::string > unary = {
    "SELECT",
    "PROJECT",
    "RENAME",
    "ASSIGN"
};

const std::vector< char > binary = {
    'X',
    'U',
    '-',
    '^',
    'v',
    '@'
};

int is_val (std::string);
size_t find_unary(std::string s);

Cell parseCell (std::string);
Table parseTuple (std::string);
Predicate parsePredicate (std::string);
Expression* parseExpr (std::string);
#endif
