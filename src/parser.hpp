#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"

const std::vector< std::string > operations = {
    LEQ,
    GEQ,
    NEQ,
    EQ,
    LT,
    GT
};

const std::vector< std::string > unary = {
    SELECT,
    PROJECT,
    RENAME,
    ASSIGN
};

const std::vector< std::string > binary = {
    INTERSEC,
    UNION,
    SETDIFF,
    NATJOIN,
    CARTESIAN,
};

const std::vector< std::string > aggregate = {
    MAX,
    MIN,
    AVG,
    SUM,
    COUNT
};

int is_val (std::string);
size_t find_unary(const std::string&);

Cell parseCell (std::string);
Table parseTuple (std::string);
bool hasAgg (const std::vector< std::string >&);
Aggregate parseAgg (std::string);
Predicate parsePredicate (std::string);
Expression* parseExpr (std::string);

#endif
