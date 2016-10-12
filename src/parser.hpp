#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"

// List of boolean operations on values
const std::vector< std::string > operations = {
    LEQ,
    GEQ,
    NEQ,
    EQ,
    LT,
    GT
};

// List of unary operations on table
const std::vector< std::string > unary = {
    SELECT,
    PROJECT,
    RENAME,
    ASSIGN,
    STORE
};

// List of binary operations on table
const std::vector< std::string > binary = {
    INTERSEC,
    UNION,
    SETDIFF,
    NATJOIN,
    CARTESIAN,
};

// List of available aggregate functions
const std::vector< std::string > aggregate = {
    MAX,
    MIN,
    AVG,
    SUM,
    COUNT
};

// Determine if a given string is a value
int is_val (std::string);

// Find a unary operator in a string (if exists)
size_t find_unary(const std::string&);

// Determine of the list of columns have aggregate functions
bool hasAgg (const std::vector< std::string >&);

// Parse string for a Cell instance
Cell parseCell (std::string);

// Parse a tuple
Table parseTuple (std::string);

// Parse a string for an aggregate function
Aggregate parseAgg (std::string);

// Parse string for predicate
Predicate parsePredicate (std::string);

// Parse string for expression, **kind of** a Recursive
// Descent parser.
Expression parseExpr (std::string);

#endif
